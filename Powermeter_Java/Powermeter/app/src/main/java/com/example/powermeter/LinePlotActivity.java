package com.example.powermeter;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;

import android.util.Log;
import com.github.mikephil.charting.data.LineData;

import androidx.appcompat.app.AppCompatActivity;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.ValueFormatter;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class LinePlotActivity extends AppCompatActivity {
    private static final String TAG = LinePlotActivity.class.getSimpleName();

    private LineChart lineChart;
    private LineDataSet dataSet;
    private LineData lineData;

    private String csvFilePath;

    private Timer updateTimer;
    private int currentFileIndex;
    private BluetoothLE bluetoothLE;
    private BuildActionBar buildActionBar;
    private boolean isConnect = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_line_plot);

        buildActionBar = new BuildActionBar(this);

        bluetoothLE = (BluetoothLE)
                TransferManager.getInstance().getTransferObject("bluetoothLE");


        // Aktualisierung Verbindungsstatus visuell über Icon in der Actionbar
        bluetoothLE.setConnectionListener(new BluetoothLE.ConnectionListener() {
            @Override
            public void onDeviceConnect() {
                isConnect = true;
                buildActionBar.updateConnectionIcon(true);
            }

            @Override
            public void onDeviceDisconnect() {
                isConnect = false;
                buildActionBar.updateConnectionIcon(false);
            }
        });

        lineChart = findViewById(R.id.line_chart);
        csvFilePath = Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_DOWNLOADS) + "/data.csv";

        lineChart.setTouchEnabled(true);
        lineChart.setDragEnabled(true);
        lineChart.setScaleEnabled(true);
        lineChart.setPinchZoom(true);

        Description description = new Description();
        description.setText("Powermeter Plot");
        lineChart.setDescription(description);

        XAxis xAxis = lineChart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        xAxis.setValueFormatter(new TimeAxisValueFormatter());

        YAxis yAxisLeft = lineChart.getAxisLeft();
        yAxisLeft.setDrawGridLines(true);

        YAxis yAxisRight = lineChart.getAxisRight();
        yAxisRight.setEnabled(false);
    }

    @Override
    protected void onResume() {
        super.onResume();
        TransferManager transferManager = TransferManager.getInstance();
        Object isConnectObject = transferManager.getTransferObject("isConnect");
        if (isConnectObject instanceof Boolean) {
            isConnect = (boolean) isConnectObject;
        }
        buildActionBar.updateConnectionIcon(isConnect);
        startDataUpdate();
    }

    @Override
    protected void onPause() {
        super.onPause();
        TransferManager transferManager = TransferManager.getInstance();
        transferManager.addTransferObject("isConnect", isConnect);
        stopDataUpdate();
    }

    private void startDataUpdate() {
        updateTimer = new Timer();
        TimerTask updateTask = new TimerTask() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        List<Entry> newEntries = loadNewChartDataFromSource();
                        if (!newEntries.isEmpty()) {
                            updateChart(newEntries);
                        }
                    }
                });
            }
        };
        updateTimer.scheduleAtFixedRate(updateTask, 0, 2000); // Update every 2 seconds
    }

    private void stopDataUpdate() {
        if (updateTimer != null) {
            updateTimer.cancel();
            updateTimer = null;
        }
    }

    private List<Entry> loadNewChartDataFromSource() {
        List<Entry> entries = new ArrayList<>();

        try {
            File csvFile = new File(csvFilePath);
            FileInputStream fileInputStream = new FileInputStream(csvFile);
            InputStreamReader inputStreamReader = new InputStreamReader(fileInputStream);
            BufferedReader reader = new BufferedReader(inputStreamReader);
            String line;
            float x = lineChart.getData() != null ? lineChart.getData().getXMax() : 0f;
            int dataCount = 0;

            // Skip lines until reaching the end of the file
            while ((line = reader.readLine()) != null) {
                if (dataCount >= currentFileIndex) {
                    String[] values = line.split(",");
                    double y = bluetoothLE.calcForce(Short.parseShort(values[0]));

                    Entry entry = new Entry(x, (float) y);
                    entries.add(entry);

                    x += 0.005f; // Increase x value by 5 milliseconds
                }
                dataCount++;
            }

            reader.close();
            currentFileIndex = dataCount;

        } catch (IOException e) {
            Log.e(TAG, "Error loading CSV file: " + e.getMessage());
        }

        return entries;
    }



    private void updateChart(List<Entry> newEntries) {
        if (dataSet == null) {
            dataSet = new LineDataSet(newEntries, "Data");
            dataSet.setColor(Color.BLUE);
            dataSet.setLineWidth(2f);
            dataSet.setDrawCircles(false);
            lineData = new LineData(dataSet);
            lineChart.setData(lineData);
        } else {
            for (Entry entry : newEntries) {
                lineData.addEntry(entry, 0);
            }
            dataSet.removeFirst();

            lineData.notifyDataChanged();
            lineChart.notifyDataSetChanged();
        }
        lineChart.invalidate();
        lineChart.setVisibleXRangeMaximum(1);
        lineChart.moveViewToX(lineData.getEntryCount() - 1);
    }



    private class TimeAxisValueFormatter extends ValueFormatter {
        @Override
        public String getFormattedValue(float value) {
            int milliseconds = (int) (value * 1000);
            int minutes = milliseconds / (60 * 1000);
            int seconds = (milliseconds % (60 * 1000)) / 1000;
            int millis = milliseconds % 1000;
            return String.format("%02d:%02d:%03d", minutes, seconds, millis);
        }
    }
}










import android.content.Context;
import android.graphics.Color;
import android.os.Handler;
import android.util.Log;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.IAxisValueFormatter;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

public class Plotter {
    private static final String TAG = "DataPlotter";
    private static final String FILE_NAME = "received_data.bin";
    private static final int DATA_POINTS_COUNT = 4;
    private static final int X_AXIS_INTERVAL = 5; // in milliseconds

    private Context context;
    private LineChart lineChart;
    private List<Entry> entries;
    private Handler handler;

    public DataPlotter(Context context, LineChart lineChart) {
        this.context = context;
        this.lineChart = lineChart;
        this.entries = new ArrayList<>();
        this.handler = new Handler();
    }

    public void startPlotting() {
        // Set up chart settings
        lineChart.getDescription().setEnabled(false);
        lineChart.setTouchEnabled(false);
        lineChart.setDragEnabled(false);
        lineChart.setScaleEnabled(false);
        lineChart.setPinchZoom(false);
        lineChart.setDrawGridBackground(false);
        lineChart.getLegend().setEnabled(false);

        XAxis xAxis = lineChart.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        xAxis.setDrawGridLines(false);
        xAxis.setGranularity(X_AXIS_INTERVAL);
        xAxis.setValueFormatter(new IAxisValueFormatter() {
            @Override
            public String getFormattedValue(float value, AxisBase axis) {
                return String.format("%.1f", value);
            }
        });

        YAxis yAxis = lineChart.getAxisLeft();
        yAxis.setDrawGridLines(true);
        yAxis.setGridColor(Color.LTGRAY);
        yAxis.setAxisMinimum(0);
        yAxis.setAxisMaximum(255);

        lineChart.getAxisRight().setEnabled(false);

        // Start updating the chart
        handler.postDelayed(updateChartRunnable, 0);
    }

    public void stopPlotting() {
        handler.removeCallbacks(updateChartRunnable);
    }

    private Runnable updateChartRunnable = new Runnable() {
        @Override
        public void run() {
            updateChart();
            handler.postDelayed(this, X_AXIS_INTERVAL);
        }
    };

    private void updateChart() {
        try {
            File file = new File(context.getFilesDir(), FILE_NAME);
            FileInputStream fis = new FileInputStream(file);

            byte[] buffer = new byte[DATA_POINTS_COUNT];
            int bytesRead = fis.read(buffer);
            fis.close();

            if (bytesRead == DATA_POINTS_COUNT) {
                // Extract the relevant data
                ByteBuffer byteBuffer = ByteBuffer.wrap(buffer);
                byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
                int data1 = byteBuffer.getShort() & 0xFFFF;
                int data2 = byteBuffer.getShort() & 0xFFFF;

                // Add data to entries list
                entries.add(new Entry(entries.size(), data1));
                entries.add(new Entry(entries.size(), data2));

                // Update the chart data
                LineDataSet dataSet = new LineDataSet(entries, "Data");
                dataSet.setDrawValues(false);
                dataSet.setDrawCircles(false);
                dataSet.setColor(Color.BLUE);
                dataSet.setLineWidth(2f);

                LineData lineData = new LineData(dataSet);
                lineChart.setData(lineData);
                lineChart.invalidate();
            }
        } catch (IOException e) {
            Log.e(TAG, "Error reading file: " + e.getMessage());
        }
    }
}

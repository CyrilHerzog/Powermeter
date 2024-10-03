package com.example.powermeter;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.View;
import androidx.appcompat.app.AppCompatActivity;
import android.bluetooth.BluetoothDevice;
import android.os.Handler;
import android.os.Looper;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;


import java.util.List;

public class BleConActivity extends AppCompatActivity {

    private static final String Tag = "BleConActivity";

    private Button scanButton;
    private TextView selectDeviceText;
    private ListView deviceListView;
    private ArrayAdapter<String> arrayAdapter;
    private Handler handler;
    private Runnable scanRunnable;
    private BluetoothLE bluetoothLE;
    private BuildActionBar buildActionBar;
    private boolean isConnect = false;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble_connect);

        // Rotation blockieren
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        buildActionBar = new BuildActionBar(this);

        bluetoothLE = (BluetoothLE)
                TransferManager.getInstance().getTransferObject("bluetoothLE");


        deviceListView   = findViewById(R.id.device_list);
        scanButton       = findViewById(R.id.scan_button);
        selectDeviceText = findViewById(R.id.selected_device_text);


        SharedPreferences sharedPreferences = getSharedPreferences(
                "PowermeterPreference", Context.MODE_PRIVATE);
        String DeviceName = sharedPreferences.getString("DeviceName", "");
        selectDeviceText.setText(DeviceName);

        handler = new Handler(Looper.getMainLooper());


        arrayAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1);
        deviceListView.setAdapter(arrayAdapter);




        bluetoothLE.setScanListener(new BluetoothLE.ScanListener() {
            @Override
            public void onDeviceFound(List<BluetoothDevice> devices) {
                arrayAdapter.clear();
                for (BluetoothDevice device : devices) {
                   // if (ActivityCompat.checkSelfPermission(BleConActivity.this, android
                   //         .Manifest.permission.BLUETOOTH_CONNECT) != PackageManager
                   //         .PERMISSION_GRANTED) {
                   //     return;
                   // }
                    String deviceName = device.getName();
                    if(deviceName == null){
                        deviceName = "Unknown";
                    }
                    arrayAdapter.add(deviceName + "\n" + device.getAddress());
                }
            }
        });

        scanButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                bluetoothLE.stopConnectToDevice();
                if (bluetoothLE.getScanState()) {
                    stopScanning();
                }else {
                    startScanning();
                }
            }
        });

        deviceListView.setOnItemClickListener(new AdapterView.OnItemClickListener(){
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id){
                String deviceName = arrayAdapter.getItem(position);
                if (deviceName != null) {

                    bluetoothLE.stopConnectToDevice();
                    // Gerät anzeigen
                    selectDeviceText.setText(deviceName);

                    // Adresse setzen für Verbindungsaufbau
                    bluetoothLE.setDeviceAddress(deviceName);

                    // Speichere selektiertes Gerät "ScanListDevice" in persistenten Speicher
                    SharedPreferences sharedPreferences = getSharedPreferences(
                            "PowermeterPreference", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPreferences.edit();
                    editor.putString("DeviceName", deviceName);
                    editor.apply();

                    bluetoothLE.connectToDevice();
                }
            }
        });


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
    }

    @Override
    protected void onPause() {
        super.onPause();
        TransferManager transferManager = TransferManager.getInstance();
        transferManager.addTransferObject("isConnect", isConnect);
    }


    public void startScanning() {
        if (scanRunnable != null) {
            handler.removeCallbacks(scanRunnable);
        }

        bluetoothLE.startScan();
        scanButton.setText("Stop");
        scanRunnable = new Runnable() {
            @Override
            public void run() {
                stopScanning();
            }
        };
        handler.postDelayed(scanRunnable, 10000);
    }

    public void stopScanning(){
        bluetoothLE.stopScan();
        scanButton.setText("Scan");
        if (scanRunnable != null) {
            handler.removeCallbacks(scanRunnable);
        }
    }

}


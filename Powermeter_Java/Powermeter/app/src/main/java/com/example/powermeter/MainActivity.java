package com.example.powermeter;


import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.View;



public class MainActivity extends AppCompatActivity {
    private static final String Tag = "MainActivity";

    // Variablen
    private CardView exitButton;
    private CardView displayButton;
    private CardView bleButton;
    private CardView justageButton;
    private CardView linePlotButton;

    private BuildActionBar buildActionBar;
    private BluetoothLE bluetoothLE;
    private boolean isConnect = false;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Rotation blockieren
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        buildActionBar = new BuildActionBar(this);

        bluetoothLE = new BluetoothLE(this);

        // Aktualisierung Verbindungsstatus visuel über Icon in der Actionbar
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

        SharedPreferences sharedPreferences = getSharedPreferences("PowermeterPreference",
                Context.MODE_PRIVATE);
        String scaleMin = sharedPreferences.getString("ScaleMin", "");
        String scaleMax = sharedPreferences.getString("ScaleMax", "");
        if (!scaleMin.isEmpty() && !scaleMax.isEmpty()) {
            bluetoothLE.setForceScale(Double.parseDouble(scaleMin), Double.parseDouble(scaleMax));
        } else {
            bluetoothLE.setForceScale(0.0, 200.0);
        }


        // Taster verlinken
        exitButton     = findViewById(R.id.card_exit_button);
        displayButton  = findViewById(R.id.card_monitor_button);
        bleButton      = findViewById(R.id.card_ble_button);
        justageButton  = findViewById(R.id.card_justage_button);
        linePlotButton = findViewById(R.id.card_line_plot_button);


        // Exit Taster Listener
        exitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bluetoothLE.stopConnectToDevice();
                bluetoothLE.disconnectFromDevice();
                finishAffinity();
            }
        });

        //Monitor Taster Listener
        displayButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TransferManager.getInstance().addTransferObject("bluetoothLE", bluetoothLE);
                TransferManager.getInstance().addTransferObject("isConnect", isConnect);
                Intent intent = new Intent(MainActivity.this, MonitorActivity.class);
                startActivity(intent);
            }
        });

        // Bluetooth-Gerät suchen
        bleButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TransferManager.getInstance().addTransferObject("bluetoothLE", bluetoothLE);
                TransferManager.getInstance().addTransferObject("isConnect", isConnect);
                Intent intent = new Intent(MainActivity.this, BleConActivity.class);
                startActivity(intent);
            }
        });

        // Justage Fenster öffnen
        justageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TransferManager.getInstance().addTransferObject("bluetoothLE", bluetoothLE);
                TransferManager.getInstance().addTransferObject("isConnect", isConnect);
                Intent intent = new Intent(MainActivity.this, JustageActivity.class);
                startActivity(intent);
            }
        });

        // Line Plot Fenster öffnen
        linePlotButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TransferManager.getInstance().addTransferObject("bluetoothLE", bluetoothLE);
                TransferManager.getInstance().addTransferObject("isConnect", isConnect);
                Intent intent = new Intent(MainActivity.this, LinePlotActivity.class);
                startActivity(intent);
            }
        });


    }

    @Override
    protected void onResume() {
        super.onResume();
        SharedPreferences sharedPreferences = getSharedPreferences("PowermeterPreference",
                Context.MODE_PRIVATE);
        String deviceName = sharedPreferences.getString("DeviceName", "");
        if (!deviceName.isEmpty()) {
            bluetoothLE.setDeviceAddress(deviceName);
            bluetoothLE.connectToDevice();
        } else {
            // no more error handling => the user must just select an device in this case
            Log.e(Tag,"No Device Address available");
        }

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

    }

}
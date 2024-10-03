package com.example.powermeter;



import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.PopupMenu;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;

import java.text.DecimalFormat;
import java.util.Timer;
import java.util.TimerTask;


public class MonitorActivity extends AppCompatActivity {

    private CardView cardViewPower;
    private CardView cardViewAveragePower;
    private CardView cardViewMaxPower;
    private CardView cardViewEnergy;
    private CardView cardViewFrequency;
    private TextView textViewFrequency;
    private CardView cardViewDegree;
    private CardView cardViewForce;
    private TextView textViewForce;
    private TextView textViewPower;
    private RelativeLayout relativeLayout;
    private DecimalFormat decimalFormat;
    private Timer timer;

    private BluetoothLE bluetoothLE;
    private BuildActionBar buildActionBar;
    private boolean isConnect = false;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_monitor);

        // Rotation blockieren
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        buildActionBar = new BuildActionBar(this);

        bluetoothLE = (BluetoothLE)
                TransferManager.getInstance().getTransferObject("bluetoothLE");


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

        cardViewPower        = findViewById(R.id.card_power);
        cardViewAveragePower = findViewById(R.id.card_average_power);
        cardViewMaxPower     = findViewById(R.id.card_max_power);
        cardViewEnergy       = findViewById(R.id.card_energy);
        cardViewFrequency    = findViewById(R.id.card_frequency);
        cardViewDegree       = findViewById(R.id.card_degree);
        cardViewForce        = findViewById(R.id.card_force);

        textViewForce        = findViewById(R.id.value_force);
        textViewFrequency    = findViewById(R.id.value_frequency);
        textViewPower        = findViewById(R.id.value_power);

        relativeLayout = findViewById(R.id.layout_monitor);
        relativeLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showCardMenu(v);
            }
        });

    }


    private void showCardMenu(View view) {
        PopupMenu popupMenu = new PopupMenu(MonitorActivity.this, view);
        MenuInflater inflater = popupMenu.getMenuInflater();
        inflater.inflate(R.menu.menu_monitor, popupMenu.getMenu());

        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.menu_item_power:
                        toggleCardVisibility(cardViewPower);
                        return true;
                    case R.id.menu_item_average_power:
                        toggleCardVisibility(cardViewAveragePower);
                        return true;
                    case R.id.menu_item_max_power:
                        toggleCardVisibility(cardViewMaxPower);
                        return true;
                    case R.id.menu_item_energy:
                        toggleCardVisibility(cardViewEnergy);
                        return true;
                    case R.id.menu_item_frequency:
                        toggleCardVisibility(cardViewFrequency);
                        return true;
                    case R.id.menu_item_degree:
                        toggleCardVisibility(cardViewDegree);
                        return true;
                    case R.id.menu_item_force:
                        toggleCardVisibility(cardViewForce);
                        return true;
                    default:
                        return false;
                }
            }
        });

        popupMenu.show();
    }


    private void toggleCardVisibility(CardView cardView) {
        if (cardView.getVisibility() == View.GONE) {
            cardView.setVisibility(View.VISIBLE);
        } else {
            cardView.setVisibility(View.GONE);
        }
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

        startUpdatingValue();
    }

    @Override
    protected void onPause() {
        super.onPause();
        TransferManager transferManager = TransferManager.getInstance();
        transferManager.addTransferObject("isConnect", isConnect);
        stopUpdatingValue();
    }

    // ...

    private void startUpdatingValue() {
        decimalFormat = new DecimalFormat("#0.0");
        timer = new Timer();
        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                // Check if the MonitorActivity is visible before updating the value
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            textViewForce.setText(decimalFormat.format(bluetoothLE.getForce()));
                            textViewFrequency.setText(decimalFormat.format(bluetoothLE.getCadence()));
                            textViewFrequency.setText(decimalFormat.format(bluetoothLE.getPower()));


                        }
                    });

            }
        };
        timer.scheduleAtFixedRate(task, 0, 1000);
    }

    private void stopUpdatingValue() {
        if (timer != null) {
            timer.cancel(); // Timer stoppen
            timer = null; // Timer-Referenz auf null setzen
        }
    }

}







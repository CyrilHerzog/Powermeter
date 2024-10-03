package com.example.powermeter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.text.DecimalFormat;
import java.util.Timer;
import java.util.TimerTask;

public class JustageActivity extends AppCompatActivity {

    private Timer timer;
    private DecimalFormat decimalFormat;
    private TimerTask task;

    private Button btnScaleSave;
    private EditText editTextScaleMin;
    private EditText editTextScaleMax;
    private TextView textViewVoltage;
    private TextView textViewForce;
    private Button btnPotInc;
    private Button btnPotDec;
    private Spinner spinnerPot;
    private double scaleMinVal;
    private double scaleMaxVal;
    private byte selectPot;

    private BluetoothLE bluetoothLE;
    private BuildActionBar buildActionBar;
    private boolean isConnect = false;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_justage);

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


        btnScaleSave      = findViewById(R.id.scale_save_button);
        editTextScaleMin  = findViewById(R.id.value_scale_min);
        editTextScaleMax  = findViewById(R.id.value_scale_max);
        textViewVoltage   = findViewById(R.id.value_voltage);
        textViewForce     = findViewById(R.id.value_force);
        btnPotInc         = findViewById(R.id.pot_inc_button);
        btnPotDec         = findViewById(R.id.pot_dec_button);
        spinnerPot        = findViewById(R.id.pot_spinner);

        RelativeLayout backgroundLayout = findViewById(R.id.layout_justage);

        ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(this,
                R.array.potentiometer_items, android.R.layout.simple_spinner_item);
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerPot.setAdapter(spinnerAdapter);

        backgroundLayout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                // Hide the keyboard
                hideInput(v);

                return false;
            }
        });



        editTextScaleMin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                editTextScaleMin.setFocusableInTouchMode(true);
                editTextScaleMin.requestFocus();
                InputMethodManager inputMethodManager =
                        (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                inputMethodManager.showSoftInput(editTextScaleMin, InputMethodManager.SHOW_IMPLICIT);
            }
        });

        editTextScaleMin.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // Not needed in this case
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // Update the userInput variable with the entered text
                try {
                    scaleMinVal = Double.parseDouble(s.toString());

                } catch (NumberFormatException e) {
                    editTextScaleMin.setText(String.valueOf(scaleMinVal));
                }
            }

            @Override
            public void afterTextChanged(Editable s) {
            }

        });

        editTextScaleMax.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                editTextScaleMax.setFocusableInTouchMode(true);
                editTextScaleMax.requestFocus();
                InputMethodManager inputMethodManager =
                        (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                inputMethodManager.showSoftInput(editTextScaleMax, InputMethodManager.SHOW_IMPLICIT);
            }
        });

        editTextScaleMax.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // Not needed in this case
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // Update the userInput variable with the entered text
                try {
                    scaleMaxVal = Double.parseDouble(s.toString());

                } catch (NumberFormatException e) {
                    editTextScaleMax.setText(String.valueOf(scaleMaxVal));
                }

            }


            @Override
            public void afterTextChanged(Editable s) {

            }

        });

        btnScaleSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SharedPreferences sharedPreferences = getSharedPreferences(
                        "PowermeterPreference", Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putString("ScaleMin", String.valueOf(scaleMinVal));
                editor.putString("ScaleMax", String.valueOf(scaleMaxVal));
                editor.apply();

                hideInput(v);

                bluetoothLE.setForceScale(scaleMinVal, scaleMaxVal);

            }
        });

        spinnerPot.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String[] potentiometerItems = getResources().
                        getStringArray(R.array.potentiometer_items);
                String selectedOption = potentiometerItems[position];



                if (selectedOption.equals("Gain")) {
                    // Assign value for Gain
                    selectPot = 0;
                } else {
                    selectPot = 8;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        

        btnPotInc.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                byte inst = (byte) (selectPot | 0x01);
                byte val = 0;
                bluetoothLE.setCommand(inst, val);

            }
        });

        btnPotDec.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                byte inst = (byte) (selectPot | 0x02);
                byte val = 0;
                bluetoothLE.setCommand(inst, val);
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

        SharedPreferences sharedPreferences = getSharedPreferences("PowermeterPreference",
                Context.MODE_PRIVATE);
        String scaleMin = sharedPreferences.getString("ScaleMin", "");
        if (!scaleMin.isEmpty()) {
            scaleMinVal = Double.parseDouble(scaleMin);
        } else {
            scaleMinVal = 0.0;
        }

        String scaleMax = sharedPreferences.getString("ScaleMax", "");
        if (!scaleMin.isEmpty()) {
            scaleMaxVal = Double.parseDouble(scaleMax);
        } else {
            scaleMaxVal = 0.0;
        }

        editTextScaleMin.setText(scaleMin);
        editTextScaleMax.setText(scaleMax);

        startUpdatingDisplayValue();

    }

    private void hideInput(View v) {
        InputMethodManager inputMethodManager =
                (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(v.getWindowToken(), 0);
        editTextScaleMin.clearFocus();
        editTextScaleMax.clearFocus();
    }

    private void startUpdatingDisplayValue() {
        // Create a new Timer
        timer = new Timer();
        decimalFormat = new DecimalFormat("#0.0");

        // Create a new TimerTask
        task = new TimerTask() {
            @Override
            public void run() {
                // Check if the MonitorActivity is visible before updating the value
                if (!isFinishing()) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            textViewForce.setText(decimalFormat.format(bluetoothLE.getForce()));
                            textViewVoltage.setText(String.valueOf(bluetoothLE.getAdcVal()));
                        }
                    });
                }
            }
        };

        // Schedule the TimerTask to run periodically
        timer.scheduleAtFixedRate(task, 0, 1000); // Update every 1 second
    }



    @Override
    protected void onPause() {
        super.onPause();
        TransferManager transferManager = TransferManager.getInstance();
        transferManager.addTransferObject("isConnect", isConnect);

    }


}

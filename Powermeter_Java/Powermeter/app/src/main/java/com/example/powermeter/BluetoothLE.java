/*
13.05.23
Herzog Cyril


 */


package com.example.powermeter;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.sqlite.SQLiteDatabase;
import android.os.Environment;
import android.util.Log;


import androidx.core.app.ActivityCompat;

import java.io.FileWriter;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class BluetoothLE implements Serializable {
    private static final String Tag = "BluetoothLE";
    private static final UUID SERVICE_UUID = UUID.fromString
            ("00035b03-58e6-07dd-021a-08123a000300");
    private static final UUID CHARACTERISTIC_UUID = UUID.fromString
            ("00035b03-58e6-07dd-021a-08123a000301");




    private static final byte APP_TRANSFER_CMD = (byte) 0b11000000;

    private static final int REQUEST_ENABLE_BLE = 1;
    private Context context;
    private BluetoothManager bluetoothManager;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic bluetoothGattCharacteristic;

    private ScanSettings scanSettings;
    private List<ScanFilter> scanFilterList;
    private List<BluetoothDevice> deviceList;
    private ScanListener scanListener;
    private ConnectionListener connectionListener;

    private boolean isScanning = false;
    private boolean isConnected = false;
    private boolean isTryToConnect = false;
    private String deviceAddress;

    private short adcVal = 0;
    private short dpsAxisZ = 0;
    private double scaleMinVal;
    private double scaleMaxVal;
    private  String filePath;
    private double dpsIntegral;
    private double forceIntegral;


    public BluetoothLE(Context context) {
        this.context = context;

        if (this.context.checkSelfPermission(android.Manifest.permission
                .ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            final AlertDialog.Builder builder = new AlertDialog.Builder(this.context);
            builder.setTitle("Diese Applikation benötigt Standortzugriff");
            builder.setMessage("Bitte den Standortzugriff erlauben");
            builder.setPositiveButton(android.R.string.ok, null);
            builder.show();
        }


        bluetoothManager = (BluetoothManager) this.context
                .getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
        if (bluetoothAdapter == null) {
            Log.e(Tag, "Bluetooth is not supported on this device");
        }
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            if (this.context instanceof Activity) {
                if (ActivityCompat.checkSelfPermission(this.context, android.Manifest
                        .permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
                ((Activity) this.context).startActivityForResult(enableBtIntent,
                        REQUEST_ENABLE_BLE);
            }
        }
        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();

        scanSettings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_POWER).build();

        scanFilterList = new ArrayList<>();
        deviceList = new ArrayList<>();
    }

    public void startScan() {
        if (isScanning) {
            return;
        }
        deviceList.clear();

        if (ActivityCompat.checkSelfPermission(context, android.Manifest
                .permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        bluetoothLeScanner.startScan(scanFilterList, scanSettings, leScanCallback);
        Log.d(Tag, "Device has start scanning");
        isScanning = true;
    }

    public void stopScan() {
        if (ActivityCompat.checkSelfPermission(context, android.Manifest
                .permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        bluetoothLeScanner.stopScan(leScanCallback);
        Log.d(Tag, "Device has stop scanning");
        isScanning = false;
    }

    public boolean getConnectState() {
        return isConnected;
    }

    public boolean getScanState() {
        return isScanning;
    }

    public List<BluetoothDevice> getDeviceList() {
        return deviceList;
    }

    public void setDeviceAddress(String deviceName) {
        String[] itemParts = deviceName.split("\n");
        String addressPart = itemParts[1].trim();
        this.deviceAddress = addressPart;
        Log.d(Tag, "The Address of selected Device is: " + addressPart);
    }

    public interface ScanListener {
        void onDeviceFound(List<BluetoothDevice> devices);
    }

    public void setScanListener(ScanListener listener) {
        this.scanListener = listener;
    }

    public interface ConnectionListener {
        void onDeviceConnect();

        void onDeviceDisconnect();
    }

    public void setConnectionListener(ConnectionListener listener) {
        this.connectionListener = listener;
    }


    public void connectToDevice() {
        if (!isTryToConnect && !isConnected) {
            isTryToConnect = true;

            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (isTryToConnect) {
                        if (!isConnected) {
                            BluetoothDevice device = bluetoothAdapter.
                                    getRemoteDevice(deviceAddress);
                            if (ActivityCompat.checkSelfPermission(context, android.Manifest
                                    .permission.BLUETOOTH_CONNECT) != PackageManager
                                    .PERMISSION_GRANTED) {
                                return;
                            }
                            bluetoothGatt = device.connectGatt(
                                    context, false, leGattCallback);
                        }
                        try {
                            Thread.sleep(2000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }).start();
        }

    }

    public void stopConnectToDevice() {
        isTryToConnect = false;
        isConnected = false;
        disconnectFromDevice();
    }

    public void disconnectFromDevice() {
        if (bluetoothGatt != null) {
            if (ActivityCompat.checkSelfPermission(this.context, android.Manifest
                    .permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                //    return;
            }
            bluetoothGatt.disconnect();
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
    }


    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            BluetoothDevice device = result.getDevice();
            if (!deviceList.contains(device)) {
                deviceList.add(device);
                Log.d(Tag, "A new Device is add to the List");
                if (scanListener != null) {
                    scanListener.onDeviceFound(deviceList);
                }
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            Log.e(Tag, "Scan has a failure");
        }
    };

    private final BluetoothGattCallback leGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothGatt.STATE_CONNECTED) {
                Log.i(Tag, "Connected to GATT server");
                if (ActivityCompat.checkSelfPermission(context, android
                        .Manifest.permission.BLUETOOTH_CONNECT) != PackageManager
                        .PERMISSION_GRANTED) {
                    return;
                }
                gatt.discoverServices();
                isConnected = true;
                if (connectionListener != null) {
                    connectionListener.onDeviceConnect();
                }
            } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                Log.i(Tag, "Disconnected from GATT server");
                isConnected = false;
                if (connectionListener != null) {
                    connectionListener.onDeviceDisconnect();
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                BluetoothGattService service = gatt.getService(SERVICE_UUID);
                if (service != null) {
                    bluetoothGattCharacteristic = service.getCharacteristic(CHARACTERISTIC_UUID);
                    if (bluetoothGattCharacteristic != null) {
                        if (ActivityCompat.checkSelfPermission(context, android
                                .Manifest.permission.BLUETOOTH_CONNECT) != PackageManager
                                .PERMISSION_GRANTED) {
                            return;
                        }
                        gatt.setCharacteristicNotification(bluetoothGattCharacteristic, true);

                        int properties = bluetoothGattCharacteristic.getProperties();
                        if ((properties & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0) {
                            BluetoothGattDescriptor descriptor = bluetoothGattCharacteristic
                                    .getDescriptor(UUID.fromString(
                                            "00002902-0000-1000-8000-00805f9b34fb"));
                            if (descriptor != null) {
                                descriptor.setValue(BluetoothGattDescriptor
                                        .ENABLE_NOTIFICATION_VALUE);
                                gatt.writeDescriptor(descriptor);
                            }
                        }
                    }
                }
            }
        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                // Notifications/Indications enabled successfully
                BluetoothGattCharacteristic characteristic = descriptor.getCharacteristic();
                if (characteristic != null) {
                    // Check if it's the characteristic we're interested in
                    if (CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                        // Now notifications are enabled for the characteristic
                        Log.d(Tag, "Notifications enabled");
                    }
                }
            } else {
                // Failed to enable notifications/indications
                Log.e(Tag, "Failed to enable Notifications/Indications");
            }
        }

        private boolean receivingData = false;
        private byte[] receivedData = new byte[255];
        private int receivedIndex = 0;

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            byte[] data = characteristic.getValue();
            for (byte value : data) {
                if (!receivingData) {
                    if (value == (byte) APP_TRANSFER_CMD) {
                        receivingData = true;
                        receivedIndex = 0;
                        receivedData[receivedIndex++] = value;
                    }
                } else {
                    receivedData[receivedIndex++] = value;

                    if (receivedIndex == 6) {
                        // if command detected
                        if (receivedData[5] == 10) {
                            // extract payload
                            byte[] payLoad = new byte[4];
                            System.arraycopy(receivedData, 1, payLoad, 0, 4);

                            adcVal   = (short) (((payLoad[1] & 0xFF) << 8) | (payLoad[0] & 0xFF));
                            dpsAxisZ = (short) (((payLoad[3] & 0xFF) << 8) | (payLoad[2] & 0xFF));
                            //Log.d(Tag, "adc" + adcVal);
                            //Log.d(Tag, "dps" + dpsAxisZ);
                            writeDataToCSVFile(adcVal, dpsAxisZ);



                        }

                        // Reset the Variables
                        receivingData = false;
                        receivedIndex = 0;
                    }
                }
            }
        }
    };

    private void writeDataToCSVFile(short adcVal, short dpsAxisZ) {
        try {
            String csvFilePath = Environment.getExternalStoragePublicDirectory(
                    Environment.DIRECTORY_DOWNLOADS) + "/data.csv";

            FileWriter writer = new FileWriter(csvFilePath, true);

            // Schreiben Sie die Daten in die CSV-Datei
            writer.append(String.valueOf(adcVal));
            writer.append(",");
            writer.append(String.valueOf(dpsAxisZ));
            writer.append("\n");

            // Schließen Sie den Writer
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private void writeData(byte[] data) {
            if (bluetoothGatt != null && bluetoothGattCharacteristic != null) {
                bluetoothGattCharacteristic.setValue(data);

                if (ActivityCompat.checkSelfPermission(context, android.Manifest.permission
                        .BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
                boolean success = bluetoothGatt.writeCharacteristic(bluetoothGattCharacteristic);
                if (success) {
                    Log.d(Tag, "Data write operation initiated successfully");
                } else {
                    Log.e(Tag, "Failed to initiate data write operation");
                }
            } else {
                Log.e(Tag, "Bluetooth GATT or characteristic is null");
            }
        }

        public void setCommand(byte instruction, byte val) {
            byte[] data = new byte[5];
            data[0] = APP_TRANSFER_CMD;
            data[1] = (byte) (instruction | 0x80);
            data[2] = 0;
            data[3] = 13;
            data[4] = 10;
            writeData(data);
        }


        public short getAdcVal() {
            return adcVal;
        }

        public double getCadence() {
            return (((((double) dpsAxisZ / 70.0) * 2000.0) / 32767.0) / (2.0 * Math.PI));
        }

        public double getPower() {
            double force = (((double) adcVal * (scaleMaxVal - scaleMinVal)) / 3300.0) + scaleMinVal;
            return (((((double) dpsAxisZ / 70.0) * 2000.0) / 32767.0) / (Math.PI) * (double) force * 0.17);
        }

        public void setForceScale(double scaleMinVal, double scaleMaxVal) {
            this.scaleMinVal = scaleMinVal;
            this.scaleMaxVal = scaleMaxVal;
        }

        public double getForce() {
            return (((double) adcVal * (scaleMaxVal - scaleMinVal)) / 3300.0) + scaleMinVal;
        }

        public double calcForce(short x) {
            return (((double) x * (scaleMaxVal - scaleMinVal)) / 3300.0) + scaleMinVal;
        }

    }




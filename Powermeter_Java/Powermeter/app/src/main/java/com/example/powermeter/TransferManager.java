package com.example.powermeter;

import java.util.HashMap;
import java.util.Map;

public class TransferManager {
    private static TransferManager instance;
    private Map<String, Object> transferObjects;

    private TransferManager() {
        transferObjects = new HashMap<>();
    }

    public static TransferManager getInstance() {
        if (instance == null) {
            instance = new TransferManager();
        }
        return instance;
    }

    public void addTransferObject(String key, Object object) {
        transferObjects.put(key, object);
    }

    public Object getTransferObject(String key) {
        return transferObjects.get(key);
    }

    public void removeTransferObject(String key) {
        transferObjects.remove(key);
    }
}

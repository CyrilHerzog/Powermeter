package com.example.powermeter;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;



public class BuildActionBar {
    private AppCompatActivity activity;
    private ImageView iconConnect;
    private  ImageView iconDisconnect;

    public BuildActionBar(AppCompatActivity activity) {
        this.activity = activity;

        View viewActionBar = LayoutInflater.from(this.activity)
                .inflate(R.layout.build_action_bar, null);
        

        ActionBar actionBar = this.activity.getSupportActionBar();

        if (actionBar != null){
            actionBar.setDisplayOptions(ActionBar.DISPLAY_SHOW_CUSTOM);
            actionBar.setCustomView(viewActionBar);
        }

        iconConnect = viewActionBar.findViewById(R.id.icon_connect);
        iconDisconnect = viewActionBar.findViewById(R.id.icon_disconnect);

        // Default
        updateConnectionIcon(false);

    }


    public void updateConnectionIcon(boolean isConnect) {
        int connectVisibility    = isConnect ? View.VISIBLE : View.INVISIBLE;
        int disconnectVisibility = isConnect ? View.INVISIBLE : View.VISIBLE;

        iconDisconnect.setVisibility(disconnectVisibility);
        iconConnect.setVisibility(connectVisibility);
    }


}

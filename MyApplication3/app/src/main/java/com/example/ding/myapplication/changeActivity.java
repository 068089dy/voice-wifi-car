package com.example.ding.myapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class changeActivity extends AppCompatActivity {
    private Button voice;
    private Button hand;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_change);
        voice = (Button) findViewById(R.id.voice);
        hand = (Button) findViewById(R.id.hand);
        voice.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(changeActivity.this,MainActivity.class);
                startActivity(intent);
            }
        });
        hand.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(changeActivity.this,handActivity.class);
                startActivity(intent);
            }
        });
    }
}

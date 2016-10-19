package com.example.ding.myapplication;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class handActivity extends AppCompatActivity{

    private Button left;
    private Button right;
    private Button front;
    private Button back;
    private String ip = "123.207.174.226";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hand);
        ButtonListener b = new ButtonListener();
        back = (Button) findViewById(R.id.back);
        back.setOnTouchListener(b);
        front = (Button) findViewById(R.id.front);
        front.setOnTouchListener(b);
        left = (Button) findViewById(R.id.left);
        left.setOnTouchListener(b);
        right = (Button) findViewById(R.id.right);
        right.setOnTouchListener(b);
    }
    /*public void onClick(View v){
        switch(v.getId()){
            case R.id.front:
                MyThread tt = new MyThread(0,ip,"前");
                tt.start();
                break;
            case R.id.left:
                MyThread tt1 = new MyThread(0,ip,"左");
                tt1.start();
                break;
            case R.id.back:
                MyThread tt2 = new MyThread(0,ip,"退");
                tt2.start();
                break;
            case R.id.right:
                MyThread tt3 = new MyThread(0,ip,"右");
                tt3.start();
                break;
            case R.id.stop:
                MyThread tt4 = new MyThread(0,ip,"停");
                tt4.start();
                break;
            default:
                break;
        }
    }*/

    class ButtonListener implements View.OnTouchListener {


        public boolean onTouch(View v, MotionEvent event) {
            if(v.getId() == R.id.back){
                if(event.getAction() == MotionEvent.ACTION_UP){
                    new MyThread(ip,"停").start();

                }
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    new MyThread(ip,"后").start();
                }
            }
            if(v.getId() == R.id.front){
                if(event.getAction() == MotionEvent.ACTION_UP){
                    new MyThread(ip,"停").start();

                }
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    new MyThread(ip,"前").start();
                }
            }
            if(v.getId() == R.id.right){
                if(event.getAction() == MotionEvent.ACTION_UP){
                    Log.d("test", "cansal button ---> cancel");
                    new MyThread(ip,"停").start();

                }
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    Log.d("test", "cansal button ---> down");
                    new MyThread(ip,"右").start();
                }
            }
            if(v.getId() == R.id.left){
                if(event.getAction() == MotionEvent.ACTION_UP){
                    Log.d("test", "cansal button ---> cancel");
                    new MyThread(ip,"停").start();

                }
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    Log.d("test", "cansal button ---> down");
                    new MyThread(ip,"左").start();
                }
            }
            return false;
        }

    }

}

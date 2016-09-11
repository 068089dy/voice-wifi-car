package com.example.ding.myapplication;

import android.util.Log;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

/**
 * Created by ding on 16-8-8.
 */
public class MyThread extends Thread {
    String ip;
    String send;
    int num;
    public MyThread(int num,String ip,String send) {
        this.ip = ip;
        this.send = send;
        this.num = num;
    }
    public void run() {
        try{
            Log.v("ip",ip);
            ByteArrayInputStream tInputStringStream = new ByteArrayInputStream(send.getBytes());
            Socket socket = new Socket(ip,8888);
            PrintWriter out = new PrintWriter(socket.getOutputStream());
            //BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            BufferedReader sin = new BufferedReader(new InputStreamReader(tInputStringStream));
            String s;

            s=sin.readLine();
            out.println(s);
            out.flush();
            //System.out.println("污月月2说:"+in.readLine());

            out.close();
            //in.close();
            socket.close();
        }
        catch(Exception e)
        {
            System.out.println(e);
            Log.v("ip",ip);
        }
    }
}

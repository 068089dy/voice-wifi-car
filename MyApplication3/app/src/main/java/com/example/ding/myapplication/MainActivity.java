package com.example.ding.myapplication;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
//import android.widget.EditText;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.List;

import cn.yunzhisheng.common.USCError;
import cn.yunzhisheng.pro.USCRecognizer;
import cn.yunzhisheng.pro.USCRecognizerListener;

public class MainActivity extends AppCompatActivity{

    private USCRecognizer mRecognizer;
    private TextView tx1;
    private TextView mVolume;
    private Button begin;
    private EditText ipedit;
    String ip;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // 创建识别对象，appKey通过 http://dev.hivoice.cn/ 网站申请
        mRecognizer = new USCRecognizer(this, Config.appKey);

        initData();
        initRecognizer();
        begin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mRecognizer.start();
                //Toast.makeText(MainActivity.this, "begin", Toast.LENGTH_SHORT).show();
            }
        });
    }
    /**
     * 初始化识别
     */
    private void initRecognizer() {


        mRecognizer.setVADTimeout(2000,200);
        // 保存录音数据
        // recognizer.setRecordingDataEnable(true);
        //识别对象回调监听
        mRecognizer.setListener(new USCRecognizerListener() {
            public void onResult(String result, boolean isLast) {
                // 识别结果实时返回,保留识别结果组成完整的识别内容。
                tx1.setText(result);
                ip = ipedit.getText().toString();
                MyThread tt = new MyThread(0,ip,result);
                tt.start();
            }
            public void onEnd(USCError error) {
                //识别结束
                if(error == null) {
                    //识别成功,可在此处理text结果
                    Toast.makeText(MainActivity.this, "end", Toast.LENGTH_SHORT).show();
                }
            }

            public void onVADTimeout() {
                //用户停止说话回调
                mRecognizer.stop();
            }
            public void onUpdateVolume(int volume) {
                //实时返回说话音量 0~100
                mVolume.setText("音量："+volume);
            }
            public void onUploadUserData(USCError error) {
                //完成个性化用户数据上传
            }
            public void onRecognizerStart() {
                //录音设备打开识别开始,用户可以开始说话
                tx1.setText("开始说话");
            }
            public void onRecordingStop(List<byte[]> arg0){
                //停止录音,等待识别结果回调
            }
            public void onSpeechStart() {
                //用户开始说话回调
            }
        });
    }
    /**
     * 初始化控件
     */
    private void initData() {

        //mVolume = (ProgressBar) findViewById(R.id.volume_progressbar);
        tx1 = (TextView) findViewById(R.id.tx1);
        mVolume = (TextView) findViewById(R.id.mVolume);
        begin = (Button) findViewById(R.id.beginbt);
        ipedit = (EditText) findViewById(R.id.ipedit);
    }

}

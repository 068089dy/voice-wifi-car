package com.example.ding.myapplication;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
//import android.widget.EditText;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.ProcessingInstruction;
import org.w3c.dom.Text;

import java.util.List;

import cn.yunzhisheng.common.USCError;
import cn.yunzhisheng.pro.USCRecognizer;
import cn.yunzhisheng.pro.USCRecognizerListener;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private USCRecognizer mRecognizer;
    private TextView tx1;
    private TextView mVolume;
    private Button begin;
    private Button stop;
    private Button back;
    private EditText ipedit;
    private ProgressBar mVolume_pro;
    boolean start = true;
    String ip;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // 创建识别对象，appKey通过 http://dev.hivoice.cn/ 网站申请
        mRecognizer = new USCRecognizer(this, Config.appKey);
        initData();
        initRecognizer();

    }
    public void onClick(View v){
        switch(v.getId()){
            case R.id.beginbt:
                mRecognizer.start();
                begin.setEnabled(false);
                stop.setEnabled(true);
                ipedit.setEnabled(false);
                begin.setText("已开启...");
                start = true;
                break;
            case R.id.stop:
                mRecognizer.stop();
                begin.setEnabled(true);
                begin.setText("开始");
                stop.setEnabled(false);
                ipedit.setEnabled(true);
                start = false;
                break;
            case R.id.back:
                mRecognizer.stop();
                start = false;
                finish();
                break;
            default:
                break;
        }
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
                new MyThread(ip,result).start();

            }
            public void onEnd(USCError error) {
                //识别结束
                if(error == null) {
                    //识别成功,可在此处理text结果
                    if(start) {
                        Toast.makeText(MainActivity.this, "end", Toast.LENGTH_SHORT).show();
                        mRecognizer.start();
                    }
                }
            }

            public void onVADTimeout() {
                //用户停止说话回调
                mRecognizer.stop();
                //mRecognizer.sleep(1000);

            }
            public void onUpdateVolume(int volume) {
                //实时返回说话音量 0~100
                mVolume.setText("音量："+volume);
                mVolume_pro.setProgress(volume);
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
        stop = (Button) findViewById(R.id.stop);
        back = (Button) findViewById(R.id.back);
        stop.setEnabled(false);
        back.setOnClickListener(this);
        begin.setOnClickListener(this);
        stop.setOnClickListener(this);
        ipedit = (EditText) findViewById(R.id.ipedit);
        mVolume_pro = (ProgressBar) findViewById(R.id.mVolume_pro);
        mVolume_pro.setVisibility(View.VISIBLE);
        mVolume_pro.setMax(100);
    }
    public void onBackPressed() {
        Intent intent= new Intent(Intent.ACTION_MAIN);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.addCategory(Intent.CATEGORY_HOME);
        startActivity(intent);
    }

}

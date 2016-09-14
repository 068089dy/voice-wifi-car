import java.io.*;
import java.net.*;
import java.util.*;
import java.util.Map.Entry;

public class server{
  public static void main(String args[])throws Exception{
    try{
      //链式散列集用来存储与小车连接的线程0
      Map<String,serverthread> threadList = new HashMap<String,serverthread>();
      ServerSocket server = new ServerSocket(8888);
      while(true){
        Socket socket = server.accept();
        System.out.println("connection ok!");
        new serverthread(socket,threadList).start();
      }
    }
    catch(Exception e){
      //server.close();
    }
  }
}
class serverthread extends Thread{
  Socket socket;
  BufferedReader in = null;
  PrintWriter out;
  BufferedReader sin;
  Map<String,serverthread> threadList;
  static int i = 0;
  String car = null;
  String ist = null;
  public serverthread(Socket socket,Map<String,serverthread> threadList){
    this.socket = socket;
    //this.set = set;
    this.threadList = threadList;
    i++;
    car = "car"+i;
    threadList.put(car,this);
    System.out.println("add suceess!");
  }
  public void run(){
    try{
      out = new PrintWriter(socket.getOutputStream(),true);//发送数据
      in = new BufferedReader(new InputStreamReader(socket.getInputStream()));//获取client端流
      //System.out.println("in ok!"+in.readLine());
      if(in != null){
        String s = in.readLine();
        System.out.println("st:"+s);
        if(s.startsWith("phone")){
          threadList.remove(car);
        }
        Iterator<Entry<String,serverthread>> it = threadList.entrySet().iterator();
        while (it.hasNext()&&s!=null) {
          Map.Entry entry = (Map.Entry) it.next();
          //返回对应到该条目的值
          serverthread st = (serverthread) entry.getValue();
          if(st!=this){

            st.out.println(instruct(s));
            System.out.println("send:"+instruct(s));
            out.println(instruct(s));
          }
        }
      }
      //sin = new BufferedReader(new InputStreamReader(System.in));
      //返回一个包含中条目的规则集（迭代器）
      System.out.println("end");
      in.close();
      out.close();
      socket.close();
    }catch(Exception e){}
  }


  public String instruct(String st){
    for(int j = 0;j<st.length();j++){
      //System.out.println(st.substring(i,i+1));
      String st1 = st.substring(j,j+1);
      System.out.println("st1:"+st1);
      if(st1.equals("前")||st1.equals("钱")||st1.equals("近")||st1.equals("进")){
        System.out.println("result:a");
        return "A";

      }else if(st1.equals("后")||st1.equals("退")){
        System.out.println("result:b");
        return "B";

      }else if(st1.equals("右")||st1.equals("有")){
        System.out.println("result:d");
        return "D";

      }else if(st1.equals("左")||st1.equals("夺")||st1.equals("冠")){
        System.out.println("result:c");
        return "C";

      }else if(st1.equals("停")||st1.equals("傻")||st1.equals("逼")){
        System.out.println("result:i");
        return "I";

      }
    }
    return "null";
  }
}

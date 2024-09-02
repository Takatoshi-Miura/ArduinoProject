#include <MsTimer2.h>

//変数の宣言
int _number=0;
int _cnt=0;
int t=0;

//Arduinoデジタルピン2～9にアノード,10～13にカソードを接続
void setup(){
  for(int n=2 ; n<14 ;n++){
    pinMode(n,OUTPUT);
  }
  MsTimer2::set(2, timerFire);  //2msごとに割り込みを発生させる
  MsTimer2::start();
}

//7セグLEDのレイアウトを定義(小数点なし)
boolean Num_Array[10][8]={
  {1,1,1,1,1,1,0,0}, // 0
  {0,1,1,0,0,0,0,0}, // 1
  {1,1,0,1,1,0,1,0}, // 2
  {1,1,1,1,0,0,1,0}, // 3
  {0,1,1,0,0,1,1,0}, // 4
  {1,0,1,1,0,1,1,0}, // 5
  {1,0,1,1,1,1,1,0}, // 6
  {1,1,1,0,0,1,0,0}, // 7
  {1,1,1,1,1,1,1,0}, // 8
  {1,1,1,1,0,1,1,0}, // 9
};
//7セグLEDのレイアウトを定義(小数点あり)
boolean Num_Array_dp[10][8]={
  {1,1,1,1,1,1,0,1}, // 0.
  {0,1,1,0,0,0,0,1}, // 1.
  {1,1,0,1,1,0,1,1}, // 2.
  {1,1,1,1,0,0,1,1}, // 3.
  {0,1,1,0,0,1,1,1}, // 4.
  {1,0,1,1,0,1,1,1}, // 5.
  {1,0,1,1,1,1,1,1}, // 6.
  {1,1,1,0,0,1,0,1}, // 7.
  {1,1,1,1,1,1,1,1}, // 8.
  {1,1,1,1,0,1,1,1}, // 9.
};

//LED表示関数 (Numberに表示したい数字を入れる)
void NumPrint(int Number){
  for(int w=0; w<=8 ; w++){
    digitalWrite(w+2,-Num_Array[Number][w]);
  }
}

//LED表示関数 (小数点あり)
void NumPrint_dp(int Number){
  for(int w=0; w<=8 ; w++){
    digitalWrite(w+2,-Num_Array_dp[Number][w]);
  }
}

//全LEDをOFF (7セグLEDのアノードはArduinoの2～9番に接続)
void off7SegLED(){
  for(int i=2;i<14;i++){
    digitalWrite(i,LOW);
  }
}

//全LEDをON (7セグLEDのカソードはArduinoの10～13番に接続)
void on7SegLED(){
  for(int i=10;i<14;i++){
    digitalWrite(i,HIGH);
  }  
}


//4桁の数字を各々の桁の数字に分解する関数
int NumParse(int Number,int s){
  int m=0;
  if(s==1){
    return Number % 10; //10で割った余り＝1桁目の数字
  }
  if(s==2){
    m=Number % 100; //100で割った余りをmに代入
    return m / 10; //mを10で割った商＝2桁目の数字
  }
  if(s==3){
    m=Number % 1000; //1000で割った余りをmに代入
    return m / 100; //mを100で割った商＝3桁目の数字
  }
  if(s==4){
    return Number / 1000; //1000で割った商＝4桁目の数字
  }
  return 0;
}

void timerFire(){
       MsTimer2::stop();
       
       on7SegLED();
       t++;
       if(t>=4){
        t=0;
       }
        if(t==0){
          on7SegLED();
          NumPrint(NumParse(_number,1));
          digitalWrite(10,LOW);
          delay(5);
          off7SegLED();
        }
        if(t==1){
          on7SegLED();
          NumPrint(NumParse(_number,2));
          digitalWrite(11,LOW);
          delay(10);
          off7SegLED();
        }
        if(t==2){
          on7SegLED();
          NumPrint(NumParse(_number,3));
          digitalWrite(12,LOW);
          delay(10);
          off7SegLED();
        }
        if(t==3){
          on7SegLED();
          NumPrint(NumParse(_number,4));
          digitalWrite(13,LOW);
          delay(10);
          off7SegLED();
        }
        MsTimer2::start();
       
}


void loop(){
  if(_number >= 10000){
    _number = 0;
  }
  //100まで行ったら表示する数値(_number)を1つ足す(実質delayの10×100で1秒に1カウント)
  if(_cnt >= 100){
    _number++;
    _cnt = 0;
  }
  on7SegLED();
 _cnt++;
// delay(1);
}        


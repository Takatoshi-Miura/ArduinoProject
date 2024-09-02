/****************************************************************************/
/* MP3プレイヤー兼ロータリーエンコーダーテスター　　                        */
/*                                                                          */
/* Arduino megaを使用                                                       */
/*                                                                          */
/*【7セグメントLEDとの接続】                                                */
/* デジタルピン4…A,デジタルピン5…B,デジタルピン6…C,デジタルピン7…D  　  */
/* デジタルピン8…E,デジタルピン9…F,デジタルピン10…G                  　　*/
/* デジタルピン11…DP,デジタルピン12…1桁目カソード　　           　   　　 */
/* デジタルピン13…2桁目カソード,デジタルピン14…3桁目カソード              */
/* デジタルピン15…4桁目カソード                                            */
/*                                                                          */
/****************************************************************************/

/* ライブラリ */
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>


/* 変数の宣言 */
unsigned long millis();
unsigned long pre_Time=0;
unsigned long mission_Time=0;
//unsigned long now_Time=0;

/* ピン設定 */
#define ENC_A 2
#define ENC_B 3
#define Aseg 12
#define Bseg 13
#define Cseg 14
#define Dseg 15
#define SW1 16
#define R_LED 17
#define G_LED 18
#define B_LED 19
#define trig_pin 20
#define echo_pin 21
volatile byte pos;
volatile int  enc_count;
volatile int  pre_enc_count=0;
int Time=0;
static boolean SW_ON_OFF = LOW;

int eric_ueda =0;

double Duration=0;  //受信した間隔
double Distance=0;  //距離


boolean Num_Array[10][7]={   // 7セグLEDのレイアウトを定義(小数点なし)
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,1,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}, // 9
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

/*　初期設定  */
void setup(){
  Serial.begin( 9600 );
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(SW1, INPUT); 
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(echo_pin, INPUT);   //エコー
  pinMode(trig_pin, OUTPUT);  //トリガー
  attachInterrupt(0, ENC_READ, CHANGE);  //2
  attachInterrupt(1, ENC_READ, CHANGE);  //3
  for(int n=4 ; n<16 ;n++){
    pinMode(n,OUTPUT);
  }
  digitalWrite(SW1,LOW);
  mp3_set_serial (Serial);      //set Serial for DFPlayer-mini mp3 module 
  delay(10);                     // delay 1ms to set volume
  mp3_set_volume (25);          // value 0~30
  delay(10);
  mp3_play (1);
  delay(3000);
  LED_OFF();
  pre_Time=millis();
}


/* メインループ */
void loop(){
  
  if(digitalRead(SW1)==HIGH){
    delay(100); // チャタリング対策
    if(SW_ON_OFF==LOW){
      Time=enc_count;
    }
    SW_ON_OFF = !SW_ON_OFF;
    enc_count=Time;
    pre_Time=millis();
    mission_Time=millis();;
    digitalWrite(SW1,LOW);
  }

  if(SW_ON_OFF==HIGH){
    Time=enc_count-(millis()/1000-pre_Time/1000);
    view7SegLED_timer(Time);
    Time_check();
  }

  if(SW_ON_OFF==LOW){
    view7SegLED_timer(enc_count);
  }

  if(Time<0){
    Time=0;
    enc_count=0;
    view7SegLED_timer(0);
  }
  LED_OFF();
  digitalWrite(G_LED,HIGH);
  digitalWrite(R_LED,HIGH);

  Get_Distance();
  if(Distance<200 && Distance!=0){
    eric_ueda++;
    if(eric_ueda==1){
      mp3_play (12);
    }
    LED_OFF();  // G+B=赤,G+R=青
    digitalWrite(G_LED,HIGH);
    digitalWrite(B_LED,HIGH);
  }
  else if(eric_ueda>0){
    eric_ueda=0;
    mp3_stop ();
  }
  else{
    eric_ueda=0;
  }
  
  
}


/* 割り込み処理 */
void ENC_READ() {
  byte cur = (!digitalRead(ENC_B) << 1) + !digitalRead(ENC_A);
  byte old = pos & B00000011;
  byte dir = (pos & B00110000) >> 4;
 
  if (cur == 3) cur = 2;
  else if (cur == 2) cur = 3;
 
  if (cur != old)
  {
    if (dir == 0)
    {
      if (cur == 1 || cur == 3) dir = cur;
    } else {
      if (cur == 0)
      {
        if (dir == 1 && old == 3) enc_count++;
        else if (dir == 3 && old == 1) enc_count--;
        dir = 0;
      }
    }
    if(enc_count<0){
      enc_count=5999;
    }
    if(enc_count>5999){
      enc_count=0;
    }
    view7SegLED_timer(enc_count);
    pos = (dir << 4) + (old << 2) + cur;
  }
}


/* ------------------------------ 以降 関数群 ----------------------------------*/

/* Time_check */
void Time_check(){
  if(( millis()-mission_Time)/1000 ==1) mp3_play(113); // ミッション開始時
  if(( millis()-mission_Time)/1000 ==300) mp3_play(103); // 開始5分経過
  if(( millis()-mission_Time)/1000 ==600) mp3_play(104); // 開始10分経過
  if(( millis()-mission_Time)/1000 ==900) mp3_play(105); // 開始15分経過
  if(( millis()-mission_Time)/1000 ==1200) mp3_play(106); // 開始20分経過
  if(( millis()-mission_Time)/1000 ==1500) mp3_play(107); // 開始25分経過
  if(( millis()-mission_Time)/1000 ==1800) mp3_play(108); // 開始30分経過
  if(( millis()-mission_Time)/1000 ==2100) mp3_play(109); // 開始35分経過
  if(( millis()-mission_Time)/1000 ==2400) mp3_play(110); // 開始40分経過
  if(( millis()-mission_Time)/1000 ==2700) mp3_play(111); // 開始45分経過
  if(( millis()-mission_Time)/1000 ==3000) mp3_play(112); // 開始50分経過
  if(Time==4) mp3_play (10);
  if(Time==31) mp3_play (100);
  if(Time==61) mp3_play (101);
}

/* 距離計測関数 */
void Get_Distance(){
  digitalWrite(trig_pin,HIGH); // トリガーを10μsだけHIGH出力
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);
  Duration=pulseIn(echo_pin,HIGH,5000);  // エコーで受信
  Distance=(Duration/2)*340*1000/1000000;  // 距離計算(音速340m/s)
  Serial.print("Distance:");
  Serial.print(Distance);
  Serial.println("mm");
}

/* LED_OFF */
void LED_OFF(){
  digitalWrite(R_LED,LOW);
  digitalWrite(G_LED,LOW);
  digitalWrite(B_LED,LOW);
}

/* LED_ON */
void LED_ON(){
  digitalWrite(R_LED,HIGH);
  digitalWrite(G_LED,HIGH);
  digitalWrite(B_LED,HIGH);
}

/* LED表示関数 */
void NumPrint(int Number){
  Number=Number-48;
  for(int w=0; w<=6 ; w++){
    digitalWrite(w+4,-Num_Array[Number][w]);
  }
}

/* LED表示関数(小数点あり) */
void NumPrint_dp(int Number){
  Number=Number-48;
  for(int w=0; w<=7 ; w++){
    digitalWrite(w+4,-Num_Array_dp[Number][w]);
  }
}

/* 全LEDをOFF (7セグLEDのアノードはArduinoの4～10番に接続) */
void off7SegLED(){
  for(int i=4;i<12;i++){
    digitalWrite(i,LOW);
  }
}

// 全LEDをON (7セグLEDのカソードはArduinoの11～13番に接続)
void on7SegLED(){
  for(int i=Aseg;i<=Dseg;i++){
    digitalWrite(i,HIGH);
  }  
}

/* view7SegLED タイマーver */
void view7SegLED_timer(int n){
  //String str = String(n);
  String second = String(n);
  String minute = String(n);
  second = n%60;
  minute = n/60;
  //2桁の場合 0を埋める
  if(10 > n%60){
    second = "0"+second;
  }
  if(10 > n/60){
    minute = "0"+minute;
  }
  

  on7SegLED();
  digitalWrite(Dseg,LOW);
  NumPrint(minute.charAt(0));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);

  on7SegLED();
  digitalWrite(Cseg,LOW);
  NumPrint_dp(int(minute.charAt(1)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);

  on7SegLED();
  digitalWrite(Bseg,LOW);
  NumPrint(int(second.charAt(0)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);
  
  on7SegLED();
  digitalWrite(Aseg,LOW);
  NumPrint(int(second.charAt(1)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);
}









/* 0-9999 表示 */
void view7SegLED(int n){
  String str = String(n);
  //3桁の場合 0を埋める
  if(1000 > n && n > 99){
    str = "0"+str;
  }
  //2桁の場合 0を埋める
  if(100 > n && n > 9){
    str = "00"+str;
  }
  //1桁の場合 00を埋める
  if(10 > n){
   str = "000"+str;
  }
  //値がマイナスの場合 00を埋める
  if(0 > n){
    str = "0";
  }
  //Serial.println(str);
  
  on7SegLED();
  digitalWrite(Dseg,LOW);
  NumPrint(str.charAt(0));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);

  on7SegLED();
  digitalWrite(Cseg,LOW);
  NumPrint_dp(int(str.charAt(1)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);

  on7SegLED();
  digitalWrite(Bseg,LOW);
  NumPrint(int(str.charAt(2)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);
  
  on7SegLED();
  digitalWrite(Aseg,LOW);
  NumPrint(int(str.charAt(3)));
  delay(3);
  off7SegLED();
  delayMicroseconds(10);
}




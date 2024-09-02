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
#include <IRremote.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>


/* 変数の宣言 */
unsigned long millis();
unsigned long pre_Time=0;
unsigned long mission_Time=0;
//unsigned long now_Time=0;

/* ピン設定 */
#define ENC_A 19
#define ENC_B 18
#define Aseg 12
#define Bseg 13
#define Cseg 14
#define Dseg 15
#define SW1 27
#define R_LED 24
#define G_LED 25
#define B_LED 26
#define trig_pin 22
#define echo_pin 23

//IRsend irsend;
//int RECV_PIN = 28;
//IRrecv irrecv(RECV_PIN);
//decode_results results;

#define Hibari 1
#define Fran 2
#define Urara 3
#define Teruomi 4
#define Tsubaki 5
#define Kotori 6

volatile int ope;

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
  attachInterrupt(4, ENC_READ, CHANGE);  //2
  attachInterrupt(5, ENC_READ, CHANGE);  //3
  for(int n=3 ; n<16 ;n++){
    pinMode(n,OUTPUT);
  }
  digitalWrite(SW1,LOW);
  mp3_set_serial (Serial);      //set Serial for DFPlayer-mini mp3 module 
  delay(10);                     // delay 1ms to set volume
  mp3_set_volume (30);          // value 0~30
  delay(10);
  mp3_play (1);
  delay(3000);
  ope = Hibari;
  LED_OFF();
  //irrecv.enableIRIn(); // Start the receiver
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
    mission_Time=millis();
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

  if(ope == Hibari){
    digitalWrite(G_LED,HIGH);
  }else if(ope == Fran){
    digitalWrite(B_LED,HIGH);
  }else if(ope == Urara){
    digitalWrite(R_LED,HIGH);
  }else if(ope == Teruomi){
    digitalWrite(B_LED,HIGH);
    digitalWrite(R_LED,HIGH);
  }else if(ope == Tsubaki){
    digitalWrite(B_LED,HIGH);
    digitalWrite(G_LED,HIGH);
  }else if(ope == Kotori){
    digitalWrite(G_LED,HIGH);
    digitalWrite(R_LED,HIGH);
  }

/*  if (irrecv.decode(&results)) {          // sonyリモコン解析
   Serial.println(results.value, HEX);
   dump(&results);
   irrecv.resume(); // Receive the next value
  }*/
  
  ERIC_UEDA();
  SET_TIME();
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
  if(ope == Hibari){
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
    if(Time==181) mp3_play (102);
  }else if(ope == Fran){
    if(( millis()-mission_Time)/1000 ==1) mp3_play(214); // ミッション開始時
    if(( millis()-mission_Time)/1000 ==300) mp3_play(203); // 開始5分経過
    if(( millis()-mission_Time)/1000 ==600) mp3_play(204); // 開始10分経過
    if(( millis()-mission_Time)/1000 ==900) mp3_play(205); // 開始15分経過
    if(( millis()-mission_Time)/1000 ==1200) mp3_play(206); // 開始20分経過
    if(( millis()-mission_Time)/1000 ==1500) mp3_play(207); // 開始25分経過
    if(( millis()-mission_Time)/1000 ==1800) mp3_play(208); // 開始30分経過
    if(( millis()-mission_Time)/1000 ==2100) mp3_play(209); // 開始35分経過
    if(( millis()-mission_Time)/1000 ==2400) mp3_play(210); // 開始40分経過
    if(( millis()-mission_Time)/1000 ==2700) mp3_play(211); // 開始45分経過
    if(( millis()-mission_Time)/1000 ==3000) mp3_play(212); // 開始50分経過
    if(Time==4) mp3_play (10);
    if(Time==31) mp3_play (200);
    if(Time==61) mp3_play (201);
    if(Time==181) mp3_play (202);
    if(Time==301) mp3_play (213); 
  }else if(ope == Urara){
    if(( millis()-mission_Time)/1000 ==1) mp3_play(314); // ミッション開始時
    if(( millis()-mission_Time)/1000 ==300) mp3_play(303); // 開始5分経過
    if(( millis()-mission_Time)/1000 ==600) mp3_play(304); // 開始10分経過
    if(( millis()-mission_Time)/1000 ==900) mp3_play(305); // 開始15分経過
    if(( millis()-mission_Time)/1000 ==1200) mp3_play(306); // 開始20分経過
    if(( millis()-mission_Time)/1000 ==1500) mp3_play(307); // 開始25分経過
    if(( millis()-mission_Time)/1000 ==1800) mp3_play(308); // 開始30分経過
    if(( millis()-mission_Time)/1000 ==2100) mp3_play(309); // 開始35分経過
    if(( millis()-mission_Time)/1000 ==2400) mp3_play(310); // 開始40分経過
    if(( millis()-mission_Time)/1000 ==2700) mp3_play(311); // 開始45分経過
    if(( millis()-mission_Time)/1000 ==3000) mp3_play(312); // 開始50分経過
    if(Time==4) mp3_play (10);
    if(Time==31) mp3_play (300);
    if(Time==61) mp3_play (301);
    if(Time==181) mp3_play (302);
    if(Time==301) mp3_play (313);
  }else if(ope == Teruomi){
    if(( millis()-mission_Time)/1000 ==1) mp3_play(414); // ミッション開始時
    if(( millis()-mission_Time)/1000 ==300) mp3_play(403); // 開始5分経過
    if(( millis()-mission_Time)/1000 ==600) mp3_play(404); // 開始10分経過
    if(( millis()-mission_Time)/1000 ==900) mp3_play(405); // 開始15分経過
    if(( millis()-mission_Time)/1000 ==1200) mp3_play(406); // 開始20分経過
    if(( millis()-mission_Time)/1000 ==1500) mp3_play(407); // 開始25分経過
    if(( millis()-mission_Time)/1000 ==1800) mp3_play(408); // 開始30分経過
    if(( millis()-mission_Time)/1000 ==2100) mp3_play(409); // 開始35分経過
    if(( millis()-mission_Time)/1000 ==2400) mp3_play(410); // 開始40分経過
    if(( millis()-mission_Time)/1000 ==2700) mp3_play(411); // 開始45分経過
    if(( millis()-mission_Time)/1000 ==3000) mp3_play(412); // 開始50分経過
    if(Time==4) mp3_play (10);
    if(Time==31) mp3_play (400);
    if(Time==61) mp3_play (401);
    if(Time==181) mp3_play (402);
    if(Time==301) mp3_play (413);
  }else if(ope == Tsubaki){
    if(( millis()-mission_Time)/1000 ==1) mp3_play(514); // ミッション開始時
    if(( millis()-mission_Time)/1000 ==300) mp3_play(503); // 開始5分経過
    if(( millis()-mission_Time)/1000 ==600) mp3_play(504); // 開始10分経過
    if(( millis()-mission_Time)/1000 ==900) mp3_play(505); // 開始15分経過
    if(( millis()-mission_Time)/1000 ==1200) mp3_play(506); // 開始20分経過
    if(( millis()-mission_Time)/1000 ==1500) mp3_play(507); // 開始25分経過
    if(( millis()-mission_Time)/1000 ==1800) mp3_play(508); // 開始30分経過
    if(( millis()-mission_Time)/1000 ==2100) mp3_play(509); // 開始35分経過
    if(( millis()-mission_Time)/1000 ==2400) mp3_play(510); // 開始40分経過
    if(( millis()-mission_Time)/1000 ==2700) mp3_play(511); // 開始45分経過
    if(( millis()-mission_Time)/1000 ==3000) mp3_play(512); // 開始50分経過
    if(Time==4) mp3_play (10);
    if(Time==31) mp3_play (500);
    if(Time==61) mp3_play (501);
    if(Time==181) mp3_play (502);
    if(Time==301) mp3_play (513);
  }else if(ope == Kotori){
    if(( millis()-mission_Time)/1000 ==1) mp3_play(614); // ミッション開始時
    if(( millis()-mission_Time)/1000 ==300) mp3_play(603); // 開始5分経過
    if(( millis()-mission_Time)/1000 ==600) mp3_play(604); // 開始10分経過
    if(( millis()-mission_Time)/1000 ==900) mp3_play(605); // 開始15分経過
    if(( millis()-mission_Time)/1000 ==1200) mp3_play(606); // 開始20分経過
    if(( millis()-mission_Time)/1000 ==1500) mp3_play(607); // 開始25分経過
    if(( millis()-mission_Time)/1000 ==1800) mp3_play(608); // 開始30分経過
    if(( millis()-mission_Time)/1000 ==2100) mp3_play(609); // 開始35分経過
    if(( millis()-mission_Time)/1000 ==2400) mp3_play(610); // 開始40分経過
    if(( millis()-mission_Time)/1000 ==2700) mp3_play(611); // 開始45分経過
    if(( millis()-mission_Time)/1000 ==3000) mp3_play(612); // 開始50分経過
    if(Time==4) mp3_play (10);
    if(Time==31) mp3_play (600);
    if(Time==61) mp3_play (601);
    if(Time==181) mp3_play (602);
    if(Time==301) mp3_play (613);
  }
}

/* 距離計測関数 */
void Get_Distance(){
  digitalWrite(trig_pin,HIGH); // トリガーを10μsだけHIGH出力
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);
  Duration=pulseIn(echo_pin,HIGH,5000);  // エコーで受信
  Distance=(Duration/2)*340*1000/1000000;  // 距離計算(音速340m/s)
  /*if(Distance>0){
    Serial.print("Distance:");
    Serial.print(Distance);
    Serial.println("mm");
  }*/
}

/* エリック上田と警告する関数 */
void ERIC_UEDA(){
  Get_Distance();
  if(Distance<100 && Distance!=0){
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
    LED_OFF();
  }
  else{
    eric_ueda=0;
  }   
}

/* NECの信号で時間をリセットする関数 */
void SET_TIME() {
      int ans = IRrecive();
      if (ans != 0) Serial.println(ans);
      //Serial.println(ans);
      if (ans != 0) {
       switch(ans) {
           case 22:               // IRリモコンの0ボタン
           /*  for (int i = 0; i < 10; i++) {
              irsend.sendSony(0xa90, 12); // Sonyフォーマットでテレビ電源ONの信号
              delay(40);
              Serial.println("SONY OK");
             }*/
           break;
           
           case 12:               // IRリモコンの1ボタン

           break;
           
           case 24:               // IRリモコンの2ボタン

           break;  
           
           case 9:               // IRリモコンのEQボタン

           break;
           
           case 21:               // IRリモコンの+ボタン
              enc_count=enc_count+60;
           break;
           
           case 7:               // IRリモコンの-ボタン
              enc_count=enc_count-60;
           break;
           
           case 67:               // IRリモコンの再生ボタン
              //SW_ON_OFF = !SW_ON_OFF;
              //Time=enc_count;
           break;
           
           case 71:               // IRリモコンのCH+ボタン
              ope++;
              if(ope>6) ope=1;
              if(ope<1) ope=6;
              if(ope == Hibari){
                mp3_play(114);
              }else if(ope == Fran){
                mp3_play(215);
              }else if(ope == Urara){
                mp3_play(315);
              }else if(ope == Teruomi){
                mp3_play(415);
              }else if(ope == Tsubaki){
                mp3_play(515);
              }else if(ope == Kotori){
                mp3_play(615);
              }
              
           break;
           
           case 69:               // IRリモコンのCH-ボタン
              ope--;
              if(ope>6) ope=1;
              if(ope<1) ope=6;
              if(ope == Hibari){
                mp3_play(114);
              }else if(ope == Fran){
                mp3_play(215);
              }else if(ope == Urara){
                mp3_play(315);
              }else if(ope == Teruomi){
                mp3_play(415);
              }else if(ope == Tsubaki){
                mp3_play(515);
              }else if(ope == Kotori){
                mp3_play(615);
              }
           break;                      
       }
  }
}


// 赤外線リモコンのデータを受信する処理関数
int IRrecive()
{
     unsigned long t ;
     int i , j ;
     int cnt , ans ;
     char IRbit[64] ;
     #define IR_PIN     28
     #define DATA_POINT 3           // 受信したデータから読取る内容のデータ位置

     ans = 0 ;
     t = 0 ;
     if (digitalRead(IR_PIN) == LOW) {
          // リーダ部のチェックを行う
          t = micros() ;                        // 現在の時刻(us)を得る
          while (digitalRead(IR_PIN) == LOW) ;  // HIGH(ON)になるまで待つ
          t = micros() - t ;                    // LOW(OFF)の部分をはかる
     }
     // リーダ部有りなら処理する(3.4ms以上のLOWにて判断する)
     if (t >= 3400) {
          i = 0 ;
          while(digitalRead(IR_PIN) == HIGH) ;  // ここまでがリーダ部(ON部分)読み飛ばす
          // データ部の読み込み
          while (1) {
               while(digitalRead(IR_PIN) == LOW) ;// OFF部分は読み飛ばす
               t = micros() ;
               cnt = 0 ;
               while(digitalRead(IR_PIN) == HIGH) {// LOW(OFF)になるまで待つ
                    delayMicroseconds(10) ;
                    cnt++ ;
                    if (cnt >= 1200) break ;    // 12ms以上HIGHのままなら中断
               }
               t = micros() - t ;
               if (t >= 10000) break ;      // ストップデータ
               if (t >= 1000)  IRbit[i] = (char)0x31 ;  // ON部分が長い
               else            IRbit[i] = (char)0x30 ;  // ON部分が短い
               i++ ;
          }
          // データ有りなら指定位置のデータを取り出す
          if (i != 0) {
               i = (DATA_POINT-1) * 8 ;
               for (j=0 ; j < 8 ; j++) {
                    if (IRbit[i+j] == 0x31) bitSet(ans,j) ;
               }
          }
     }
     return( ans ) ;
}


/* SONYフォーマットコード解析 */
void dump(decode_results *results) {
int count = results->rawlen;
if (results->decode_type == UNKNOWN) {
Serial.print("Unknown encoding: ");
}
else if (results->decode_type == NEC) {
Serial.print("Decoded NEC: ");
}
else if (results->decode_type == SONY) {
Serial.print("Decoded SONY: ");
}
else if (results->decode_type == RC5) {
Serial.print("Decoded RC5: ");
}
else if (results->decode_type == RC6) {
Serial.print("Decoded RC6: ");
}
/*else if (results->decode_type == PANASONIC) { 
Serial.print("Decoded PANASONIC - Address: ");
Serial.print(results->panasonicAddress,HEX);
Serial.print(" Value: ");
}
/*else if (results->decode_type == LG) {
Serial.print("Decoded LG: ");
}
else if (results->decode_type == JVC) {
Serial.print("Decoded JVC: ");
}
else if (results->decode_type == AIWA_RC_T501) {
Serial.print("Decoded AIWA RC T501: ");
}
else if (results->decode_type == WHYNTER) {
Serial.print("Decoded Whynter: ");
}*/
Serial.print(results->value, HEX);
Serial.print(" (");
Serial.print(results->bits, DEC);
Serial.println(" bits)");
Serial.print("Raw (");
Serial.print(count, DEC);
Serial.print("): ");
 
for (int i = 0; i < count; i++) {
 if ((i % 2) == 1) {
  Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
 }
 else {
  Serial.print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
 }
 Serial.print(" ");
}
Serial.println("");
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

/* 全LEDをON (7セグLEDのカソードはArduinoの11～13番に接続) */
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




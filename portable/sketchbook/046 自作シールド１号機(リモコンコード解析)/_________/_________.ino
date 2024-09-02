#include <LiquidCrystal.h>  /* LCD用ライブラリ */ 
#include <IRremote.h>
 
int RECV_PIN = 9;
long int TRIG_VAL_0 = 0xFF6897; // IRリモコン0
long int TRIG_VAL_1 = 0xA90; // SONYリモコン電源
//IRsend irsend;
IRrecv irrecv(RECV_PIN);

decode_results results;

int LED = 6; // 確認用LED
int RS = 8;  // LCD用
int E  = 7;
int DB4= 3;
int DB5= 2;
int DB6= 4;
int DB7= 5;
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);      // LCDのピン設定

void setup()
{
 Serial.begin(9600);
 pinMode(LED,OUTPUT);
 irrecv.enableIRIn(); // Start the receiver
   for(int n=0 ; n<9 ;n++){
     pinMode(n,OUTPUT);
   }
 LCD_init();
}


void loop() {
 if (irrecv.decode(&results)) {
   Serial.println(results.value, HEX);
   dump(&results);
   irrecv.resume(); // Receive the next value
 }
/*  int ans = IRrecive();
  lcd.setCursor(0, 1); 
  lcd.print(">> ");
   if (ans != 0){
    lcd.setCursor(3, 1);
    lcd.print(ans);
    Serial.println(ans);
   }*/
}




// ---------以降関数群---------------------------

/* LCD初期化関数(ロゴ表示) */
void LCD_init(){
  lcd.begin(16, 2);                          /* LCDの設定(16文字2行) */
  lcd.clear();                               /* LCDのクリア */
  lcd.setCursor(0, 0);                       /* 0列0行から表示する */
  lcd.print("CODE_ANALYSIS");          /* 文字列の表示 */
  lcd.setCursor(9, 1);                      /* 10列1行から表示する */
  lcd.print("ver 1.0");                      /* 文字列の表示 */
  delay(3000);
  lcd.clear();
}


/* SONY信号解析 */
// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
// decode_results *results = (decode_results *)v
void dump(decode_results *results) {
int count = results->rawlen;

if (results->decode_type == UNKNOWN) {
  lcd.clear();
  digitalWrite(LED,LOW);
  Serial.print("Unknown encoding: ");
  lcd.setCursor(0, 0);              /* 0列0行から表示する */
  lcd.print("Unknown encoding:");
}
else if (results->decode_type == NEC) {
  lcd.clear();
  digitalWrite(LED,LOW);
  Serial.print("Decoded NEC: ");
  lcd.setCursor(0, 0);              /* 0列0行から表示する */
  lcd.print("Decoded NEC:");
  if(results->value == TRIG_VAL_0) {
//    digitalWrite(LED,LOW);
//    delay(3000);
//    digitalWrite(LED,HIGH);
  }
}
else if (results->decode_type == SONY) {
  lcd.clear();
  Serial.print("Decoded SONY: ");
  digitalWrite(LED,LOW);
  lcd.setCursor(0, 0);              /* 0列0行から表示する */
  lcd.print("Decoded SONY:");
  if(results->value == TRIG_VAL_1) {
    digitalWrite(LED,HIGH);
  }  
}
else if (results->decode_type == RC5) {
  lcd.clear();
Serial.print("Decoded RC5: ");
}
else if (results->decode_type == RC6) {
  lcd.clear();
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
lcd.setCursor(0, 1);                      /* 0列1行から表示する */
lcd.print(">> ");
lcd.setCursor(3, 1);                      /* 0列1行から表示する */
lcd.print(results->value,HEX);

Serial.print(results->value,HEX);
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

}


// 赤外線リモコンのデータを受信する処理関数
int IRrecive()
{
     unsigned long t ;
     int i , j ;
     int cnt , ans ;
     char IRbit[64] ;
     #define IR_PIN     RECV_PIN
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


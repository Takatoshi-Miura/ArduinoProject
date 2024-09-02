/****************************************************************************/
/* < ロータリーエンコーダーによる回転方向判定>   Arduino unoを使用          */
/* 秋月電子のロータリーエンコーダー(RE-160F-40E3-(L)A-24P)を使用            */
/* UP,DOWNの判定結果をLCDに表示する。                                       */
/*                                                                          */
/****************************************************************************/

/* ライブラリ */
#include <LiquidCrystal.h>  // LCD用ライブラリ


/* 変数の宣言 */
unsigned long pre_Time=0;
unsigned long now_Time=0;
unsigned long millis();


/* ピン設定 */
int RS = 6;                    // LCD用 
int E  = 7;
int DB4= 8;
int DB5= 9;
int DB6= 4;
int DB7= 5;
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);    // LCDのピン設定 
#define ENC_A 2
#define ENC_B 3
volatile byte pos;
volatile int  enc_count;



/*　初期設定  */
void setup(){
  Serial.begin( 9600 );
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP); 
  attachInterrupt(0, ENC_READ, CHANGE);
  attachInterrupt(1, ENC_READ, CHANGE);
  for(int n=2 ; n<8 ;n++){
    pinMode(n,OUTPUT);
  }
  pre_Time=millis();
  lcd.begin(16, 2);
  lcd.clear();
}


/* メインループ */
void loop(){
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
 
    bool rote = 0;
    if (cur == 3 && old == 0) rote = 0;
    else if (cur == 0 && old == 3) rote = 1;
    else if (cur > old) rote = 1;

    lcd.clear(); 
    lcd.setCursor(0, 0);                       // 0列0行から表示する
    if(rote==0){
      lcd.print("DOWN");
    }
    else{
      lcd.print("UP");
    }
    lcd.setCursor(0, 1);
    lcd.print(enc_count);
    pos = (dir << 4) + (old << 2) + cur;
 
    const char vector[2] = {'<', '>'};
    Serial.print(vector[rote]);
    Serial.print(" ");
    Serial.print(enc_count);
    Serial.print(" ");
    Serial.println(pos + (1 << 7), BIN);
    if (cur == 0) Serial.println();
  }
}


/* ------------------------------ 以降 関数群 ----------------------------------*/

/* LCD初期化関数(ロゴ表示) */
void LCD_init(){
  lcd.begin(16, 2);                          // LCDの設定(20文字4行)
  lcd.clear();                               // LCDのクリア 
  lcd.setCursor(0, 0);                       // 0列0行から表示する
  lcd.print("Speed_Time_Distance");          // 文字列の表示 
  lcd.setCursor(12, 1);
  lcd.print("ver 7.4");
  lcd.setCursor(5, 3);
  lcd.print("CHODAI CO.,LTD.");
  delay(2000);
  lcd.clear();
}



/****************************************************************************/
/* GE switch                                    　　                        */
/*                                                                          */
/* Arduino megaを使用                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* ライブラリ */
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

/* ピン設定 */
SoftwareSerial mySoftwareSerial(15, 14); // RX3, TX3
#define SWITCH_A 2  // 割り込み0
#define SWITCH_B 3  // 割り込み1
#define SWITCH_C 21 // 割り込み2
#define SWITCH_D 20 // 割り込み3
#define SWITCH_E 19 // 割り込み4
//#define BURST_PIN 18 // 割り込み5
#define LED_A 9  //　な
#define LED_B 10 //　に
#define LED_C 11 //　ぬ
#define LED_D 12 //　ね
#define LED_E 13 //　の
#define PUSH_TIME 100000 // ボタンを押した時間(長押し判定用)

/* 変数の宣言 */
int BUTTON_NO=0; // 押したボタン番号を格納する変数

/* 初期設定 */
void setup() {
  mySoftwareSerial.begin(9600);
  pinMode(SWITCH_A, INPUT_PULLUP);
  pinMode(SWITCH_B, INPUT_PULLUP);
  pinMode(SWITCH_C, INPUT_PULLUP);
  pinMode(SWITCH_D, INPUT_PULLUP);
  pinMode(SWITCH_E, INPUT_PULLUP);
//  pinMode(BURST_PIN, INPUT);
  attachInterrupt(0, SWITCH_READ_A, FALLING);  // pin2
  attachInterrupt(1, SWITCH_READ_B, FALLING);  // pin3
  attachInterrupt(2, SWITCH_READ_C, FALLING);  // pin21
  attachInterrupt(3, SWITCH_READ_D, FALLING);  // pin20
  attachInterrupt(4, SWITCH_READ_E, FALLING);  // pin19
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_E, OUTPUT);
  mp3_set_serial (mySoftwareSerial);   //set Serial for DFPlayer-mini mp3 module 
  delay(10);                  // delay 1ms to set volume
  mp3_set_volume (10);        // value 0~30
  delay(10);
}

void loop() {

  unsigned long gauge_A=0;
  unsigned long gauge_B=0;
  unsigned long gauge_C=0;
  unsigned long gauge_D=0;
  unsigned long gauge_E=0;

  switch(BUTTON_NO){
    case 1:
        ALL_LED_OFF();
        digitalWrite(LED_A, HIGH);
        mp3_play (1);
        BUTTON_NO=0;
        break;
    case 2:
        ALL_LED_OFF();
        digitalWrite(LED_B, HIGH);
        mp3_play (2);
        BUTTON_NO=0;
        break;
    case 3:
        ALL_LED_OFF();
        digitalWrite(LED_C, HIGH);
        mp3_play (3);
        BUTTON_NO=0;
        break;
    case 4:
        ALL_LED_OFF();
        digitalWrite(LED_D, HIGH);
        mp3_play (4);
        BUTTON_NO=0;
        break;
    case 5:
        ALL_LED_OFF();
        digitalWrite(LED_E, HIGH);
        mp3_play (5);
        BUTTON_NO=0;
        break;
    default:
        BUTTON_NO=0;
  }

  while(!digitalRead(SWITCH_A)){
    gauge_A++;
    if(gauge_A>PUSH_TIME){
      BURST_MODE();
    }
  }

}


/* 割り込み関数 */
void SWITCH_READ_A(){
  BUTTON_NO=1;
}

void SWITCH_READ_B(){
  BUTTON_NO=2;
}

void SWITCH_READ_C(){
  BUTTON_NO=3;
}

void SWITCH_READ_D(){
  BUTTON_NO=4;
}

void SWITCH_READ_E(){
  BUTTON_NO=5;
}


/******************** 関数群 ********************/
void ALL_LED_OFF(){
  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_D, LOW);
  digitalWrite(LED_E, LOW);
}

void ALL_LED_ON(){
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_C, HIGH);
  digitalWrite(LED_D, HIGH);
  digitalWrite(LED_E, HIGH);
}

void LED_FLOW(){
  for(int i=9;i<14;i++){
    ALL_LED_OFF();
    digitalWrite(i, HIGH);
    delay(150);
  }
}

void ALL_LED_OFF_PWM(){
  ALL_LED_OFF();
  for(int n=255; n>-1; n--){
    analogWrite(LED_A, n);
    analogWrite(LED_B, n);
    analogWrite(LED_C, n);
    analogWrite(LED_D, n);
    analogWrite(LED_E, n);
    delay(10);
  }
}

void LED_3ON(){
  ALL_LED_OFF();
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_C, HIGH);
  digitalWrite(LED_E, HIGH);
}

void LED_2ON(){
  ALL_LED_OFF();
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_D, HIGH);
}

void LED_ON_OFF(){
  ALL_LED_ON();
  delay(380);
  ALL_LED_OFF();
  delay(380);
}

void LED_BURST(){
  ALL_LED_OFF();
  
  LED_3ON();
  delay(800);
  LED_2ON();
  delay(800);
  LED_3ON();
  delay(800);
  LED_2ON();
  delay(800);
  
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);

  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();

  ALL_LED_ON();
  ALL_LED_OFF_PWM();
  delay(200);

  LED_3ON();
  delay(800);
  LED_2ON();
  delay(800);
  LED_3ON();
  delay(800);
  LED_2ON();
  delay(800);
  
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);
  ALL_LED_ON();
  delay(350);
  ALL_LED_OFF();
  delay(350);

  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();
  LED_FLOW();

  ALL_LED_ON();
  ALL_LED_OFF_PWM();
  delay(350);

  for(int s=0;s<13;s++){
    LED_ON_OFF();
  }

  for(int m=0;m<22;m++){
    LED_FLOW();
  }
  
  for(int s=0;s<14;s++){
    LED_ON_OFF();
  }
}


void BURST_MODE(){
  ALL_LED_OFF();
  mp3_play (100);
  delay(4500);
  for(int k=0;k<500;k++){
    LED_BURST();
  }
}









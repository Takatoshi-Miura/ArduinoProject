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
#define LED_A 9  //　な
#define LED_B 10 //　に
#define LED_C 11 //　ぬ
#define LED_D 12 //　ね
#define LED_E 13 //　の

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
  mp3_set_volume (18);        // value 0~30
  delay(10);
  mp3_play (100);
  delay(4500);
  LED_BURST();
  LED_BURST();
  LED_BURST();
  LED_BURST();
  LED_BURST();
}

void loop() {

  switch(BUTTON_NO){
    case 1:
        digitalWrite(LED_A, HIGH);
        delay(1700);
        BUTTON_NO=0;
        break;
    case 2:
        digitalWrite(LED_B, HIGH);
        delay(2000);
        BUTTON_NO=0;
        break;
    case 3:
        digitalWrite(LED_C, HIGH);
        delay(1200);
        BUTTON_NO=0;
        break;
    case 4:
        digitalWrite(LED_D, HIGH);
        delay(6500);
        BUTTON_NO=0;
        break;
    case 5:
        digitalWrite(LED_E, HIGH);
        delay(4400);
        BUTTON_NO=0;
        break;
    default:
        BUTTON_NO=0;
        ALL_LED_OFF();
  }

  

}


/* 割り込み関数 */
void SWITCH_READ_A(){
  ALL_LED_OFF();
  mp3_play (1);
  BUTTON_NO=1;
  digitalWrite(LED_A, HIGH);
}

void SWITCH_READ_B(){
  ALL_LED_OFF();
  mp3_play (2);
  BUTTON_NO=2;
  digitalWrite(LED_B, HIGH);
}

void SWITCH_READ_C(){
  ALL_LED_OFF();
  mp3_play (3);
  BUTTON_NO=3;
  digitalWrite(LED_C, HIGH);
}

void SWITCH_READ_D(){
  ALL_LED_OFF();
  mp3_play (4);
  BUTTON_NO=4;
  digitalWrite(LED_D, HIGH);
}

void SWITCH_READ_E(){
  ALL_LED_OFF();
  mp3_play (5);
  BUTTON_NO=5;
  digitalWrite(LED_E, HIGH);
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









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
#define SWITCH_A 2  // 割り込み0
#define SWITCH_B 3  // 割り込み1
#define SWITCH_C 21 // 割り込み2
#define SWITCH_D 20 // 割り込み3
#define SWITCH_E 19 // 割り込み4


/* 初期設定 */
void setup() {
    Serial.begin (9600);
    pinMode(SWITCH_A, INPUT_PULLUP);
    pinMode(SWITCH_B, INPUT_PULLUP);
    pinMode(SWITCH_C, INPUT_PULLUP);
    pinMode(SWITCH_D, INPUT_PULLUP);
    pinMode(SWITCH_E, INPUT_PULLUP);
    attachInterrupt(0, SWITCH_READ_A, CHANGE);  // pin2
    attachInterrupt(1, SWITCH_READ_B, CHANGE);  // pin3
    attachInterrupt(2, SWITCH_READ_C, CHANGE);  // pin21
    attachInterrupt(3, SWITCH_READ_D, CHANGE);  // pin20
    attachInterrupt(4, SWITCH_READ_E, CHANGE);  // pin19
    mp3_set_serial (Serial);   //set Serial for DFPlayer-mini mp3 module 
    delay(10);                  // delay 1ms to set volume
    mp3_set_volume (18);        // value 0~30
    delay(10);
}

void loop() {

}


/* 割り込み関数 */
void SWITCH_READ_A(){
  mp3_play (1);
}

void SWITCH_READ_B(){
  mp3_play (2);
}

void SWITCH_READ_C(){
  mp3_play (3);
}

void SWITCH_READ_D(){
  mp3_play (4);
}

void SWITCH_READ_E(){
  mp3_play (5);
}


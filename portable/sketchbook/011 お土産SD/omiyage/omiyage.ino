/****************************************************************************/
/* < Distance & Time ver3.0>   Arduino megaを使用                           */
/* 障害物までの距離をmm単位で計測。距離はLCDで確認可能。                    */
/* 障害物までの距離が1ｍを切ると警告用LEDが点滅する。                       */
/* プログラムを起動してからの時間をLCDに表示。(NEC製リモコンでリセット可)　 */
/* 障害物までの距離と時間をmicroSDに記録する。                              */
/*                                                                          */
/*【距離センサーとの接続】　　　　　　　　　　　　　　　　　                */
/* デジタルピン(SDA)…Blue, デジタルピン(SCL)…Green                        */ 
/*                                                                          */
/*【LCDとの接続】                                                           */
/* デジタルピン2…DB4,デジタルピン3…DB5,デジタルピン4…DB6,                */
/* デジタルピン5…DB7,デジタルピン6…Rs, デジタルピン7…E                   */
/*                                                                          */
/*【SDカードスロットとの接続(変更不可)】                                    */
/* デジタルピン51…MOSI, デジタルピン50…MISO                               */
/* デジタルピン52…SCK,  デジタルピン53…CS                                 */
/*                                                                          */
/*【赤外線受信モジュールとの接続】                                          */
/* デジタルピン12…Vout                                                     */
/*                                                                          */
/*【警告用LED】                                                             */
/* デジタルピン13…アノード                                                 */
/*                                                                          */
/****************************************************************************/

/* ライブラリ */
#include <LiquidCrystal.h>  /* LCDライブラリ */
#include <boarddefs.h>        /* リモコン用ライブラリ */
#include <IRremote.h>
#include <LIDARLite.h>      /* 距離センサー用ライブラリ */
#include <SPI.h>             /* SDカードスロット用ライブラリ */
#include <SD.h>


/* 変数の宣言 */
unsigned long pre_Time=0;
unsigned long now_Time=0;
unsigned long millis();
int Dis=0;
double Dis1=0;
int hour=0;
int minute=0;
int second=0;
const int chipSelect = 53;    /* CSのピン番号を指定 */


/* ピン設定 */
int RS = 6;      /* LCD用 */
int E  = 7;
int DB4= 2;
int DB5= 3;
int DB6= 4;
int DB7= 5;  
int Rem= 12;     /* 赤外線受信モジュール用 */
int LED= 13;     /* 警告LED用 */
LIDARLite myLidarLite;


/*　初期設定  */
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7); 
IRrecv irrecv(Rem); // 赤外線受信モジュール
decode_results results;
void setup(){
  Serial.begin( 9600 );
  pinMode(LED,OUTPUT);
  lcd.begin(16, 2);                          /* LCDの設定(16文字2行) */
  lcd.clear();                               /* LCDのクリア */
  lcd.setCursor(0, 0);                       /* 0列0行から表示する */
  lcd.print("Start!!");              /* 文字列の表示 */
  delay(3000);
  lcd.clear();    

  Serial.print(F("Initializing SD card..."));     /* SDライブラリの初期化 */
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    lcd.setCursor(0, 0);                      
    lcd.print("Card failed,");
    lcd.setCursor(0, 1);
    lcd.print("or not present");              
    while(1);
  }
  Serial.println(F("ok."));
  SdFile::dateTimeCallback( &dateTime );     /* ファイルの作成日時を記録 */
}

unsigned long count=0;

/* メインループ */
void loop(){  
  lcd.setCursor(0, 0);          
  lcd.print("count: ");
  lcd.setCursor(7, 0);          
  lcd.print(count);  
  String dataString="変態氏がお土産を買ってこないなんて、わけがわからないよ。";
  File dataFile = SD.open("omiyage.txt", FILE_WRITE);
    if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    count++;
    }    
}




/* ------------------------------ 以降 関数群 ----------------------------------*/
/* LCDに距離と時間を表示する関数 */
void LCD(long n){                   /* 引数として時間のデータを与える */
  lcd.setCursor(0, 0);              /* 0列0行から表示する */
  lcd.print("Dis :     ");
  Dis = myLidarLite.distance();     /* 距離データ取得 */
  Dis1 = (double)Dis/100;           /* 距離データ[mm]→[m]変換 */
  lcd.setCursor(10, 0);
  lcd.print(Dis1);                  /* 距離の表示 */
  lcd.setCursor(14, 0);
  lcd.print(" m");
  hour = n/3600000;                 /* 時間の計算 */
  minute = n/60000;                 /* 分の計算 */
  second = (n%60000)/1000;          /* 秒の計算 */
  lcd.setCursor(0, 1);              /* 0列1行から表示する */
  lcd.print("Time: ");            
  lcd.setCursor(6, 1);              /* 6列1行から表示する 時間の表示 */
  lcd.print(hour);  
  lcd.setCursor(7, 1);              /* 7列1行から表示する 単位(時間)の表示 */  
  lcd.print("h");
  lcd.setCursor(9, 1);              /* 9列1行から表示する 分の表示 */  
  lcd.print(minute);  
  lcd.setCursor(11, 1);             /* 11列1行から表示する 単位(分)の表示 */
  lcd.print("m   ");
  lcd.setCursor(13, 1);             /* 13列1行から表示する 秒の表示 */  
  lcd.print(second);  
  lcd.setCursor(15, 1);             /* 15列1行から表示する 単位(秒)の表示 */
  lcd.print("s");  
}


/* LEDを点滅させる関数 */
void LED_FLASH(){
  digitalWrite(LED,HIGH);
  delay(70);
  digitalWrite(LED,LOW);
  delay(50);
}


/* SDカードに"変態氏がお土産を買ってこないなんてわけがわからないよ"と1000回書き込む関数 */
void SD_SAVE(){
  String dataString="";
  File dataFile = SD.open("omiyage.txt", FILE_WRITE);
  if (dataFile) {
    for(int m=0;m<=1000;m++){
      dataString += "変態氏がお土産を買ってこないなんて、わけがわからないよ。 ";
    }
    dataFile.println(dataString);
    Serial.println(dataString);
    dataFile.close();
  }
  else {
    Serial.println(F("error opening data.txt"));
  } 
}



/* 日付と時刻を取得する関数(要 GPS or RTC) */
void dateTime(uint16_t* date, uint16_t* time)
{
  uint16_t year = 2016;
  uint8_t month = 2, day = 20, hour1 = 9, minute1 = 0, second1 = 0;
  *date = FAT_DATE(year, month, day);        /* FAT_DATEマクロでフィールドを埋めて日付を返す */
  *time = FAT_TIME(hour1, minute1, second1); /* FAT_TIMEマクロでフィールドを埋めて時間を返す */
}


/* NECの信号で時間をリセットする関数 */
void RESET_TIME(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == NEC) {
      Serial.print("Decoded NEC: ");
      lcd.clear();
      pre_Time=millis();
      now_Time=millis();
      now_Time=now_Time - pre_Time;
      LCD(now_Time);
      lcd.clear();       
      lcd.setCursor(0, 0);       /* 0列0行から表示する */
      lcd.print("Reset Time ");  /* 文字列の表示 */
      delay(1000);
   }
}


/* シリアルモニターでリモコンのコードを解析する関数 */
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


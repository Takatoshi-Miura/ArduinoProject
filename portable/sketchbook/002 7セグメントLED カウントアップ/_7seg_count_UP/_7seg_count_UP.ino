void setup(){
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
}

//7セグLEDのレイアウトを定義
boolean Num_Array[10][7]={
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

//LED表示関数 (Numberに表示したい数字を入れる)
void NumPrint(int Number){
  for(int w=0; w<=7 ; w++){
    digitalWrite(w+2,-Num_Array[Number][w]);
  }
}

//全LEDをOFF (7セグLEDのピンはArduinoの2～8番に接続)
void off7SegLED(){
  for(int i=2;i<9;i++){
    digitalWrite(i,LOW);
  }
}


void loop(){
  //0～9までカウントアップ
  for(int i=0;i<10;i++){
    NumPrint(i);
    delay(1000);
    off7SegLED();
    delay(50);
  }
}



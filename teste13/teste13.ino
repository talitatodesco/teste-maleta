//Biblioteca
#include <NTPClient.h>  // Biblioteca do NTP.
#include <WiFi.h>       // Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>

#define FIREBASE_HOST "https://malit-3b137-default-rtdb.firebaseio.com"   
#define FIREBASE_AUTH "qHaoj4vl7AqZ1qlBr0ruwjAMvUOmX3DQcFOOxp1X"  
#define WIFI_SSID "CLARO_2G544AC8"               
#define WIFI_PASSWORD "96544AC8"

//Conexão com wifi
const char* ssid = "CLARO_2G544AC8";
const char* password = "96544AC8";

// --------------Configuração de relógio online -------------------
const char* ntpServer1 = "c.st1.ntp.br"; //Endereço 1
const char* ntpServer2 = "pool.ntp.org";//Endereço 2
const long gmtOffset_sec = 0; 
const int daylightOffset_sec = -3600*3;

int hrs = 0;
int mins = 0;
int Ch[8];
int Cm[8];  
int hora[8];
int minuto[8];
int horamedi[8];
int minmedi[8];

int funcaoalarme;
int led_statusC1;
int led_statusC2;

int led_statusC3;
// int led_statusC4;

// int led_statusC5;
// int led_statusC6;

// int led_statusC7;
// int led_statusC8;

int led_statusC9;
// unsigned long tempoAtual     = 0;
// unsigned long tempoAnterior1 = 0;
// unsigned long tempoAnterior2 = 0;


struct tm timeinfo;

int led1 = 15;
int led2 = 2;                      // definição dos pinos de saidas
int led3 = 4;
int led4 = 16;
int led5 = 5;                      // definição dos pinos de saidas
int led6 = 18;
int led7 = 19;
int led8 = 21;
int led9 = 32;      
// int buzzer = 27;


void printLocalTime() {

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Não foi possivel obter o tempo");
    return;
  } //Caso não consiga buscar o horario
  Serial.println(&timeinfo, "%H:%M");  //Saida horario monitor serial

  delay(1000);
}


void setup() {
  Serial.begin(115200);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2); //Configuração Time

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);
  // pinMode(buzzer, OUTPUT);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  Serial.println(" WIFI CONECTADO!");
  delay(1000);

// -------- Conexão Firebase --------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 
}


// Função declaração alarme
void AlarmTools(){

  Ch[0] = Firebase.getInt("maleta/C1/hora");
  Cm[0] = Firebase.getInt("maleta/C1/min");
  hora[0] = Firebase.getInt("maleta/C1/adhoramed");
  minuto[0] = Firebase.getInt("maleta/C1/adminmed");
  horamedi[0] = Ch[0] + hora[0];
  minmedi[0] = Cm[0] + minuto[0];

  
  Ch[1] = Firebase.getInt("maleta/C2/hora");
  Cm[1] = Firebase.getInt("maleta/C2/min");
  hora[1] = Firebase.getInt("maleta/C2/adhoramed");
  minuto[1] = Firebase.getInt("maleta/C2/adminmed");
  horamedi[1] = Ch[1] + hora[1];
  minmedi[1] = Cm[1] + minuto[1];


  Ch[2] = Firebase.getInt("maleta/C3/hora");
  Cm[2] = Firebase.getInt("maleta/C3/min");
  hora[2] = Firebase.getInt("maleta/C3/adhoramed");
  minuto[2] = Firebase.getInt("maleta/C3/adminmed");
  horamedi[2] = Ch[2] + hora[2];
  minmedi[2] = Cm[2] + minuto[2];
  
  

 

  // Ch[3] = Firebase.getInt("maleta/C4/hora");
  // Cm[3] = Firebase.getInt("maleta/C4/min");
  // Ch[4] = Firebase.getInt("maleta/C5/hora");
  // Cm[4] = Firebase.getInt("maleta/C5/min");
  // Ch[5] = Firebase.getInt("maleta/C6/hora");
  // Cm[5] = Firebase.getInt("maleta/C6/min");
  // Ch[6] = Firebase.getInt("maleta/C7/hora");
  // Cm[6] = Firebase.getInt("maleta/C7/min");
  // Ch[7] = Firebase.getInt("maleta/C8/hora");
  // Cm[7] = Firebase.getInt("maleta/C8/min");
  // Ch[8] = Firebase.getInt("maleta/C9/hora");
  // Cm[8] = Firebase.getInt("maleta/C9/min");


 
 

  

  // hora[3] = Firebase.getInt("maleta/C4/adhoramed");
  // minuto[3] = Firebase.getInt("maleta/C4/adminmed");

  // hora[4] = Firebase.getInt("maleta/C5/adhoramed");
  // minuto[4] = Firebase.getInt("maleta/C5/adminmed");

  // hora[5] = Firebase.getInt("maleta/C6/adhoramed");
  // minuto[5] = Firebase.getInt("maleta/C6/adminmed");

  // hora[6] = Firebase.getInt("maleta/C7/adhoramed");
  // minuto[6] = Firebase.getInt("maleta/C7/adminmed");

  // hora[7] = Firebase.getInt("maleta/C8/adhoramed");
  // minuto[7] = Firebase.getInt("maleta/C8/adminmed");

  // hora[8] = Firebase.getInt("maleta/C9/adhoramed");
  // minuto[8] = Firebase.getInt("maleta/C9/adminmed");

  
  funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
 

  hrs = timeinfo.tm_hour; 
  mins = timeinfo.tm_min;

 

  // horamedi[3] = Ch[3] + hora[3];
  // minmedi[3] = Cm[3] + minuto[3];
  
  // horamedi[4] = Ch[4] + hora[4];
  // minmedi[4] = Cm[4] + minuto[4];

  // horamedi[5] = Ch[5] + hora[5];
  // minmedi[5] = Cm[5] + minuto[5];
  
  // horamedi[6] = Ch[6] + hora[6];
  // minmedi[6] = Cm[6] + minuto[6];

  // horamedi[7] = Ch[7] + hora[7];
  // minmedi[7] = Cm[7] + minuto[7];


}

void loop()
{
  AlarmTools();

  led_statusC1 = Firebase.getBool("maleta/C1/led_status");
  led_statusC2 = Firebase.getBool("maleta/C2/led_status");
  led_statusC3 = Firebase.getBool("maleta/C3/led_status");
  // led_statusC4 = Firebase.getBool("maleta/C4/led_status");
  // led_statusC5 = Firebase.getBool("maleta/C5/led_status");
  // led_statusC6 = Firebase.getBool("maleta/C6/led_status");
  // led_statusC7 = Firebase.getBool("maleta/C7/led_status");
  // led_statusC8 = Firebase.getBool("maleta/C8/led_status");
  // led_statusC9 = Firebase.getBool("maleta/C9/led_status");


  while(funcaoalarme == 1){
    AlarmTools();
    
    if(hrs == horamedi[0] &&  mins == minmedi[0] ){
      digitalWrite(led1, HIGH);
    }else{
    digitalWrite(led1,LOW);
    }
    if(hrs == horamedi[1] &&  mins == minmedi[1]){
      digitalWrite(led2, HIGH);
    }else{
      digitalWrite(led2,LOW);
    }
    if(hrs == horamedi[2] &&  mins == minmedi[2]){
      digitalWrite(led3, HIGH);
    }else{
      digitalWrite(led3,LOW);
    }
  // if(hrs == horamedi[3] &&  mins == minmedi[3]){
  //   digitalWrite(led4, HIGH);
  // }else{
  //   digitalWrite(led4,LOW);
  // }
  // if(hrs == horamedi[4] &&  mins == minmedi[4]){
  //   digitalWrite(led5, HIGH);
  // }else{
  //   digitalWrite(led5,LOW);
  // }
  // if(hrs == horamedi[5] &&  mins == minmedi[5]){
  //   digitalWrite(led6, HIGH);
  // }else{
  //   digitalWrite(led6,LOW);
  // }
  // if(hrs == horamedi[6] &&  mins == minmedi[6]){
  //   digitalWrite(led7, HIGH);
  // }else{
  //   digitalWrite(led7,LOW);
  // }
  // if(hrs == horamedi[7] &&  mins == minmedi[7]){
  //   digitalWrite(led8, HIGH);
  // }else{
  //   digitalWrite(led8,LOW);
  // }
  // if(hrs == horamedi[8] &&  mins == minmedi[8]){
  //   digitalWrite(led9, HIGH);
  // }else{
  //   digitalWrite(led9,LOW);
  // }

   printLocalTime();
  
  }
  
  if(led_statusC1 == 1){
    digitalWrite(led1, HIGH);
  }else{
    digitalWrite(led1,LOW);
  }
  if(led_statusC2 == 1){
    digitalWrite(led2, HIGH);
  }else{
    digitalWrite(led2,LOW);
  }
  if(led_statusC3 == 1){
    digitalWrite(led3, HIGH);
  }else{
   digitalWrite(led3,LOW);
  }

    // if(led_statusC4 == 1){
    //   digitalWrite(led4, HIGH);
    // }else{
    // digitalWrite(led4,LOW);
    // }
    // if(led_statusC5 == 1){
    //   digitalWrite(led5, HIGH);
    // }else{
    // digitalWrite(led5,LOW);
    // }

    // if(led_statusC6 == 1){
    //   digitalWrite(led6, HIGH);
    // }else{
    // digitalWrite(led6,LOW);
    // }

    // if(led_statusC7 == 1){
    //   digitalWrite(led7, HIGH);
    // }else{
    // digitalWrite(led7,LOW);
    // }

    // if(led_statusC8 == 1){
    //   digitalWrite(led8, HIGH);
    // }else{
    // digitalWrite(led8,LOW);
    // }

    // if(led_statusC9 == 1){
    //   digitalWrite(led9, HIGH);
    // }else{
    // digitalWrite(led9,LOW);
    // }
    


 printLocalTime();
}

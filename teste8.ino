//TIMESTAMP: ACIONAMENTO LED POR TEMPO

//Bibliotecas
#include <NTPClient.h>  // Biblioteca do NTP.
#include <WiFi.h>       // Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>

#define FIREBASE_HOST "https://projetoaulaextra-64b8d-default-rtdb.firebaseio.com"   
#define FIREBASE_AUTH "XWj6EaNcf9tpwxrAjAM995NJxDjZSceyxjIiLyjd"  
#define WIFI_SSID "Teste"               
#define WIFI_PASSWORD "paocomovos"

//Conexão com wifi
const char* ssid = "Demetria";
const char* password = "demizinha";

// --------------Configuração de relógio online -------------------
const char* ntpServer1 = "c.st1.ntp.br"; //Endereço 1
const char* ntpServer2 = "pool.ntp.org";//Endereço 2
const long gmtOffset_sec = 0; 
const int daylightOffset_sec = -3600*3;

int hrs = 0;
int mins = 0;

int Ch1;
int Cm1;  
int Ch2;
int Cm2;
int Ch3;
int Cm3;
int Ch4;
int Cm4;
int hora;
int minuto;
int horamedi;
int minmedi;




struct tm timeinfo;


int led1 = 15;
int led2 = 5;                      // definição dos pinos de saidas
int led3 = 4;
int led4 = 16;




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

  pinMode(led1, OUTPUT);  //Define o pino como saída
  // digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  // digitalWrite(led2, LOW);
  pinMode(led3, OUTPUT);
  // digitalWrite(led3, LOW);
    pinMode(led3, OUTPUT);
  // digitalWrite(led3, LOW);
  pinMode(led4, OUTPUT);
  // digitalWrite(led3, LOW);
    pinMode(led4, OUTPUT);
  // digitalWrite(led3, LOW);


  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  
  Serial.println(" WIFI CONECTADO!");
  delay(1000);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void AlarmTools(){

  Ch1 = Firebase.getInt("maleta/C1/hora");
  Cm1 = Firebase.getInt("maleta/C1/minu");  
  Ch2 = Firebase.getInt("maleta/C2/hora");
  Cm2 = Firebase.getInt("maleta/C2/minu");

  Ch3 = Firebase.getInt("maleta/C3/hora");
  Cm3 = Firebase.getInt("maleta/C3/minu");
  Ch4 = Firebase.getInt("maleta/C4/hora");
  Cm4 = Firebase.getInt("maleta/C4/minu");

  hora = Firebase.getInt("maleta/C1/adhoramed");

  minuto = Firebase.getInt("maleta/C1/adminmed");

  hrs = timeinfo.tm_hour; 
  mins = timeinfo.tm_min;

  horamedi = Ch1 + hora;
  minmedi = Cm1 + minuto;

}


void loop()
{
  AlarmTools();

  // Serial.println(Ch1);

  if(hrs == horamedi &&  mins == minmedi ){
    digitalWrite(led1, HIGH);
  }else{
    digitalWrite(led1,LOW);
  }
  
  // if(hrs == Ch2 && mins == Cm2 ){
  //   digitalWrite(led2, HIGH);
  // }else{
  //   digitalWrite(led2,LOW);
  // }
  // if(hrs == Ch3 && mins == Cm3 ){
  //   digitalWrite(led3, HIGH);
  // }else{
  //   digitalWrite(led3,LOW);
  // }
  // if(hrs == Ch4 && mins == Cm4 ){
  //   digitalWrite(led4, HIGH);
  // }else{
  //   digitalWrite(led4,LOW);
  // }

  delay(1000);
  printLocalTime();
}
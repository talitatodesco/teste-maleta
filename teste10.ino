//TIMESTAMP: ACIONAMENTO LED POR TEMPO

//Bibliotecas
#include <NTPClient.h>  // Biblioteca do NTP.
#include <WiFi.h>       // Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>

#define FIREBASE_HOST "https://projetoaulaextra-64b8d-default-rtdb.firebaseio.com"   
#define FIREBASE_AUTH "XWj6EaNcf9tpwxrAjAM995NJxDjZSceyxjIiLyjd"  
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

int Ch1;
int Cm1;  
int Ch2;
int Cm2;
int Ch3;
int Cm3;
int Ch4;
int Cm4;
int hora[4];
int minuto[4];
int horamedi[4];
int minmedi[4];
unsigned long tempoAtual     = 0;
unsigned long tempoAnterior1 = 0;
unsigned long tempoAnterior2 = 0;


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

  hora[0] = Firebase.getInt("maleta/C1/adhoramed");
  minuto[0] = Firebase.getInt("maleta/C1/adminmed");

  hora[1] = Firebase.getInt("maleta/C2/adhoramed");
  minuto[1] = Firebase.getInt("maleta/C2/adminmed");

  hora[2] = Firebase.getInt("maleta/C3/adhoramed");
  minuto[2] = Firebase.getInt("maleta/C3/adminmed");

  hora[3] = Firebase.getInt("maleta/C4/adhoramed");
  minuto[3] = Firebase.getInt("maleta/C4/adminmed");


  hrs = timeinfo.tm_hour; 
  mins = timeinfo.tm_min;

  horamedi[0] = Ch1 + hora[0];
  minmedi[0] = Cm1 + minuto[0];

  horamedi[1] = Ch2 + hora[1];
  minmedi[1] = Cm2 + minuto[1];

  horamedi[2] = Ch3 + hora[2];
  minmedi[2] = Cm3 + minuto[2];

  horamedi[3] = Ch4 + hora[3];
  minmedi[3] = Cm4 + minuto[3];

}


void loop()
{
 tempoAtual = millis();

// Executa a tarefa a cada 1 segundo.
  if (tempoAtual >= (tempoAnterior1 + 1000)) { // Verfica se passou 1 segundo.
    tempoAnterior1 = tempoAtual;               // atualiza a variavel de verificação de passagem de tempo.
    AlarmTools();                              // Verifica se ocorreu alguma atualização na temporização dos remédios.
  }

while(true){

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
  if(hrs == horamedi[3] &&  mins == minmedi[3]){
    digitalWrite(led4, HIGH);
  }else{
    digitalWrite(led4,LOW);
  }


  // delay(1000);
  printLocalTime();
  }
 
}
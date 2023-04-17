
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
  Serial.println(&timeinfo, "%R");  //Saida horario monitor serial

  //delay(1000);
}


void setup() {
  Serial.begin(115200);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2); //Configuração Time

  pinMode(led1, OUTPUT);  //Define o pino como saída
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led4, OUTPUT);


  // Teste dos Leds da maleta
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led3, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led3, LOW);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  
  Serial.println(" WIFI CONECTADO!");
 // delay(1000);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Busca no banco de dados os horários dos remédios de cada compartimento da maleta
void AlarmTools(){

  Ch1 = Firebase.getInt("maleta/C1/hora");
  Cm1 = Firebase.getInt("maleta/C1/minu");  
  Ch2 = Firebase.getInt("maleta/C2/hora");
  Cm2 = Firebase.getInt("maleta/C2/minu");

  Ch3 = Firebase.getInt("maleta/C3/hora");
  Cm3 = Firebase.getInt("maleta/C3/minu");
  Ch4 = Firebase.getInt("maleta/C4/hora");
  Cm4 = Firebase.getInt("maleta/C4/minu");

  hrs = timeinfo.tm_hour; 
  mins = timeinfo.tm_min;
}


void loop()
{
  tempoAtual = millis();

// Executa a tarefa a cada 1 segundo.
  if (tempoAtual >= (tempoAnterior1 + 1000)) { // Verfica se passou 1 segundo.
    tempoAnterior1 = tempoAtual;               // atualiza a variavel de verificação de passagem de tempo.
    AlarmTools();                              // Verifica se ocorreu alguma atualização na temporização dos remédios.
  }

  // Serial.println(Ch1);

  if(hrs == Ch1 && mins == Cm1 ){
    digitalWrite(led1, HIGH);
  }else{
    digitalWrite(led1,LOW);
  }
  
  if(hrs == Ch2 && mins == Cm2 ){
    digitalWrite(led2, HIGH);
  }else{
    digitalWrite(led2,LOW);
  }
  if(hrs == Ch3 && mins == Cm3 ){
    digitalWrite(led3, HIGH);
  }else{
    digitalWrite(led3,LOW);
  }
  if(hrs == Ch4 && mins == Cm4 ){
    digitalWrite(led4, HIGH);
  }else{
    digitalWrite(led4,LOW);
  }

  //delay(1000);
  printLocalTime();
}
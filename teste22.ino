//Biblioteca
#include <NTPClient.h>  // Biblioteca do NTP.
#include <WiFi.h>       // Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>
#include <TimeAlarms.h>
#include <TimeLib.h>


#define FIREBASE_HOST "https://projetoaulaextra-64b8d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "XWj6EaNcf9tpwxrAjAM995NJxDjZSceyxjIiLyjd"
#define WIFI_SSID "Demetria"
#define WIFI_PASSWORD "demizinha"

#define led1 15
#define led2 2  // definição dos pinos de saidas
#define led3 4
#define led4 16
#define led5 5  // definição dos pinos de saidas
#define led6 18
#define led7 19
#define led8 21
#define led9 32
#define buzzer 27

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;


//Conexão com wifi
const char* ssid = "Demetria";
const char* password = "demizinha";
// --------------Configuração de relógio online -------------------
const char* ntpServer1 = "c.st1.ntp.br";  //Endereço 1
const char* ntpServer2 = "pool.ntp.org";  //Endereço 2
const long gmtOffset_sec = 0;
const int daylightOffset_sec = -3600 * 3;

//***** Variáveis para controle de tempo ****
unsigned long tempoAtual;
unsigned long tempoAlarmeTools;
unsigned long tempoTesteMaleta;

unsigned long tempoAtualCase0;
unsigned long tempoAtualCase0_1;
//*******************************************
unsigned char step;  // Controle da máquina de estado para leitura da programação dos compartimentos.
unsigned char stepTesteMaleta;


int hrs = 0;
int mins = 0;
uint8_t Ch[8];
uint8_t Cm[8];
uint8_t hora[8];
uint8_t minuto[8];
uint8_t horamedi[8];
uint8_t minmedi[8];

int funcaoalarme;
int funcao;
int led_statusC1;
int led_statusC2;
int led_statusC3;
int led_statusC4;
int led_statusC5;
int led_statusC6;
int led_statusC7;
int led_statusC8;
int led_statusC9;
int buzzer_status;


struct tm timeinfo;

void printLocalTime() {

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Não foi possivel obter o tempo");
    return;
  }                                    //Caso não consiga buscar o horario
  Serial.println(&timeinfo, "%H:%M");  //Saida horario monitor serial
}


void setup() {
  step = 0;
  stepTesteMaleta = 0;
  tempoAtual = 0;
  tempoAlarmeTools = 0;
  tempoTesteMaleta = 0;

  Serial.begin(115200);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);  //Configuração Time

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);
  pinMode(buzzer, OUTPUT);

  ledcSetup(ledChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(buzzer, ledChannel);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println(" WIFI CONECTADO!");

  // -------- Conexão Firebase --------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void calculaHora(uint8_t Ch, uint8_t Cm, uint8_t hora, uint8_t minuto, uint8_t* valorHora, uint8_t* valorMinuto) {
  *valorHora = Ch + hora;
  *valorMinuto = Cm + minuto;

  // Correção dos minutos e horas
  if (*valorMinuto >= 60) {
    *valorHora += *valorMinuto / 60;  // Soma o resultado da divisão ao valor da Hora
    *valorMinuto %= 60;               // Recebe o resto da divisão, desta forma teremos o valor em minutos.
  }

  // Correção do valor das Horas
  if (*valorHora >= 24) {
    *valorHora %= 24;
  }
}

void incrementoDaHora(uint8_t incHora, uint8_t incMinutos, uint8_t* hora, uint8_t* minutos) {
  // Correção dos minutos e horas
  *minutos += incMinutos;
  *hora += incHora;
  if (*minutos >= 60) {
    *hora += *minutos / 60;  // Soma o resultado da divisão ao valor da Hora
    *minutos %= 60;          // Recebe o resto da divisão, desta forma teremos o valor em minutos.
  }

  // Correção do valor das Horas
  if (*hora >= 24) {
    *hora %= 24;
  }
}

// Função declaração alarme
void AlarmTools() {

  Serial.println("*********");
  Serial.print("step:");
  Serial.println(step);

  switch (step) {
    case 0:
      tempoAtualCase0 = millis();
      funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
      hrs = timeinfo.tm_hour;
      mins = timeinfo.tm_min;
      step++;
      break;

    case 1:
      tempoAtualCase0 = millis();
      Ch[0] = Firebase.getInt("maleta/C1/hora");
      Cm[0] = Firebase.getInt("maleta/C1/min");
      hora[0] = Firebase.getInt("maleta/C1/adhoramed");
      minuto[0] = Firebase.getInt("maleta/C1/adminmed");
      // horamedi[0] = Ch[0] + hora[0];
      // minmedi[0] = Cm[0] + minuto[0];
      calculaHora(Ch[0], Cm[0], hora[0], minuto[0], &horamedi[0], &minmedi[0]);
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 2:
      tempoAtualCase0 = millis();
      Ch[1] = Firebase.getInt("maleta/C2/hora");
      Cm[1] = Firebase.getInt("maleta/C2/min");
      hora[1] = Firebase.getInt("maleta/C2/adhoramed");
      minuto[1] = Firebase.getInt("maleta/C2/adminmed");
      horamedi[1] = Ch[1] + hora[1];
      minmedi[1] = Cm[1] + minuto[1];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 3:
      tempoAtualCase0 = millis();
      Ch[2] = Firebase.getInt("maleta/C3/hora");
      Cm[2] = Firebase.getInt("maleta/C3/min");
      hora[2] = Firebase.getInt("maleta/C3/adhoramed");
      minuto[2] = Firebase.getInt("maleta/C3/adminmed");
      horamedi[2] = Ch[2] + hora[2];
      minmedi[2] = Cm[2] + minuto[2];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 4:
      tempoAtualCase0 = millis();
      Ch[3] = Firebase.getInt("maleta/C4/hora");
      Cm[3] = Firebase.getInt("maleta/C4/min");
      hora[3] = Firebase.getInt("maleta/C4/adhoramed");
      minuto[3] = Firebase.getInt("maleta/C4/adminmed");
      horamedi[3] = Ch[3] + hora[3];
      minmedi[3] = Cm[3] + minuto[3];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 5:
      tempoAtualCase0 = millis();
      Ch[4] = Firebase.getInt("maleta/C5/hora");
      Cm[4] = Firebase.getInt("maleta/C5/min");
      hora[4] = Firebase.getInt("maleta/C5/adhoramed");
      minuto[4] = Firebase.getInt("maleta/C5/adminmed");
      horamedi[4] = Ch[4] + hora[4];
      minmedi[4] = Cm[4] + minuto[4];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 6:
      tempoAtualCase0 = millis();
      Ch[5] = Firebase.getInt("maleta/C6/hora");
      Cm[5] = Firebase.getInt("maleta/C6/min");
      hora[5] = Firebase.getInt("maleta/C6/adhoramed");
      minuto[5] = Firebase.getInt("maleta/C6/adminmed");
      horamedi[5] = Ch[5] + hora[5];
      minmedi[5] = Cm[5] + minuto[5];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 7:
      tempoAtualCase0 = millis();
      Ch[6] = Firebase.getInt("maleta/C7/hora");
      Cm[6] = Firebase.getInt("maleta/C7/min");
      hora[6] = Firebase.getInt("maleta/C7/adhoramed");
      minuto[6] = Firebase.getInt("maleta/C7/adminmed");
      horamedi[6] = (Ch[6] + hora[6]) * horamedi[6];
      minmedi[6] = (Cm[6] + minuto[6]) * minmedi[6];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;

    case 8:
      tempoAtualCase0 = millis();
      Ch[7] = Firebase.getInt("maleta/C8/hora");
      Cm[7] = Firebase.getInt("maleta/C8/min");
      hora[7] = Firebase.getInt("maleta/C8/adhoramed");
      minuto[7] = Firebase.getInt("maleta/C8/adminmed");
      horamedi[7] = Ch[7] + hora[7];
      minmedi[7] = Cm[7] + minuto[7];
      step++;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);

      break;

    case 9:
      tempoAtualCase0 = millis();
      Ch[8] = Firebase.getInt("maleta/C9/hora");
      Cm[8] = Firebase.getInt("maleta/C9/min");
      hora[8] = Firebase.getInt("maleta/C9/adhoramed");
      minuto[8] = Firebase.getInt("maleta/C9/adminmed");
      step = 0;
      tempoAtualCase0_1 = millis();
      Serial.println("*********");
      Serial.print("Tempo para ler os dados:");
      Serial.println(tempoAtualCase0_1 - tempoAtualCase0);
      break;


    default:
      step = 0;
      break;
  }
}


void loop() {
  tempoAtual = millis();

  if (tempoAtual >= (tempoAlarmeTools + 5000)) {
    tempoAlarmeTools = tempoAtual;
    AlarmTools();
  }
  // Serial.println("*********");
  // Serial.print("tempoTesteMaleta:");
  // Serial.println(tempoTesteMaleta);

  // Serial.println("*********");
  // Serial.print("stepTesteMaleta:");
  // Serial.println(stepTesteMaleta);

  if (tempoAtual >= (tempoTesteMaleta + 50)) {
    tempoTesteMaleta = tempoAtual;

    if (!funcaoalarme) {
      switch (stepTesteMaleta) {
        case 0:
          buzzer_status = Firebase.getBool("maleta/buzzer_status");
          stepTesteMaleta++;
          break;
        case 1:
          led_statusC1 = Firebase.getBool("maleta/C1/led_status");
          stepTesteMaleta++;
          break;
        case 2:
          led_statusC2 = Firebase.getBool("maleta/C2/led_status");
          stepTesteMaleta++;
          break;
        case 3:
          led_statusC3 = Firebase.getBool("maleta/C3/led_status");
          stepTesteMaleta++;
          break;
        case 4:
          led_statusC4 = Firebase.getBool("maleta/C4/led_status");
          stepTesteMaleta++;
          break;
        case 5:
          led_statusC5 = Firebase.getBool("maleta/C5/led_status");
          stepTesteMaleta++;
          break;
        case 6:
          led_statusC6 = Firebase.getBool("maleta/C6/led_status");
          stepTesteMaleta++;
          break;
        case 7:
          led_statusC7 = Firebase.getBool("maleta/C7/led_status");
          stepTesteMaleta++;
          break;
        case 8:
          led_statusC8 = Firebase.getBool("maleta/C8/led_status");
          stepTesteMaleta++;
          break;
        case 9:
          led_statusC9 = Firebase.getBool("maleta/C9/led_status");
          stepTesteMaleta = 0;
          break;
        default:
          stepTesteMaleta = 0;
          break;
      }
    }
  }


  // Operação da Maleta, indica ao usuário quando deve tomar o remédio
  if (funcaoalarme) {
    Serial.println(minmedi[0]);
    incrementoDaHora(hora[0], minuto[0], &horamedi[0], &minmedi[0]);  // Próxima hora de tomar o remédio


    // if (hrs == horamedi[1] && mins == minmedi[1]) {
    //   digitalWrite(led2, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led2, LOW);
    // }
    // if (hrs == horamedi[2] && mins == minmedi[2]) {
    //   digitalWrite(led3, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led3, LOW);
    // }
    // if (hrs == horamedi[3] && mins == minmedi[3]) {
    //   digitalWrite(led4, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led4, LOW);
    // }
    // if (hrs == horamedi[4] && mins == minmedi[4]) {
    //   digitalWrite(led5, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led5, LOW);
    // }
    // if (hrs == horamedi[5] && mins == minmedi[5]) {
    //   digitalWrite(led6, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led6, LOW);
    // }
    // if (hrs == horamedi[6] && mins == minmedi[6]) {
    //   digitalWrite(led7, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led7, LOW);
    // }
    // if (hrs == horamedi[7] && mins == minmedi[7]) {
    //   digitalWrite(led8, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led8, LOW);
    // }
    // if (hrs == horamedi[8] && mins == minmedi[8]) {
    //   digitalWrite(led9, HIGH);
    //   ledcWrite(ledChannel, 5000);
    // } else {
    //   digitalWrite(led9, LOW);
    // }

    //printLocalTime();
  }

  // Verifica se deve executar a rotina de teste da maleta
  if (!funcaoalarme) {
    if (led_statusC1 == 1) {
      digitalWrite(led1, HIGH);
      delay(5000);
      digitalWrite(led1, LOW);
      led_statusC1 = false;
      Firebase.setBool("maleta/C1/led_status", led_statusC1);
    }
    if (led_statusC2 == 1) {
      digitalWrite(led2, HIGH);
      delay(5000);
      digitalWrite(led2, LOW);
      led_statusC2 = false;
      Firebase.setBool("maleta/C2/led_status", led_statusC2);
    }
    if (led_statusC3 == 1) {
      digitalWrite(led3, HIGH);
      delay(5000);
      digitalWrite(led3, LOW);
      led_statusC3 = false;
      Firebase.setBool("maleta/C3/led_status", led_statusC3);
    }
    if (led_statusC4 == 1) {
      digitalWrite(led4, HIGH);
      delay(5000);
      digitalWrite(led4, LOW);
      led_statusC4 = false;
      Firebase.setBool("maleta/C4/led_status", led_statusC4);
    }
    if (led_statusC5 == 1) {
      digitalWrite(led5, HIGH);
      delay(5000);
      digitalWrite(led5, LOW);
      led_statusC5 = false;
      Firebase.setBool("maleta/C5/led_status", led_statusC5);
    }

    if (led_statusC6 == 1) {
      digitalWrite(led6, HIGH);
      delay(5000);
      digitalWrite(led6, LOW);
      led_statusC6 = false;
      Firebase.setBool("maleta/C6/led_status", led_statusC6);
    }

    if (led_statusC7 == 1) {
      digitalWrite(led7, HIGH);
      delay(5000);
      digitalWrite(led7, LOW);
      led_statusC7 = false;
      Firebase.setBool("maleta/C7/led_status", led_statusC7);
    }

    if (led_statusC8 == 1) {
      digitalWrite(led8, HIGH);
      delay(5000);
      digitalWrite(led8, LOW);
      led_statusC8 = false;
      Firebase.setBool("maleta/C8/led_status", led_statusC8);
    }

    if (led_statusC9 == 1) {
      digitalWrite(led9, HIGH);
      delay(5000);
      digitalWrite(led9, LOW);
      led_statusC9 = false;
      Firebase.setBool("maleta/C9/led_status", led_statusC9);
    }
    if (buzzer_status == 1) {
      ledcWriteTone(ledChannel, 1000);
      delay(1000);
      buzzer_status = false;
      Firebase.setBool("maleta/buzzer_status", buzzer_status);
    }
  }
  printLocalTime();
}
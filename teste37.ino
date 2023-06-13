//Biblioteca
#include <NTPClient.h>  // Biblioteca do NTP.
#include <WiFi.h>       // Biblioteca do WiFi.
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>
#include <TimeAlarms.h>
#include <TimeLib.h>

// #define FIREBASE_HOST "https://projetoaulaextra-64b8d-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "XWj6EaNcf9tpwxrAjAM995NJxDjZSceyxjIiLyjd"

#define FIREBASE_HOST "https://malit-3b137-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "qHaoj4vl7AqZ1qlBr0ruwjAMvUOmX3DQcFOOxp1X"

#define WIFI_SSID "CLARO_2G544AC8"
#define WIFI_PASSWORD "96544AC8"

#define led1 14
#define led2 27
#define led3 12
#define led4 13
#define led5 18
#define led6 5
#define led7 22
#define led8 19
#define led9 15
#define buzzer 26

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const char* ssid = "CLARO_2G544AC8";
const char* password = "96544AC8";

const char* ntpServer1 = "c.st1.ntp.br";  //Endereço 1
const char* ntpServer2 = "pool.ntp.org";  //Endereço 2
const long gmtOffset_sec = -3600 * 3;
const int daylightOffset_sec = 0;

unsigned long tempoAtual;
unsigned long tempoAlarmeTools;
unsigned long tempoTesteMaleta;
unsigned long tempoLed;

unsigned long tempoAtualCase0;
unsigned long tempoAtualCase0_1;

unsigned char step;
unsigned char stepTesteMaleta;
unsigned char stepFuncaoAlarme;

uint8_t hrs = 0;
uint8_t mins = 0;


String Ch[9];
uint8_t horaInicial[9];
String Cm[9];
uint8_t minutoInicial[9];
String hora[9];
uint8_t intervaloHora[9];
String minuto[9];

uint8_t intervaloMinuto[9];
uint8_t horamedi[9];
uint8_t minmedi[9];
uint8_t horaInicialAnterior[9];
uint8_t minutoInicialAnterior[9];
uint8_t intervaloHoraAnterior[9];
uint8_t intervaloMinutoAnterior[9];
bool ajustarHoraMinuto[9];
uint8_t desligaLed;

bool funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
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
  }
  Serial.println(&timeinfo, "%H:%M");
}




void calculaHora(uint8_t horaInicial, uint8_t minutoInicial, uint8_t hora, uint8_t minuto, uint8_t* valorHora, uint8_t* valorMinuto) {
  *valorHora = horaInicial + hora;
  *valorMinuto = minutoInicial + minuto;

  if (*valorMinuto >= 60) {
    *valorHora += *valorMinuto / 60;
    *valorMinuto %= 60;
  }

  if (*valorHora >= 24) {
    *valorHora %= 24;
  }


  while (*valorHora < hrs || *valorMinuto < mins) {
    incrementoDaHora(hora, minuto, &(*valorHora), &(*valorMinuto));
  }
}
void incrementoDaHora(uint8_t incHora, uint8_t incMinutos, uint8_t* hora, uint8_t* minutos) {
  *minutos += incMinutos;
  *hora += incHora;
  if (*minutos >= 60) {
    *hora += *minutos / 60;
    *minutos %= 60;
  }
  if (*hora >= 24) {
    *hora %= 24;
  }
}


bool houveAtualizacao(uint8_t compartimento) {
  bool status = false;

  if (horaInicial[compartimento] != horaInicialAnterior[compartimento]) {
    horaInicialAnterior[compartimento] = horaInicial[compartimento];
    status = true;
  }
  if (minutoInicial[compartimento] != minutoInicialAnterior[compartimento]) {
    minutoInicialAnterior[compartimento] = minutoInicial[compartimento];
    status = true;
  }
  if (intervaloHora[compartimento] != intervaloHoraAnterior[compartimento]) {
    intervaloHoraAnterior[compartimento] = intervaloHora[compartimento];
    status = true;
  }
  if (intervaloMinuto[compartimento] != intervaloMinutoAnterior[compartimento]) {
    intervaloMinutoAnterior[compartimento] = intervaloMinuto[compartimento];
    status = true;
  }
  return status;
}


void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println(" WIFI CONECTADO!");
  WiFi.begin(ssid, password);

  step = 0;
  stepFuncaoAlarme = 0;
  desligaLed = 0;
  stepTesteMaleta = 0;
  tempoAtual = 0;
  tempoAlarmeTools = 0;
  tempoTesteMaleta = 0;

  Serial.begin(115200);

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

  ledcAttachPin(buzzer, ledChannel);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer2, ntpServer1);  //Configuração Time
  delay(250);
  printLocalTime();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  for (uint8_t x = 0; x < 9; x++) {
    horamedi[x] = hrs;
    minmedi[x] = mins;

    horaInicialAnterior[x] = 255;
    minutoInicialAnterior[x] = 255;
    intervaloHoraAnterior[x] = 255;
    intervaloMinutoAnterior[x] = 255;
  }
}


void AlarmTools() {
  Serial.println("*********");
  Serial.print("step:");
  Serial.println(step);

  switch (step) {
    case 0:
      funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
      hrs = timeinfo.tm_hour;
      mins = timeinfo.tm_min;
      step++;
      break;


    case 1:
      if (funcaoalarme) {
        Ch[0] = Firebase.getString("maleta/C1/horarioInicial");
        horaInicial[0] = atoi(Ch[0].c_str());
        Cm[0] = Firebase.getString("maleta/C1/minutoInicial");
        minutoInicial[0] = atoi(Cm[0].c_str());
        hora[0] = Firebase.getString("maleta/C1/intervaloHoras");
        intervaloHora[0] = atoi(hora[0].c_str());
        minuto[0] = Firebase.getString("maleta/C1/adminmed");
        intervaloMinuto[0] = atoi(minuto[0].c_str());
        if (houveAtualizacao(0)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[0], minutoInicial[0], intervaloHora[0], intervaloMinuto[0], &horamedi[0], &minmedi[0]);
        }
        step++;
      } else {
        funcaoalarme;
        step++;
        // return funcaoalarme;
      }
      break;

    case 2:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[1] = Firebase.getString("maleta/C2/horarioInicial");
        horaInicial[1] = atoi(Ch[1].c_str());
        Cm[1] = Firebase.getString("maleta/C2/minutoInicial");
        minutoInicial[1] = atoi(Cm[1].c_str());
        hora[1] = Firebase.getString("maleta/C2/intervaloHoras");
        intervaloHora[1] = atoi(hora[1].c_str());
        minuto[1] = Firebase.getString("maleta/C2/adminmed");
        intervaloMinuto[1] = atoi(minuto[1].c_str());
        if (houveAtualizacao(1)) {
          calculaHora(horaInicial[1], minutoInicial[1], intervaloHora[1], intervaloMinuto[1], &horamedi[1], &minmedi[1]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;

    case 3:

      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[2] = Firebase.getString("maleta/C3/horarioInicial");
        horaInicial[2] = atoi(Ch[2].c_str());
        Cm[2] = Firebase.getString("maleta/C3/minutoInicial");
        minutoInicial[2] = atoi(Cm[2].c_str());
        hora[2] = Firebase.getString("maleta/C3/intervaloHoras");
        intervaloHora[2] = atoi(hora[2].c_str());
        minuto[2] = Firebase.getString("maleta/C3/adminmed");
        intervaloMinuto[2] = atoi(minuto[2].c_str());
        if (houveAtualizacao(2)) {
          calculaHora(horaInicial[2], minutoInicial[2], intervaloHora[2], intervaloMinuto[2], &horamedi[2], &minmedi[2]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;


    case 4:

      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[3] = Firebase.getString("maleta/C4/horarioInicial");
        horaInicial[3] = atoi(Ch[3].c_str());
        Cm[3] = Firebase.getString("maleta/C4/minutoInicial");
        minutoInicial[3] = atoi(Cm[3].c_str());
        hora[3] = Firebase.getString("maleta/C4/intervaloHoras");
        intervaloHora[3] = atoi(hora[3].c_str());
        minuto[3] = Firebase.getString("maleta/C4/adminmed");
        intervaloMinuto[3] = atoi(minuto[3].c_str());
        if (houveAtualizacao(3)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[3], minutoInicial[3], intervaloHora[3], intervaloMinuto[3], &horamedi[3], &minmedi[3]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;


    case 5:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[4] = Firebase.getString("maleta/C5/horarioInicial");
        horaInicial[4] = atoi(Ch[4].c_str());
        Cm[4] = Firebase.getString("maleta/C5/minutoInicial");
        minutoInicial[4] = atoi(Cm[4].c_str());
        hora[4] = Firebase.getString("maleta/C5/intervaloHoras");
        intervaloHora[4] = atoi(hora[4].c_str());
        minuto[4] = Firebase.getString("maleta/C5/adminmed");
        intervaloMinuto[4] = atoi(minuto[4].c_str());
        if (houveAtualizacao(4)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[4], minutoInicial[4], intervaloHora[4], intervaloMinuto[4], &horamedi[4], &minmedi[4]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;

    case 6:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[5] = Firebase.getString("maleta/C6/horarioInicial");
        horaInicial[5] = atoi(Ch[5].c_str());
        Cm[5] = Firebase.getString("maleta/C6/minutoInicial");
        minutoInicial[5] = atoi(Cm[5].c_str());
        hora[5] = Firebase.getString("maleta/C6/intervaloHoras");
        intervaloHora[5] = atoi(hora[5].c_str());
        minuto[5] = Firebase.getString("maleta/C6/adminmed");
        intervaloMinuto[5] = atoi(minuto[5].c_str());
        if (houveAtualizacao(5)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[5], minutoInicial[5], intervaloHora[5], intervaloMinuto[5], &horamedi[5], &minmedi[5]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;

    case 7:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[6] = Firebase.getString("maleta/C7/horarioInicial");
        horaInicial[6] = atoi(Ch[6].c_str());
        Cm[6] = Firebase.getString("maleta/C7/minutoInicial");
        minutoInicial[6] = atoi(Cm[6].c_str());
        hora[6] = Firebase.getString("maleta/C7/intervaloHoras");
        intervaloHora[6] = atoi(hora[6].c_str());
        minuto[6] = Firebase.getString("maleta/C7/adminmed");
        intervaloMinuto[6] = atoi(minuto[6].c_str());
        if (houveAtualizacao(6)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[6], minutoInicial[6], intervaloHora[6], intervaloMinuto[6], &horamedi[6], &minmedi[6]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;


    case 8:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[7] = Firebase.getString("maleta/C8/horarioInicial");
        horaInicial[7] = atoi(Ch[7].c_str());
        Cm[7] = Firebase.getString("maleta/C8/minutoInicial");
        minutoInicial[7] = atoi(Cm[7].c_str());
        hora[7] = Firebase.getString("maleta/C8/intervaloHoras");
        intervaloHora[7] = atoi(hora[7].c_str());
        minuto[7] = Firebase.getString("maleta/C8/adminmed");
        intervaloMinuto[7] = atoi(minuto[7].c_str());
        if (houveAtualizacao(7)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[7], minutoInicial[7], intervaloHora[7], intervaloMinuto[7], &horamedi[7], &minmedi[7]);
        }
        step++;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step++;
        // return funcaoalarme;
      }
      break;

    case 9:
      if (funcaoalarme) {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        Ch[8] = Firebase.getString("maleta/C9/horarioInicial");
        horaInicial[8] = atoi(Ch[8].c_str());
        Cm[8] = Firebase.getString("maleta/C9/minutoInicial");
        minutoInicial[8] = atoi(Cm[8].c_str());
        hora[8] = Firebase.getString("maleta/C9/intervaloHoras");
        intervaloHora[8] = atoi(hora[8].c_str());
        minuto[8] = Firebase.getString("maleta/C9/adminmed");
        intervaloMinuto[8] = atoi(minuto[8].c_str());
        if (houveAtualizacao(8)) {  // Se houve atualização da programação, recalcula a hora de tomar o remédio.
          calculaHora(horaInicial[8], minutoInicial[8], intervaloHora[8], intervaloMinuto[8], &horamedi[8], &minmedi[8]);
        }
        step = 0;
      } else {
        funcaoalarme = Firebase.getBool("maleta/funcaoalarme");
        step = 0;
        // return funcaoalarme;
      }

    default:
      step = 0;
      break;
  }
}

//Controla o estado do Buzzer, ligado ou desligado
void CtrBuzzer(bool ligaDesl) {
  if (ligaDesl) {
    ledcAttachPin(buzzer, ledChannel);
    ledcWriteTone(ledChannel, 5000);
  } else {
    ledcWriteTone(ledChannel, 0);
    ledcDetachPin(buzzer);
  }
}




void loop() {
  tempoAtual = millis();

  if (tempoAtual >= (tempoAlarmeTools + 2000)) {
    tempoAlarmeTools = tempoAtual;
    AlarmTools();
  }

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


    Serial.println(funcaoalarme);

    // Apaga o Led e desliga o Buzzer
    if (tempoAtual >= (tempoLed + 5000)) {
      tempoLed = tempoAtual;
      if (desligaLed) {
        Serial.println("Desliga Led");
        digitalWrite(desligaLed, LOW);
        CtrBuzzer(false);
        desligaLed = 0;
        //delay(1000);
      }
    }

    Serial.println("********1");
    Serial.println("Hora de tomar remédio:");
    Serial.println(hrs);
    Serial.println(mins);
    Serial.println(horamedi[0]);
    Serial.println(minmedi[0]);


    switch (stepFuncaoAlarme) {
      case 1:
        if (hrs >= horamedi[0] && mins >= minmedi[0]) {
          desligaLed = led1;
          digitalWrite(led1, HIGH);
          delay(900);
          CtrBuzzer(true);  // Liga o Buzzer
          // Determina o próximo horário para tomar o remédio
          incrementoDaHora(intervaloHora[0], intervaloMinuto[0], &horamedi[0], &minmedi[0]);
          Serial.println("********1");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[0]);
          Serial.println(minmedi[0]);

        } else {
          digitalWrite(led1, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 2:
        if (hrs >= horamedi[1] && mins >= minmedi[1]) {
          desligaLed = led2;
          digitalWrite(led2, HIGH);
          delay(900);
          CtrBuzzer(true);  // Liga o Buzzer
          // Determina o próximo horário para tomar o remédio
          incrementoDaHora(intervaloHora[1], intervaloMinuto[1], &horamedi[1], &minmedi[1]);
          Serial.println("********2");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[1]);
          Serial.println(minmedi[1]);
        } else {
          digitalWrite(led2, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 3:

        if (hrs >= horamedi[2] && mins >= minmedi[2]) {
          desligaLed = led3;
          digitalWrite(led3, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[2], intervaloMinuto[2], &horamedi[2], &minmedi[2]);

          Serial.println("********3");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[2]);
          Serial.println(minmedi[2]);
        } else {
          digitalWrite(led3, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 4:

        if (hrs >= horamedi[3] && mins >= minmedi[3]) {
          desligaLed = led4;
          digitalWrite(led4, HIGH);
          delay(900);
          CtrBuzzer(true);

          incrementoDaHora(intervaloHora[3], intervaloMinuto[3], &horamedi[3], &minmedi[3]);

          Serial.println("********4");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[3]);
          Serial.println(minmedi[3]);
        } else {
          digitalWrite(led4, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 5:

        if (hrs >= horamedi[4] && mins >= minmedi[4]) {
          desligaLed = led5;
          digitalWrite(led5, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[4], intervaloMinuto[4], &horamedi[4], &minmedi[4]);

          Serial.println("********5");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[4]);
          Serial.println(minmedi[4]);
        } else {
          digitalWrite(led5, LOW);
        };
        stepFuncaoAlarme++;
        break;


      case 6:

        if (hrs >= horamedi[5] && mins >= minmedi[5]) {
          desligaLed = led6;
          digitalWrite(led6, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[5], intervaloMinuto[5], &horamedi[5], &minmedi[5]);

          Serial.println("********6");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[5]);
          Serial.println(minmedi[5]);
        } else {
          digitalWrite(led6, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 7:

        if (hrs >= horamedi[6] && mins >= minmedi[6]) {
          desligaLed = led7;
          digitalWrite(led7, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[6], intervaloMinuto[6], &horamedi[6], &minmedi[6]);
          Serial.println("********7");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[6]);
          Serial.println(minmedi[6]);
        } else {
          digitalWrite(led7, LOW);
        };
        stepFuncaoAlarme++;
        break;

      case 8:

        if (hrs >= horamedi[7] && mins >= minmedi[7]) {
          desligaLed = led8;
          digitalWrite(led8, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[7], intervaloMinuto[7], &horamedi[7], &minmedi[7]);

          Serial.println("********8");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[7]);
          Serial.println(minmedi[7]);
        } else {
          digitalWrite(led8, LOW);
        };

        stepFuncaoAlarme++;
        break;

      case 9:
        if (hrs >= horamedi[8] && mins >= minmedi[8]) {
          desligaLed = led9;
          digitalWrite(led9, HIGH);
          delay(900);
          CtrBuzzer(true);
          incrementoDaHora(intervaloHora[8], intervaloMinuto[8], &horamedi[8], &minmedi[8]);

          Serial.println("********9");
          Serial.println("Hora de tomar remédio:");
          Serial.println(horamedi[8]);
          Serial.println(minmedi[8]);
        } else {
          digitalWrite(led9, LOW);
        };
        stepFuncaoAlarme = 0;
        break;

      default:
        stepFuncaoAlarme = 0;
    }
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

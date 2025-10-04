#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ===================== CONFIGURAÇÃO DO WI-FI =====================
const char* ssid = "AMF";           // Seu SSID
const char* password = "amf@2025";  // Sua senha

void conectaWiFi() {
  Serial.print("Conectando ao Wi-Fi ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    tentativas++;
    if (tentativas > 20) { // timeout 20s
      Serial.println("\nFalha ao conectar. Verifique SSID/senha.");
      return;
    }
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ===================== CONFIGURAÇÃO DO ENDPOINT =====================
const char* endpointInsert = "https://icy-bush-2dad.cairolopes011.workers.dev/insert";
const char* endpointStatus = "https://icy-bush-2dad.cairolopes011.workers.dev/status";
const char* endpointClear  = "https://icy-bush-2dad.cairolopes011.workers.dev/clear";

// ===================== PINOS =====================
const int PIR = 23;      // Sensor de movimento
const int MIC_D0 = 22;   // Microfone digital
const int BUZZER = 21;   // Buzzer PWM

// ===================== CONFIGURAÇÕES DO BUZZER =====================
const int BUZZER_VOLUME = 10;    // 0 a 255 (baixo)
const int BUZZER_DURATION = 500; // ms

// ===================== CONTROLE DE ALARME =====================
bool alarmeAtivo = false;

// ===================== FUNÇÃO PARA ENVIAR EVENTO =====================
void enviaEvento(const char* tipo) {
  if(WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;
    client.setInsecure(); // aceita qualquer certificado (apenas teste)
    HTTPClient http;

    http.begin(client, endpointInsert);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"tipo\":\"";
    payload += tipo;
    payload += "\"}";

    int httpResponseCode = http.POST(payload);
    
    if(httpResponseCode > 0){
      Serial.print("Evento enviado, HTTP code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro enviando: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Wi-Fi não conectado");
  }
}

// ===================== FUNÇÃO PARA CHECAR COMANDO REMOTO =====================
void checkComando() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    http.begin(client, endpointStatus);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String resposta = http.getString();
      Serial.println("Resposta Worker: " + resposta);

      if (resposta.indexOf("on") > 0) {
        Serial.println("Comando remoto recebido: Ativar buzzer!");

        for (int i = 0; i < 2; i++) {
          ledcWrite(0, BUZZER_VOLUME); // liga buzzer
          delay(BUZZER_DURATION);      // duração do som
          ledcWrite(0, 0);             // desliga buzzer
          delay(200);                  // pausa entre apitos (200ms, pode ajustar)
        }

        // Confirma execução limpando comando
        HTTPClient clear;
        clear.begin(client, endpointClear);
        clear.POST("");
        clear.end();
      }
    } else {
      Serial.print("Erro ao checar comando: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  pinMode(PIR, INPUT);
  pinMode(MIC_D0, INPUT);

  // Configura PWM para buzzer
  ledcSetup(0, 2000, 8); // canal 0, 2 kHz, 8 bits
  ledcAttachPin(BUZZER, 0);
  ledcWrite(0, 0); // começa desligado

  conectaWiFi();

  Serial.println("Sistema de alerta iniciado!");
}

// ===================== LOOP PRINCIPAL =====================
void loop() {
  int movimento = digitalRead(PIR);
  int som = digitalRead(MIC_D0);

  // Verifica detecção
  if ((movimento == HIGH || som == HIGH) && !alarmeAtivo) {
    alarmeAtivo = true;

    if (movimento == HIGH) Serial.println("Movimento detectado!");
    if (som == HIGH) Serial.println("Som detectado!");

    if (movimento == HIGH) enviaEvento("movimento");
    if (som == HIGH) enviaEvento("som");

    // Liga buzzer
    ledcWrite(0, BUZZER_VOLUME);
    delay(BUZZER_DURATION);

    // Desliga buzzer
    ledcWrite(0, 0);
    alarmeAtivo = false;
  }

  // 🔔 Verifica se botão foi apertado na página
  checkComando();

  delay(500);
}

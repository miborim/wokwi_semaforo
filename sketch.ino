#include <WiFi.h>
#include <HTTPClient.h>

#define led_verde 2 // Pino utilizado para controle do led verda
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led amarelo

const int botao = 18;  // Pino utilizado para controle do botão
int botaoStatus = 0;  // Variável utilizada para leitura do botão]
// bool botaoStatus = 0;
bool buttomLastState = 0;
bool sytemState = false;

const int ldr = 4;  // Pino utilizado para controle do LDR
int threshold=600;

// Variáveis utilizadas para suportar debounce na leitura do botão
long lastDebounceTime = 0;
long debounceTime = 50;

bool botaoPullDownStatus = digitalRead(botao);

void setup() {
  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_amarelo, OUTPUT);
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);

  // Inicialização das entradas
  pinMode(botao, INPUT_PULLDOWN);

  digitalWrite(led_amarelo, LOW);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", "", 6); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  // Verifica estado do botão
  botaoStatus = digitalRead(botao);
  if (botaoStatus == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi Desconectado");
  }
}

void loop() {
  buttomLastState = botaoStatus;
  botaoStatus = digitalRead(botao);
  // Essas constantes devem corresponder aos atributos "gama" e "rl10" do fotoresistor
  const float GAMMA = 0.7;
  const float RL10 = 50;

  if(botaoStatus == 0 && buttomLastState == 1){
    sytemState = !sytemState;
  }

  Serial.println(botaoStatus);

  // Converte o valor analógico em valor lux:
  int analogValue = analogRead(ldr);
  Serial.println("Analog Value: " + String(analogValue));
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  Serial.println("Lux Value: " + String(lux));

  int ldrstatus=analogRead(ldr);

  if(sytemState){
    delay(1000);
    Serial.println("Botão Apertado");
    
    if(ldrstatus<=threshold){
      Serial.print("Está escuro, ligar luz");
      Serial.println(ldrstatus);
      digitalWrite(led_amarelo, HIGH);

    }else{
      Serial.print("Está claro, desligar luz");
      Serial.println(ldrstatus);
    
      digitalWrite(led_verde, HIGH);
      delay(3000);

      digitalWrite(led_verde, LOW);
      digitalWrite(led_amarelo, HIGH);
      delay(500);

      digitalWrite(led_amarelo, LOW);
      digitalWrite(led_vermelho, HIGH);
      delay(2000);

      digitalWrite(led_amarelo, HIGH);
      delay(500);
      digitalWrite(led_amarelo, LOW);
      digitalWrite(led_vermelho, LOW);
    }

  } else{
    if(ldrstatus<=threshold){
      Serial.print("Está escuro, ligar luz");
      Serial.println(ldrstatus);
      digitalWrite(led_amarelo, HIGH);

    }else{
      Serial.print("Está claro, desligar luz");
      Serial.println(ldrstatus);
    
    digitalWrite(led_verde, HIGH);
    delay(3000);

      digitalWrite(led_verde, LOW);
      digitalWrite(led_amarelo, HIGH);
      delay(500);

      digitalWrite(led_amarelo, LOW);
      digitalWrite(led_vermelho, HIGH);
      delay(2000);

      digitalWrite(led_amarelo, HIGH);
      delay(500);
      digitalWrite(led_amarelo, LOW);
      digitalWrite(led_vermelho, LOW);
    } 
  }

  delay(10);
}
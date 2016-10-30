#include "ESP8266WiFi.h"
#include <SPI.h>
#include <SmartConnect.h>
#include <stdlib.h>
#include <string.h>

SmartConnect SMART;
WiFiClient *_client;

const char* wifiSSID     = "SSID";
const char* wifiPassword = "PASSWORD";

// smart vars
char server[] = "HOSTNAME";              //Endereco Smartapps.
char login[] = "USER_KEY";   //Login do usuario: Encontra-se na plataforma.
char password[] = "CONNECT_KEY";            //Senha do usuário: Encontra-se na plataforma.
char app[] = "APP_NAME";                             //Nome do aplicativo utilizado.
char schema[] = "SCHEMA";  //Login do aplicativo: Normalmente o mesmo do usuário, só muda se for utilizado um aplicativo que foi compartilhado por outro usuário, neste caso é o login do outro usuário.

//
String postDataVar1 = "variavel=1&valor=";

String responseData;

long randNumber;

void config_conn(){

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifiSSID);

  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void setup() {

  //Serial.begin(9600);
  config_conn();

  // put your setup code here, to run once:
  SMART.begin(115200, false);

  // connect on smartapps platform
  Serial.println("Conectando...");

  if (SMART.connect(server, login, password, 80)) {
    Serial.println("Dispositivo autenticado.");
  } else {
    Serial.println("Nao autenticado.");
  }

}

void loop() {

  // put your main code here, to run repeatedly:
  if (SMART.connected()) {

    Serial.println("Enviando dados dados...");

    randNumber = random(30);

    String stringPostData = "";
    stringPostData.concat(postDataVar1);
    stringPostData.concat(randNumber);

    char arrayPostData[20];
    stringPostData.toCharArray(arrayPostData, 20);

    responseData = SMART.exec("POST","csv", app, schema, "variaveis_valores/insert", arrayPostData);
    Serial.println(responseData);

  } else {

    Serial.println("Dispositivo desconectado!");

  }

  delay(1000);

}




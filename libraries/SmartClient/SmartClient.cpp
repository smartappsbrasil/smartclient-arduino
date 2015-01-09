/*

SMARTClient, a Smartapps client for Arduino

@author         Fabio Costa Mangia <fabio.costa@smartapps.com.br>
@contribution   Prof. Rodrigo M A Almeida <rodrigomax@unifei.edu.br>
@contribution   José Wilker <jose.wilker@smartapps.com.br>

Smartapps (http://www.smartapps.com.br) - Copyright 2013
*/

#include "SmartClient.h"
#include "Base642.h"
#include <string.h>
#include <stdio.h>

bool SmartClient::connect(char hostname[], char login[], char pass[], int port) {
    bool result = true;

  #ifdef DEBUGPRINT
      Serial.println(F("Smart Connecting"));
   #endif

    LOGIN = login;
    encrypt(login, pass);
    if (_client.connect(hostname, port)) {
        sendHandshake(hostname, AuthID);
        result = readHandshake();
    }
    return result;
}


bool SmartClient::connected() {
    return _client.connected();
}

String SmartClient::close(char hostname[], char login[], char pass[], int port) {
    String result = "";
    LOGIN = login;
    encrypt(login, pass);
    if (_client.connect(hostname, port)) {
        _client.println(F("GET /api/fp/close/ HTTP/1.1"));
        _client.print(F("Host: "));
        _client.println(hostname);
        _client.print(F("Authorization: Basic "));
        _client.println(AuthID);
        _client.print(F("Cookie: PHPSESSID="));
        _client.println(PHPSESSID);
        _client.println();
    }


}


void SmartClient::sendSchema(char hostname[], char AuthID[]){
        _client.println(F("GET /api/fp/from/controls/_schemas  HTTP/1.1"));
        _client.print(F("Host: "));
        _client.println(hostname);
        _client.println("User-Agent: arduino-ethernet");
        _client.print(F("Authorization: Basic "));
        _client.println(AuthID);
        _client.print(F("Cookie: PHPSESSID="));
        _client.println(PHPSESSID);
        _client.println();
}


char* SmartClient::getSchema(char hostname[], char login[], char pass[], int port){
    char* result;
    LOGIN = login;
    encrypt(login, pass);
    if (_client.connect(hostname, port)) {
        sendSchema(hostname, AuthID);
       result = readSchema();
     //  Serial.println("Schema : ");
      // Serial.print(result);
    }
    return  result;
}

char* SmartClient::readSchema() {
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    int n;
    int i;
    char *retorno;
    char* result;
    char* resultJson;
    while(_client.available() == 0 && attempts < maxAttempts) {
        delay(100);
        attempts++;
    }
    while(_client.available()) {
       readLine();
        if(posBuffer = 146){
           retorno = readBuffer;
        }
    }
    resultJson = strstr(retorno,"[\"");
    resultJson = strtok(resultJson,"[\"");
   _client.stop();
   _client.flush();
    return resultJson;
}


void SmartClient::readResponse2() {
    char character;
    buff = "";
    #ifdef NEW
    posBuffer = 0;
    #endif
    while(_client.available() > 0 && (character = _client.read()) != '\n') {
       // if (character != '\r' && character != -1) {
            #ifndef NEW
            buff += character;
            #endif
            #ifdef NEW
            readBuffer[posBuffer] = character;
            posBuffer++;
            #endif
      // }
    }
    #ifdef NEW
        readBuffer[posBuffer]= '\0';
    #endif
}

void SmartClient::encrypt(char login[], char pass[]){
    //maximo no teste foi 70
    char loginPass[70]; //variavel para ser concatenada em login:pass
    strcpy(loginPass,login);
    strcat(loginPass,":");
    strcat(loginPass,pass);
    base64_encode(AuthID,loginPass,strlen(loginPass));
}
void SmartClient::sendHandshake(char hostname[], char AuthID[]) {
    int time = millis();
    _client.println(F("GET /api/fp/from HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(hostname);
    _client.print(F("Authorization: Basic "));
    _client.println(AuthID);
    _client.println();

    #ifdef DEBUGPRINT
        Serial.print(F("Dados enviados em [ms]: "));
        Serial.println(millis() - time);

        Serial.println(F("GET /api/fp/from HTTP/1.1"));
        Serial.print(F("Host: "));
        Serial.println(hostname);
        Serial.print(F("Authorization: Basic "));
        Serial.println(AuthID);
    #endif
}

bool SmartClient::readHandshake() {
    bool result = false;
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    int n;
    int i;
    //String handshake, line;

    #ifdef DEBUGPRINT
        Serial.println(F("Smart readHandshake"));
    #endif

    while(_client.available() == 0 && attempts < maxAttempts) {
        delay(100);
        attempts++;
    }

    #ifdef DEBUGPRINT
        Serial.println(F("Conectado"));
    #endif


   //procura se a resposta HTTP eh 200
    while(_client.available()) {
        readLine();

        #ifdef NEW
        n = -1;
        //-8 do HTTP/1.1 e -3 do 200
        for(i=0;i<posBuffer-11;i++){
            //procura o header
            if( (readBuffer[i] == 'H') &&
                (readBuffer[i+1] == 'T') &&
                (readBuffer[i+2] == 'T') &&
                (readBuffer[i+3] == 'P') &&
                (readBuffer[i+4] == '/') &&
                (readBuffer[i+5] == '1') &&
                (readBuffer[i+6] == '.') &&
                (readBuffer[i+7] == '1') &&
                (readBuffer[i+8] == ' ')
              ){
                //verifica se a resposta eh 200
                if( (readBuffer[i+9] == '2') &&
                   (readBuffer[i+10] == '0') &&
                   (readBuffer[i+11] == '0')
                 ){
                   //encontrou
                   n = i;
                    #ifdef DEBUGPRINT
                    Serial.println(F("Resposta HTTP OK"));
                    #endif
                   break;
                }else{
                  //deu erro na msg
                  _client.flush();
                  _client.stop();
                  return false;
                }
            }
        }
        if(n != -1){
                break;
        }
        #endif

        #ifndef NEW
        n = buff.indexOf("HTTP/1.1");
        if(n != -1){
            if ((buff.charAt(n+9)== '2')&&
                (buff.charAt(n+10)== '0')&&
                (buff.charAt(n+11)== '0')){
                break;
            }else{
                _client.flush();
                _client.stop();
                return false;
            }
        }
        #endif
    }
    #ifdef DEBUGPRINT
    Serial.println(F("Procurando PHPSESSID"));
    #endif
    //procura PHP session ID
    while(_client.available()) {
        readLine();

        #ifdef NEW
        n = -1;
        //-9 do PHPSSID e -20 do pass
        for(i=0;i<posBuffer-(9);i++){
            //procura o header
            if( (readBuffer[i+0] == 'P') &&
                (readBuffer[i+1] == 'H') &&
                (readBuffer[i+2] == 'P') &&
                (readBuffer[i+3] == 'S') &&
                (readBuffer[i+4] == 'E') &&
                (readBuffer[i+5] == 'S') &&
                (readBuffer[i+6] == 'S') &&
                (readBuffer[i+7] == 'I') &&
                (readBuffer[i+8] == 'D') &&
                (readBuffer[i+9] == '=')
              ){
                //encontrou, agora eh copiar o cookie
                //for (n=0; n<32; n++){
                for (n=i+10; readBuffer[n] != ';'; n++){
                  PHPSESSID[n-i-10]= readBuffer[n];
                }
               // Session("PHPSESSID") = PHPSESSID;
                #ifdef DEBUGPRINT
                Serial.print(F("PHPSESSID encontrado: "));
                Serial.println(PHPSESSID);
                //Session("PHPSESSID") = PHPSESSID;

                #endif
                break;
            }
        }
        if(n != -1){
            break;
        }
        #endif

        #ifndef NEW
        n = buff.indexOf("PHPSESSID=");
        if(n != -1){
            //OK. Le caracter por caracter ate encontrar  o ponto-virgula.
            for (i=n+10; buff.charAt(i) !=';'; i++){
                PHPSESSID[i-n-10]= buff.charAt(i);
            }
            break;
        }
        #endif
    }
    #ifdef DEBUGPRINT
    Serial.println(F("Verificando Sucessfull"));
    #endif

    //procura se conexao foi "sucessfull"
    while(_client.available()) {
        readLine();


        #ifdef NEW
        result = -1;
        //-10 do Successfull
        for(i=0;i<posBuffer-10;i++){
            //procura o header
            if( (readBuffer[i] == 'S') &&
                (readBuffer[i+1] == 'u') &&
                (readBuffer[i+2] == 'c') &&
                (readBuffer[i+3] == 'c') &&
                (readBuffer[i+4] == 'e') &&
                (readBuffer[i+5] == 's') &&
                (readBuffer[i+6] == 's') &&
                (readBuffer[i+7] == 'f') &&
                (readBuffer[i+8] == 'u') &&
                (readBuffer[i+9] == 'l') &&
                (readBuffer[i+10] == 'l')
              ){
                //encontrou, sai do for
                result = true;
                break;
            }
        }
        #endif

        #ifndef NEW
        result = buff.indexOf("Successfull") != -1;
        #endif

        if(result){
          break;
        }
    }


    #ifdef DEBUGPRINT
        Serial.print(F("Dados recebidos em [ms]: "));
        Serial.println(millis() - time);
        Serial.println(result);

        if(!result) {
           Serial.println(F("Authentication Failed!"));
        }
        Serial.println(PHPSESSID);
    #endif
        Serial.flush();
        _client.stop();
        _client.flush();
    return result;

}


String SmartClient::readResponse() {
    //bool result = false;
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    //TODO remove line
    String handshake = "", line;
#ifdef NEW
    char character;
#endif
    #ifdef DEBUGPRINT
        Serial.println(F("Smart readResponse 2"));
    #endif

    while(_client.available() == 0 && attempts < maxAttempts){
        delay(100);
        attempts++;
    }

    while(_client.available()) {
        readLine();
        handshake += readBuffer + '\n';
    }
   //erial.println(readBuffer);
   
}




void SmartClient::readLine() {
    char character;
    buff = "";
    #ifdef NEW
    posBuffer = 0;
    #endif
    while(_client.available() > 0 && (character = _client.read()) != '\n') {
      //  if (character != '\r' && character != -1) {
            #ifndef NEW
            buff += character;
            #endif
            #ifdef NEW
            readBuffer[posBuffer] = character;
            posBuffer++;
            //if(posBuffer>1000) posBuffer = 0;
            #endif
      //  }
    }

    #ifdef NEW
        readBuffer[posBuffer]= '\0';
    
    #endif
}
void SmartClient::send(char hostname[], char app[], char schema[], char caminho[], int variavel,int valor) {
   //DADOS A SEREM ENVIADOS!
    //_client.flush();
    String retorno;
    int i;
    
   if (_client.connect(hostname, 80)) {

    #ifdef DEBUGPRINT
        Serial.println(F("Sending Data POST"));
    #endif

    _client.print(F("POST /api/fp/exec/json/"));
    _client.print(app);
    _client.print(F("/"));
    _client.print(schema);
    _client.print(F("/"));
    _client.print(caminho);
    _client.println(F("/ HTTP/1.1"));
    _client.println(F("Host: www.smartapps.com.br"));
    _client.print(F("Authorization: Basic "));
    _client.println(AuthID);
    _client.print(F("Cookie: PHPSESSID="));
    _client.println(PHPSESSID);
    _client.println(F("Content-Type: application/x-www-form-urlencoded"));
    _client.print(F("Content-Length: "));
    _client.println(30);
    _client.println();
    _client.print("variavel=");
    _client.print(variavel);
    _client.print("&valor=");    
    _client.print(valor);
    _client.println();
    _client.stop();
     }
    
}




void SmartClient::sendLastRegister(char hostname[], char AuthID[],char *api, char *from, char *schema, char *form, char *qtd,int field){
        _client.print(F("GET /api/fp/from/"));
        _client.print(api);
        _client.print("/");
        _client.print(schema);
        _client.print("/");
        _client.print(form);
        _client.print(F("/_last/"));
        _client.print(qtd);
        _client.println(" HTTP/1.1");        
        _client.print("Host: ");
        _client.println(hostname);
        _client.println("User-Agent: arduino-ethernet");
        _client.print(F("Authorization: Basic "));
        _client.println(AuthID);
        _client.print(F("Cookie: PHPSESSID="));
        _client.println(PHPSESSID);
        _client.println();
}


char* SmartClient::getLastRegister(char hostname[], char login[], char pass[], char *api, char *from, char *schema, char *form, char *qtd,int field){
    char* result;
    LOGIN = login;
    encrypt(login, pass);
    if (_client.connect(hostname, 80)) {
        sendLastRegister(hostname, AuthID,api,from,schema,form,qtd,field);
       result = readLastRegister(field);
    }
    return  result;
}

char* SmartClient::readLastRegister(int field) {
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    int n;
    int i;
    char *retorno;
    while(_client.available() == 0 && attempts < maxAttempts) {
        delay(100);
        attempts++;
    }
    while(_client.available()) {
        readLine();
        if(posBuffer > 110){
            retorno = readBuffer;
        }
    }
    char* resultJson;
    resultJson = strstr(retorno,"variavel\"");
    char *pch;
    char *values[5];
    char *value;
    char *dateTime;
    char *subDateTime;
    i = 0;
    pch = strtok(resultJson,",");
    while(pch != NULL && pch != "\""){
        values[i] = pch;
        i++;
        pch = strtok (NULL, ",");
    }

    switch (field){
        case 1:
            value = strtok(values[0],"\"variavel\":");    
        break;
        case 2:
            value = strtok(values[1],"\"valor\":");    
        break;
        case 3:
            value = strtok(values[2],"\"latitude\":");    
        break;
        case 4:
            value = strtok(values[3],"\"longitude\":");    
        break;
        case 5:
                value = strtok(values[4],"}");
                value = strchr(value,':');
                value = strchr(value,'0x22');
                value = strtok(value,"\"");
        break;
    }
    _client.stop();
    _client.flush();
    return value;

}

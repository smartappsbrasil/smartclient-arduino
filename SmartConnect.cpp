#include <Arduino.h>
#include <Base642.h>
#include <string.h>
#include <stdio.h>
#include <SPI.h>
#include <stdlib.h>

#include <SmartConnect.h>

#include <ESP8266WiFi.h>

// lib vars
WiFiClient _client;

/**
 * [SmartConnect::begin method to start the smart connect library]
 * @param sp        [serial port]
 * @param smc_debug [active/inative debug mode]
 */
void SmartConnect::begin(int sp, bool smc_debug) {

    wConnectionAvaiable = false;
    wSerialPort = sp;
    Serial.begin(sp);
    SMC_DEBUG = smc_debug;

    memset(wBuffer,'\0',_SC_sizeBuffer);

}

/** -- HELPER FUNCTIONS **/

/**
 * Method to create base64 key for authenticate on platform.
 * @param login [description]
 * @param pass  [description]
 */
void SmartConnect::encrypt(char login[], char pass[]) {

    char loginPass[60];
    strcpy(loginPass,login);
    strcat(loginPass,":");
    strcat(loginPass,pass);
    base64_encode(AuthID,loginPass,strlen(loginPass));

}

/**
 * Method to do a hand shake on _client
 */
void SmartConnect::doHandShake() {
    int maxAttempts = 10, attempts = 0;
    while(_client.available() == 0 && attempts < maxAttempts) {
        delay(100);
        attempts++;
    }
}

/**
 * Method for clear all data on memory.
 */
void SmartConnect::doFlushData() {
    Serial.flush();
    _client.stop();
    memset(wBuffer, 0, sizeof(wBuffer));
}

/**
 * Method to parse a result based on a pattern
 * @param  outStr       [var that receive the result]
 * @param  wData        [array of data to parse]
 * @param  pattern      [array of chars for mount pattern]
 * @param  patternSize  [size of pattern]
 * @param  closeBracket [end of search]
 * @return              [return a value defined for outStr]
 */
char* SmartConnect::parseBasedPattern(char outStr[], char wData[], char pattern[], int patternSize, char closeBracket[]) {

    int wI=0;
    int wX=0;
    int wMC=0;
    int wN=0;
    int rI=0;
    int xi;

    int wDataSize;
    int patternLen = strlen(pattern);

    outStr[0] = '\0';

    if (SMC_DEBUG) {

        Serial.println();
        Serial.print(_SC_patternLength);
        Serial.print(patternSize);
        Serial.println();

        Serial.println(_SC_searchPattern);

    }

    for(xi=0; xi < patternSize; xi++) {
        Serial.print(pattern[xi]);
        if (!pattern[xi]) {
            break;
        }
        delay(10);
    }

    bool match = false;

    wDataSize = strlen(wData);

    for (wI=0; wI < wDataSize; wI++) {

        if (patternSize > 0) {

            if (!match) {

                wMC=0;
                for(wX=0; wX <= patternSize-1; wX++) {
                    if (pattern[wX] == wData[wX+wI]) {
                        wMC++;
                    }
                }

                if (wMC == patternSize) {
                    match = true;
                }

            }

            int dataSizeBuffer;

            if (match) {

                dataSizeBuffer = wDataSize;
                for(wN=wI+wMC; wN < dataSizeBuffer; wN++) {

                    if (wData[wN] == closeBracket[0]) {
                        wN=dataSizeBuffer+1;
                        wI=wDataSize;
                        outStr[rI] = '\0';
                    } else {
                        outStr[rI] = wData[wN];
                    }

                    rI++;

                }

            }

        }

    }

    if (SMC_DEBUG) {

        Serial.println(_SC_searchPatternDone);

        Serial.print(_SC_searchPatternResult);
        Serial.print(outStr);
        Serial.print(_SC_searchPatternResultLength);
        Serial.println(sizeof(outStr));

    }

    delay(1);

    return outStr;

}

/**
 * Method to check if the device is connected.
 * @return [a bool with state of connection. (true:connected/false:disconnected)]
 */
bool SmartConnect::connected() {
    return wConnectionAvaiable;
}

/** -- SMART FUNCTIONS **/

/**
 * Function to connect on S.M.A.R.T
 * @param  hostname [hostname of platform]
 * @param  login    [user key]
 * @param  pass     [connect key]
 * @param  port     [port of hostname]
 * @return          [result of connection. true=connected,false=disconnected]
 */
bool SmartConnect::connect(String hostname, char login[], char pass[], int port) {

    bool result = false;

    hostname.toCharArray(wHostname,21);

    wLogin = login;
    wPass = pass;
    wPort = port;

    encrypt(wLogin, wPass);

    while (!connected()) {

        if (SMC_DEBUG) {
            Serial.println("connectando no host...");
        }

        if (connectOnHostname(wPort)) {

            if (SMC_DEBUG) {
                Serial.println("criando envelope");
            }

            envelopeRequest("GET", "from", "", "", "", "", AuthID, "");
            result = readEnvelopeConnect();

            doFlushData(); // new added

        }

    }

    return result;

}

/**
 * Public method to get any data from an application
 * @param  app    [application name]
 * @param  schema [key of schema]
 * @param  path   [path of execution. can be the form name]
 * @return        [content of request]
 */
String SmartConnect::from(char* app, char* schema, char* path) {

    if (connected()) {

        if (connectOnHostname()) {

            if (SMC_DEBUG) {

                Serial.println("Executando: From");

                Serial.println("Enviando envelope ... ");

                Serial.println(app);
                Serial.println(schema);
                Serial.println(path);

            }

            envelopeRequest("GET", "from", app, schema, path, "", AuthID, wSessionId);

            delay(1);

            readEnvelopeResponse(SMC_DEBUG, true);

            delay(1);

            if (SMC_DEBUG) {

                Serial.println("resposta adicionada ao buffer...");
                Serial.println(wBuffer);

            }

            return wBuffer;

        }

    }

}

/**
 * Public method to send data for an application
 * @param   app          [application name]
 * @param   schema       [key of schema]
 * @param   path         [path of execution. can be the form name]
 * @param   stringData   [data vars to send. ex: name=value]
 * @return               [content of request]
 */
String SmartConnect::to(char* app, char* schema, char* path, char* stringData, bool readResponse) {

    if (connected()) {

        if (connectOnHostname()) {

            if (SMC_DEBUG) {

                Serial.println("Executando: To");

                Serial.println("Enviando envelope ... ");

                Serial.println(app);
                Serial.println(schema);
                Serial.println(path);
                Serial.println(stringData);

            }

            envelopeRequest("POST", "to", app, schema, path, stringData, AuthID, wSessionId);

            if (readResponse) {
                delay(1);
                readEnvelopeResponse(SMC_DEBUG, true);
            }

            delay(1);

            if (SMC_DEBUG) {
                Serial.println("resposta adicionada ao buffer...");
                Serial.println(wBuffer);
            }

            if (readResponse) {
                return wBuffer;
            } else {
                return "1";
            }

        }

    }

}

/**
 * Public method for exec a method in the core of an app.
 * @param  requestType [type of request. (POST, GET)]
 * @param  wreturn     [return of request. (JSON/CSV)]
 * @param  app         [application name]
 * @param  schema      [key of schema]
 * @param  path        [path of execution. can be the form name]
 * @param  stringData  [data vars to send. ex: name=value]
 * @return             [description]
 * @return             [content of request]
 */
String SmartConnect::exec(char* requestType, char* wreturn, char* app, char* schema, char* path, char* stringData, bool readResponse) {

    if (connected()) {

        if (connectOnHostname()) {

            String rtype = "exec/";
            rtype.concat(wreturn);

            char arrayCharReqType[10];
            rtype.toCharArray(arrayCharReqType,10);

            if (SMC_DEBUG) {

                Serial.println("enviando envelope ... ");

                Serial.println(rtype);
                Serial.println(app);
                Serial.println(schema);
                Serial.println(path);
                Serial.println(stringData);
                Serial.println(AuthID);
                Serial.println(wSessionId);

            }

            envelopeRequest(requestType, arrayCharReqType, app, schema, path, stringData, AuthID, wSessionId);

            if (readResponse) {
                delay(1);
                readEnvelopeResponse(SMC_DEBUG, true);
            }

            delay(1);

            if (SMC_DEBUG) {
                Serial.println("resposta adicionada ao buffer...");
                Serial.println(wBuffer);
            }

            doFlushData();

            if (readResponse) {
                return wBuffer;
            } else {
                return "1";
            }

        }

    }

}

/**
 * Private method to send data using an action
 * @param  rtype      [type of action with return. (ex: exec/json)]
 * @param  app        [application name]
 * @param  schema     [key of schema]
 * @param  path       [path of execution. can be the form name]
 * @param  stringData [data vars to send. ex: name=value]
 * @return            [content of request]
 */
String SmartConnect::send(char* rtype, char* app, char* schema, char* path, char* stringData) {

    if (connected()) {

        if (SMC_DEBUG) {
            Serial.println("Dispositivo logado");
            Serial.println("Enviando dados...");
        }

        if (connectOnHostname()) {

            if (SMC_DEBUG) {

                Serial.println("enviando envelope ... ");

                Serial.println(rtype);
                Serial.println(app);
                Serial.println(schema);
                Serial.println(path);
                Serial.println(stringData);
                Serial.println(AuthID);
                Serial.println(wSessionId);

            }

            envelopeRequest("POST", rtype, app, schema, path, stringData, AuthID, wSessionId);

            delay(1);

            readEnvelopeResponse(SMC_DEBUG, true);

            delay(1);

            if (SMC_DEBUG) {
                Serial.println("resposta adicionada ao buffer...");
                Serial.println(wBuffer);
            }

            doFlushData();

            return wBuffer;

        }

    }

}

/**
 * Private method to connect in a hostname
 * @param  wPort [port of hostname]
 * @return       [return true after connect in the hostname]
 */
bool SmartConnect::connectOnHostname(int wPort) {

     while (!_client.connected()) {

        if (SMC_DEBUG) {
            Serial.println(F("Tentando conectar..."));
            Serial.println(wHostname);
        }

        _client.connect(wHostname, wPort);

    }

    return true;

}

/**
 * Private method to build the envelope of a request.
 * @param  rtype      [type of request. (GET,POST)]
 * @param  rpath      [full path of action]
 * @param  app        [application name]
 * @param  schema     [key of schema]
 * @param  path       [path of execution. can be the form name]
 * @param  stringData [data vars to send. ex: name=value]
 * @param  AuthID     [Authorization ID]
 * @param  PHPSESSID  [Session ID]
 */
void SmartConnect::envelopeRequest(char* rtype,char* rpath,char* app,char* schema,char* path,char* stringData,char* AuthID,char* PHPSESSID) {

    //memset(wBuffer, 0, sizeof(wBuffer));

    if (SMC_DEBUG) {
        Serial.println(AuthID);
        Serial.println(PHPSESSID);
        Serial.println(wSessionId);
        Serial.println(F("Montando envelope..."));
    }

    _client.print(rtype);
    _client.print(F(" /api/fp/"));
    _client.print(rpath);

    if (SMC_DEBUG) {
        Serial.print(rtype);
        Serial.print(F(" /api/fp/"));
        Serial.print(rpath);
    }

    if (app != "") {

        _client.print(F("/"));
        _client.print(app);

        if (SMC_DEBUG) {
            Serial.print(F("/"));
            Serial.print(app);
        }

    }

    if (schema != "") {

        _client.print(F("/"));
        _client.print(schema);

        if (SMC_DEBUG) {
            Serial.print(F("/"));
            Serial.print(schema);
        }

    }

    if (path != "") {

        _client.print(F("/"));
        _client.print(path);

        if (SMC_DEBUG) {
            Serial.print(F("/"));
            Serial.print(path);
        }

    }

    if (rtype == "GET") {

        _client.println(F(" HTTP/1.0"));

        if (SMC_DEBUG) {
            Serial.println(F(" HTTP/1.0"));
        }

    } else {

        _client.println(F(" HTTP/1.1"));

        _client.print(F("Host: "));
        _client.println(F("www.smartapps.com.br"));
        _client.println(F("User-Agent: arduino-ethernet"));

        if (SMC_DEBUG) {
            Serial.println(F(" HTTP/1.1"));

            Serial.print(F("Host: "));
            Serial.println(F("www.smartapps.com.br"));
            Serial.println(F("User-Agent: arduino-ethernet"));
        }

    }

    if (AuthID[0] != '\0') {

        _client.print(F("Authorization: Basic "));
        _client.println(AuthID);

        if (SMC_DEBUG) {
            Serial.print(F("Authorization: Basic "));
            Serial.println(AuthID);
        }

    }

    if (PHPSESSID[0] != '\0') {

        _client.print(F("Cookie: PHPSESSID="));
        _client.println(PHPSESSID);

        if (SMC_DEBUG) {
            Serial.print(F("Cookie: PHPSESSID="));
            Serial.println(PHPSESSID);
        }

    }

    _client.println(F("Connection: close"));

    if (SMC_DEBUG) {
        Serial.println(F("Connection: close"));
    }

    if (rtype == "POST") {

        _client.println(F("Content-Type: application/x-www-form-urlencoded"));
        _client.print(F("Content-Length: "));
        _client.println(20);
        _client.println();
        _client.println(stringData);

        if (SMC_DEBUG) {
            Serial.println(F("Content-Type: application/x-www-form-urlencoded"));
            Serial.print(F("Content-Length: "));
            Serial.println(20);
            Serial.println();
            Serial.println(stringData);
        }

    } else {
        _client.println();
    }

    if (SMC_DEBUG) {
        Serial.println();
        Serial.println("Envelope enviado.");
    }

    delay(100);

}

/**
 * Private method to read a envelope response
 * @param d              [active/inactive debug. ex: true, false]
 * @param excludeHeaders [option for exclude headers. ps: it's increase time reading the response.]
 * @param bufferSize     [size of the buffer to save response]
 */
void SmartConnect::readEnvelopeResponse(bool d, bool excludeHeaders) {

    //char wBuffer[bufferSize];

    doHandShake();

    if (d) {
        Serial.println("lendo resposta ... ");
    }

    int maxAttempts = 9999, attempts = 0, timeout = 0;

    while(_client.available() == 0 && attempts < maxAttempts){
        delay(100);
        attempts++;
    }

    if (d) {
        Serial.println(attempts);
    }

    if (attempts == maxAttempts) {
        if (d) { Serial.println("Timeout _client when read response."); }
        timeout=1;
    }

    if (timeout == 0) {

        char character;
        int posBuffer=0;
        int posResponseContent=0;
        int charBuffer=0;

        if (excludeHeaders) {

            bool first=false;
            int lastChar[4];
            int lastCharInt=0;
            int hexNumber;

            if (d) {
                Serial.println(_client.available());
            }

            while(_client.available() > 0) {

                character = _client.read();

                int hexNumber = character;

                if (d) {
                    Serial.println(character);
                    Serial.println(hexNumber);
                }

                if (hexNumber >= 0) {
                    lastChar[lastCharInt]=character;
                }

                if (
                    hexNumber == 10 &&
                    (
                        lastChar[0] == 10 &&
                        lastChar[1] == 13 &&
                        lastChar[2] == 10 &&
                        lastChar[3] == 13
                    ) || (
                        lastChar[0] == 13 &&
                        lastChar[1] == 10 &&
                        lastChar[2] == 13 &&
                        lastChar[3] == 10
                    )
                ) {
                    posResponseContent = 1;
                }

                if (posResponseContent == 1 && first) {
                    wBuffer[charBuffer] = character;
                    charBuffer++;
                } else if (posResponseContent == 1) {
                    first=true;
                }

                if (lastCharInt == 3) {
                    lastCharInt=0;
                } else {
                    lastCharInt++;
                }

            }

            if (d) {
                Serial.println("BUFFER EH:");
                Serial.println(wBuffer);
            }

        } else {

            while(_client.available() > 0) {

                character = _client.read();

                if (d) {
                    Serial.println(character);
                }

                if (character != '\n') {
                    wBuffer[charBuffer] = character;
                } else {
                    wBuffer[charBuffer] = '\n';
                }

                charBuffer++;

                posBuffer++;

            }

            if (d) {
                Serial.println("BUFFER BASIC:");
                Serial.println(wBuffer);
            }

        }

    }

}

/**
 * Private method to read and set session on id for future interaction
 */
void SmartConnect::getAndSetSessionId() {

    if (SMC_DEBUG) {
        Serial.println(F("Procurando PHPSESSID"));
    }

    delay(10);

    // process sessid
    char wPattern[6] = {'"','i','d','"',':','"'}; // "id":"
    char wCloseBracket[1] = {'"'}; // "

    parseBasedPattern(wSessionId, wBuffer, wPattern, 6, wCloseBracket);
    if (SMC_DEBUG) {
        Serial.println(wSessionId);
    }

    if (wSessionId) {

        if (SMC_DEBUG) {
            Serial.print(F("PHPSESSID ID: "));
            Serial.println(wSessionId);
        }

        // process sessid name
        char wPattern[8] = {'"','n','a','m', 'e','"',':','"'}; // "name":"
        char wCloseBracket[1] = {'"'}; // "

        parseBasedPattern(wSessionName, wBuffer, wPattern, 8, wCloseBracket);

        if (SMC_DEBUG) {
            Serial.print(F("PHPSESSID NAME: "));
            Serial.println(wSessionName);
        }

    }

}

/**
 * Method to read envelope called on 'connect' method.
 * @return [boolean that show if connection is available]
 */
bool SmartConnect::readEnvelopeConnect() {

    if (SMC_DEBUG) {
        Serial.print(F("hostnamec: "));
        Serial.println(wHostname);
    }

    int time = millis();

    doHandShake();

    //procura se a resposta HTTP eh 200
    readEnvelopeResponse(SMC_DEBUG, false);
    delay(1);
    getAndSetSessionId();
    delay(1);

    if (wSessionId[0] != '\0') {

        if (SMC_DEBUG) {
            Serial.println("Autenticado");
        }

        wConnectionAvaiable = true;

    } else {

        wConnectionAvaiable = false;

    }

    doFlushData();

    return wConnectionAvaiable;

}
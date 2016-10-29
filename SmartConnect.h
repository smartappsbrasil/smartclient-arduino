
/**
 * Library to connect on SMARTAPPS Plataform
 * @author José Wilker <jose.wilker@smartapps.com.br>
 */

class SmartConnect {

    public:

        void begin(int sp, bool debug);

        bool connect(String hostname, char login[], char pass[], int port = 80);
        bool connected();

        String to(char* app, char* schema, char* path, char* stringData);
        String from(char* app, char* schema, char* path);
        String exec(char* requestType, char* wreturn, char* app, char* schema, char* path, char* stringData = "");

    private:

        String send(char* rtype, char* app, char* schema, char* path, char* stringData);

        // functions
        void envelopeRequest(char* rtype,char* rpath,char* app,char* schema,char* path,char* stringData,char* AuthID,char* PHPSESSID);

        void encrypt(char login[], char pass[]);
        void readEnvelopeResponse(bool d, bool excludeHeaders, int bufferSize = 1280);

        void getAndSetSessionId();
        void doHandShake();
        void doFlushData();

        bool connectOnHostname(int port=80);
        bool readEnvelopeConnect();
        bool checkIfResponseIs200();

        char* parseBasedPattern(char * outStr, char * wData, char * pattern, int patternSize, char * closeBracket);

        // vars
        bool SMC_DEBUG;

        char wBuffer[1280];

        char AuthID[70];

        char* wLogin;
        char* wPass;
        char wHostname[21];
        //char* wRecordSet;

        char wSessionId[33];
        char wSessionName[11];

        int wPort;
        int wConnectionAvaiable;
        int wSerialPort;

        // lib vars
        WiFiClient _client;

};
/**
 * Library for connect on SMARTAPPS Plataform
 * @author José Wilker <jose.wilker@smartapps.com.br>
 */

// ---- msg def ---- //
#define _SC_patternLength "Pattern length: "
#define _SC_searchPattern "Procurando padrao: "
#define _SC_searchPatternDone "Procurando padrao... Done"
#define _SC_searchPatternResult "Resultado encontrado: "
#define _SC_searchPatternResultLength " Tamanho: "

// ---- global vars ---- //
#define _SC_sizeBuffer 1024

class SmartConnect {

    public:

        void begin(int sp, bool debug);

        bool connect(String hostname, char login[], char pass[], int port = 80);
        bool connected();

        String to(char* app, char* schema, char* path, char* stringData, bool readResponse=false);
        String from(char* app, char* schema, char* path);
        String exec(char* requestType, char* wreturn, char* app, char* schema, char* path, char* stringData = "", bool readResponse=false);

    private:

        String send(char* rtype, char* app, char* schema, char* path, char* stringData);

        // functions
        void envelopeRequest(char* rtype,char* rpath,char* app,char* schema,char* path,char* stringData,char* AuthID,char* PHPSESSID);

        void encrypt(char login[], char pass[]);
        void readEnvelopeResponse(bool d, bool excludeHeaders);

        void getAndSetSessionId();
        void doHandShake();
        void doFlushData();

        bool connectOnHostname(int port=80);
        bool readEnvelopeConnect();
        bool checkIfResponseIs200();

        char* parseBasedPattern(char * outStr, char * wData, char * pattern, int patternSize, char * closeBracket);

        // vars
        bool SMC_DEBUG;

        char wBuffer[_SC_sizeBuffer];

        char AuthID[70];

        char* wLogin;
        char* wPass;
        char wHostname[21];

        char wSessionId[33];
        char wSessionName[11];

        int wPort;
        int wConnectionAvaiable;
        int wSerialPort;

};


//Smartapps Libs:
#include <SmartClient.h>

//Ethernet Libs:
#include <SPI.h>
#include <Ethernet.h>

//DHT Lib:
#include <dht.h>
dht DHT;
#define DHT22_PIN 8

//Definicao endereco MAC do Ethernet Shield:
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };

//Definicoes do usuário na Smartapps:
char server[] = "www.smartapps.com.br";              //Endereco Smartapps.
char login[] = "SUA CHAVE";   //Login do usuario: Encontra-se na plataforma.
char password[] = "SUA SENHA";            //Senha do usuário: Encontra-se na plataforma.
char app[] = "controls";                             //Nome do aplicativo utilizado.
char schema[] = "SCHEMA";  //Login do aplicativo: Normalmente o mesmo do usuário, só muda se for utilizado um aplicativo que foi compartilhado por outro usuário, neste caso é o login do outro usuário.
char caminhoPost[] = "variaveis_valores";
SmartClient smart; 

//Variaveis do Exemplo 2;
String PostData = "";

//Variaveis usadas pela funcao get_free_memory(). Muito util, principalmente com o Arduino Uno.
extern int __bss_end;                                
extern void *__brkval;                               

void setup() {
  Serial.begin(9600);
  Serial.print("Free Memory: ");
  Serial.println(get_free_memory());
  Serial.println("Starting Ethernet");
  config_rede();

//Conexao a plataforma, 5 tentativas:  
  int attempts = 0;
  while(smart.connect(server, login, password) == 0){
    delay(500);
    attempts++;
    if(attempts > 5){
      while(1){
          Serial.println("ERRO");       
      }
    }
    smart.disconnect();
  }
  delay(3000);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  //Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop() {
   
    Serial.print("DHT22, \t");
    int chk = DHT.read22(DHT22_PIN);
    switch (chk)
    {
    case DHTLIB_OK: 
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
    } 
    
    // DISPLAY DATA
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.println(DHT.temperature, 1);
      
    char temp[10];
    
    dtostrf(DHT.temperature,1,2,temp);
    PostData = "variavel=X&valor=" + String(temp); 
    smart.send(true, server, app, schema, caminhoPost, PostData); //Envia informacoes para plataforma.
    
    dtostrf(DHT.humidity,1,2,temp);
    PostData = "variavel=X&valor=" + String(temp);
    smart.send(true, server, app, schema, caminhoPost, PostData); //Envia informacoes para plataforma.
    delay(5000); //Informacao é enviada a cada 5 segundos.

}


//Funcao get_free_memory(): 
int get_free_memory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}


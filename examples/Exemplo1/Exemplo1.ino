#include <SmartClient.h>
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };
//Definicoes do usuário na Smartapps:
char server[] = "www.smartapps.com.br";              //Endereco Smartapps.
char login[] = "";   //Login do usuario: Encontra-se na plataforma.
char password[] = "=";            //Senha do usuário: Encontra-se na plataforma.
char app[] = "controls";                             //Nome do aplicativo utilizado.
char schema[] = "";  //Schema do aplicativo: Normalmente o mesmo do usuário, só muda se for utilizado um aplicativo que foi compartilhado por outro usuário, neste caso é o login do outro usuário.
char caminhoPost[] = "variaveis_valores/insert";
char *appSchema;
char *lastRegister;
char* sendData;
int variavel = 11;
int valor;
String tecla;

SmartClient smart; 
String PostData;
extern int __bss_end;                                
extern void *__brkval;   
void setup() {
  Serial.begin(9600);
  config_rede();
  smart.connect(server, login, password);
    delay(2000);
}
void loop() {
  if(Serial.available() > 0 ){
    tecla = Serial.readString();
    Serial.println(Serial.readString());
    if(tecla == "1"){
      valor = int(random(0,100));
      smart.send(server,app, schema, caminhoPost, variavel,valor);
       delay(2000);
    }else if(tecla == "2"){
      appSchema = smart.getSchema(server,login,password);
      Serial.print("Schema: ");
      Serial.println(appSchema);    
    }else{
      lastRegister = smart.getLastRegister(server,login,password,"controls","",schema,"variaveis_valores","1",2);
      Serial.println(lastRegister);
    }
  } 
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





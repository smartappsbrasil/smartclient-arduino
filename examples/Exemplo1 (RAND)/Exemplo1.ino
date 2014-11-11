//Smartapps Libs:
#include <SmartClient.h>
//Ethernet Libs:
#include <SPI.h>
#include <Ethernet.h>
//Definicao endereco MAC do Ethernet Shield:
//byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };
byte mac[] = {  0x3C, 0x07, 0x54, 0x00, 0x2D, 0xCF };
//Definicoes do usuário na Smartapps:
char server[] = "www.smartapps.com.br";              //Endereco Smartapps.
char login[] = "LOGIN";   //Login do usuario: Encontra-se na plataforma.
char password[] = "USER";            //Senha do usuário: Encontra-se na plataforma.
char app[] = "controls";                             //Nome do aplicativo utilizado.
char schema[] = "schema";  //Schema do aplicativo: Normalmente o mesmo do usuário, só muda se for utilizado um aplicativo que foi compartilhado por outro usuário, neste caso é o login do outro usuário.
char caminhoPost[] = "variaveis_valores/insert";
SmartClient smart; 
//Variaveis do Exemplo 1;
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
  smart.connect(server, login, password);
}
void loop() {
    PostData = "variavel=5&datetime=0&valor="+ String(random(0,100));  //Gera valor randomico a cada loop e adiciona a variavel escolhida.
    smart.send(true, server, app, schema, caminhoPost, PostData);    //Envia informacoes para plataforma.
    delay(5000);  //Informacao é enviada a cada 5 segundos.
}
void config_rede(){
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for(;;)
      ;
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

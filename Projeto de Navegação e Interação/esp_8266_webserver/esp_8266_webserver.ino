//Inclui as funÃ§Ãµes de Wifi do ESP
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

//Cria um server na porta 80 (porta padrÃ£o para onde os navegadores enviam as requisiÃ§Ãµes http)
WiFiServer server(80);
  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(14);
int tamanho2 = 67;
int tamanho = 71; //TAMANHO DA LINHA RAW(71 BLOCOS)
int frequencia = 32; //FREQUÊNCIA DO SINAL IR(32KHz)

uint16_t LD[67] = {8990, 4558,  488, 650,  486, 650,  486, 1770,  488, 650,  484, 648,  488, 628,  506, 650,  486, 628,  506, 1772,  508, 1750,  486, 650,  486, 1772,  486, 1770,  486, 1770,  486, 1772,  486, 1752,  506, 630,  506, 628,  528, 628,  486, 1772,  486, 646,  508, 628,  486, 628,  508, 628,  506, 1750,  506, 1750,  506, 1772,  486, 628,  506, 1770,  488, 1770,  508, 1728,  508, 1750,  508};
uint16_t AC[71] = {9014, 4538,  530, 604,  532, 604,  530, 1726,  530, 604,  530, 604,  530, 604,  530, 604,  532, 604,  530, 1726,  530, 1726,  530, 606,  530, 1728,  530, 1726,  532, 1726,  530, 1728,  530, 1726,  530, 604,  530, 604,  530, 606,  530, 606,  530, 604,  532, 604,  530, 604,  530, 606,  530, 1726,  530, 1728,  530, 1728,  530, 1726,  530, 1726,  530, 1728,  530, 1726,  530, 1726,  532, 40074,  8988, 2294,  532};
uint16_t RC[71] = {9014, 4540,  528, 606,  528, 606,  528, 1730,  506, 628,  528, 606,  528, 606,  528, 608,  528, 606,  528, 1728,  506, 1750,  506, 630,  528, 1728,  528, 1730,  528, 1730,  528, 1728,  528, 1728,  528, 1728,  530, 606,  528, 606,  530, 606,  528, 606,  530, 606,  528, 606,  530, 606,  530, 606,  528, 1730,  528, 1730,  528, 1730,  528, 1728,  528, 1728,  530, 1728,  528, 1730,  506, 40058,  8988, 2296,  508};
uint16_t MU[71] = {9046, 4480,  586, 550,  588, 546,  532, 1726,  530, 606,  530, 606,  530, 606,  530, 606,  530, 604,  530, 1728,  584, 1672,  530, 606,  530, 1726,  530, 1728,  530, 1728,  530, 1728,  530, 1726,  588, 1670,  588, 546,  584, 552,  586, 1672,  530, 604,  530, 606,  584, 552,  586, 550,  530, 606,  588, 1670,  586, 1672,  530, 604,  532, 1726,  530, 1728,  588, 1668,  590, 1668,  532, 40078,  8990, 2294,  530};
void setup()
{
  //Inicializa a Serial apenas para efeito de log
  Serial.begin(115200);

  //Configura o GPIO0 e GPIO2 como output, ou seja, como saÃ­da para podermos alterar o seu valor
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(4, OUTPUT);
  //Deixa o GPIO0 e o GPIO2 com saÃ­da LOW
  digitalWrite(4, LOW);
  digitalWrite(14, LOW);
  digitalWrite(0, LOW);
  digitalWrite(2, LOW);

  Serial.print("Conectando");
  //Faz o ESP se conectar Ã  rede WiFi. No nosso exemplo o ssid da rede Ã© TesteESP e a senha Ã© 87654321
  WiFi.begin("Marcos", "52642367");
  //WiFi.begin("Oliveira", "2735ADRM");
  //Enquanto o ESP nÃ£o se conectar Ã  rede
  while (WiFi.status() != WL_CONNECTED)
  {
    //Esperamos 100 milisegundos
    delay(100);
    Serial.print(".");
  }

  //Se chegou aqui Ã© porque conectou Ã  rede, entÃ£o mostramos no monitor serial para termos um feedback
  Serial.println("");
  Serial.println("Conectou");

  //ConfiguraÃ§Ãµes do IP fixo. VocÃª pode alterar conforme a sua rede
  IPAddress ip(192, 168, 0, 15);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  Serial.print("Configurando IP fixo para : ");
  Serial.println(ip);

  //Envia a configuraÃ§Ã£o
  WiFi.config(ip, gateway, subnet);

  //Inicializa o server que criamos na porta 80
  server.begin();

  //Mostramos no monitor serial o IP que o ESP possui para verificarmos se Ã© o mesmo que configuramos
  Serial.print("Server em: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  //Verifica se algum cliente estÃ¡ tentando se conectar
  WiFiClient client = server.available();
  if (!client)
  {
    //Se nÃ£o houver nenhum cliente podemos retornar pois nÃ£o hÃ¡ nada a fazer
    return;
  }

  Serial.println("Novo cliente conectou");

  //Fazemos a leitura da requisiÃ§Ã£o
  String req = client.readStringUntil('\r');
  Serial.print("Requisição: ");
  Serial.println(req);

  //Este Ã© o html que iremos retornar para o cliente
  //Ã‰ composto basicamente de dois botÃµes (ON e OFF) para o GPIO0 e dois botÃµes (ON e OFF) para o GPIO2
  //A parte que nos interessa Ã© o <a href=' com a aÃ§Ã£o vinculada a cada botÃ£o
  //Quando clicamos em um destes botÃµes essa informaÃ§Ã£o chegarÃ¡ atÃ© o ESP para que ele verifique qual aÃ§Ã£o deve executar
  //A parte dentro de '<style>' Ã© apenas para modificarmos o visual da pÃ¡gina que serÃ¡ exibida, vocÃª pode alterÃ¡-la como queira
  String html = 
  "<html>"
    "<head>"
      "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>"
      "<meta charset='utf-8'/>"
      "<title>ESP8266</title>"
      "<style>"
        "body{"
          "text-align: center;"
          "font-family: sans-serif;"
          "font-size:14px;"
          "padding: 25px;"
        "}"

        "p{"
          "color:#444;"
        "}"

        "button{"
          "outline: none;"
          "border: 2px solid #1fa3ec;"
          "border-radius:18px;"
          "background-color:#FFF;"
          "color: #1fa3ec;"
          "padding: 10px 50px;"
        "}"

        "button:active{"
          "color: #fff;"
          "background-color:#1fa3ec;"
        "}"
      "</style>"
    "</head>"
    "<body>"
    "<h1>GPIO0</h1>"
    "<p><a href='?acao=gpio0On'><button>OFF</button></a></p>"
    "<p><a href='?acao=gpio0Off'><button>ON</button></a></p>"
    "<h1>GPIO2</h1>"
    "<p><a href='?acao=gpio2On'><button>OFF</button></a></p>"
    "<p><a href='?acao=gpio2Off'><button>ON</button></a></p>"
    "<h1>GPIO4</h1>"
    "<p><a href='?acao=gpio4On'><button>OFF</button></a></p>"
    "<p><a href='?acao=gpio4Off'><button>ON</button></a></p>"
     "<h1>GPIO14</h1>"
    "<p><a href='?acao=gpio14On'><button>ON/OFF</button></a></p>"
     "<p><a href='?acao=gpio14MT'><button>MUTE</button></a></p>"
     "<p><a href='?acao=gpio14AC'><button>Avança Canal</button></a></p>"
     "<p><a href='?acao=gpio14RC'><button>Retrocede Canal</button></a></p>"
    "</body>"
  "</html>";

  //Escreve o html no buffer que serÃ¡ enviado para o cliente
  client.print(html);
  //Envia os dados do buffer para o cliente
  client.flush();

  //Verifica se a requisiÃ§Ã£o possui a aÃ§Ã£o gpio0On
  if (req.indexOf("acao=gpio0On") != -1)
  {
      //Se possui a aÃ§Ã£o gpio0On colocamos a saÃ­da do GPIO0 como alta
      digitalWrite(0, HIGH);
  }
  //SenÃ£o, verifica se a requisiÃ§Ã£o possui a aÃ§Ã£o gpio0Off
  else if (req.indexOf("acao=gpio0Off") != -1)
  {
    //Se possui a aÃ§Ã£o gpio0Off colocamos a saÃ­da do GPIO0 como baixa
    digitalWrite(0, LOW);
  }
  else if (req.indexOf("acao=gpio4Off") != -1)
  {
    //Se possui a aÃ§Ã£o gpio0Off colocamos a saÃ­da do GPIO0 como baixa
    digitalWrite(4, LOW);
  }
  else if (req.indexOf("acao=gpio4On") != -1)
  {
    //Se possui a aÃ§Ã£o gpio2On colocamos a saÃ­da do GPIO2 como alta
    digitalWrite(4, HIGH);
  }
  //SenÃ£o, verifica se a requisiÃ§Ã£o possui a aÃ§Ã£o gpio2On
  else if (req.indexOf("acao=gpio2On") != -1)
  {
    //Se possui a aÃ§Ã£o gpio2On colocamos a saÃ­da do GPIO2 como alta
    digitalWrite(2, HIGH);
  }
  //SenÃ£o, verifica se a requisiÃ§Ã£o possui a aÃ§Ã£o gpio2Off
  else if (req.indexOf("acao=gpio2Off") != -1)
  {
    //Se possui a aÃ§Ã£o gpio0Off colocamos a saÃ­da do GPIO2 como baixa
    digitalWrite(2, LOW);
  }
   else if (req.indexOf("acao=gpio14On") != -1)
  {
   irsend.sendRaw(LD,tamanho2,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
        Serial.println("Comando enviado: Liga / Desliga");
        delay(50); // TEMPO(EM MILISEGUNDOS) DE INTERVALO ENTRE UM COMANDO E OUTRO
  }
   else if (req.indexOf("acao=gpio14MT") != -1)
  {
   irsend.sendRaw(MU,tamanho,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
        Serial.println("Comando enviado: MUTE");
        delay(50); // TEMPO(EM MILISEGUNDOS) DE INTERVALO ENTRE UM COMANDO E OUTRO
  }
   else if (req.indexOf("acao=gpio14AC") != -1)
  {
   irsend.sendRaw(AC,tamanho,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
        Serial.println("Comando enviado: AVANÇA CANAL");
        delay(50); // TEMPO(EM MILISEGUNDOS) DE INTERVALO ENTRE UM COMANDO E OUTRO
  }
   else if (req.indexOf("acao=gpio14RC") != -1)
  {
   irsend.sendRaw(RC,tamanho,frequencia);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
        Serial.println("Comando enviado: RETROCEDE CANAL");
        delay(50); // TEMPO(EM MILISEGUNDOS) DE INTERVALO ENTRE UM COMANDO E OUTRO                                              
  }

  //Fecha a conexÃ£o com o cliente
  client.stop();
  Serial.println("Cliente desconectado");
}

#include <WiFi.h>
#include <Servo.h>

Servo myservo;  // ciar servo

// GPIO 
static const int servoPin = 13;

// Conectar na internet
const char* ssid     = "GABRIEL";
const char* password = "gabrimari1926";

// Porta internet
WiFiServer server(80);

// Variavel guardar a requisição http
String header;

// valor http get
String valueString = String(5);
int pos1 = 0; 
int pos2 = 0;

// Ajustar horario
unsigned long currentTime = millis();
// Horario antes
unsigned long previousTime = 0; 
// Definir tempo em milissegundos 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  myservo.attach(servoPin);  // selecionar por variável o gpio

  // Conectar wifi pelas variaveis
  Serial.print("Conectando: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Printar IP e WI-FI
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Clientes ativos

  if (client) {                             // IF NOVO CLIENTE ATIVO
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Novo cliente.");          // Printar mensagem
    String currentLine = "";                // String para novo cliente
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop enquanto estiver conectado
      currentTime = millis();
      if (client.available()) {             // IF Ler algum byte do cliente
        char c = client.read();             // Ler byte
        Serial.write(c);                    // Print serial
        header += c;
        if (c == '\n') {                    // IF byte nova linha
          // FIM DA REQUISIÇÃO WIFI. INICIO RESPOSTA.
          if (currentLine.length() == 0) {
            // HTTP teste
            // tipo de conteúdo
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Conexão: sair");
            client.println();

            // PÁGINA HTML
            client.println("<!DOCTYPE html><html lang=\"pt-br\">");
            client.println("<head><meta charset =\"utf-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>");
            client.println("<link rel=\"icon\" href=\"https://cdn.discordapp.com/attachments/753467122860163144/784045838812708904/pet250x250.png\">");
            client.println("<title>Dispenser Automatizado - SMART PET - O MELHOR PARA SEU PET</title> </head>");
            
            // CSS 
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto; background-color: #f57035;}");
            client.println(".slider { width: 66%; }");
            client.println(".centralizacao { margin-top: 35%; z-index: 1; position: absolute; color: white; border: 1px black; padding-left: 1,5%}</style>");   
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
                     
            // PAGINA HTML
            client.println("</head><body><div class=\"centralizacao\"><h1>Abra seu dispenser</h1>");
            client.println("<p>Posição do feixe em graus: <span id=\"servoPos\"></span></p>"); 
            client.println("<p>O feixe irá se abrir conforme você deslizar para a direita</p>");
            client.println("<input value=\"0\" type=\"range\" min=\"0\" max=\"90\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+valueString+"\"/>");
            client.println("<img src=\"https://cdn.discordapp.com/attachments/753467122860163144/784108100982538270/pet.png\" alt=\"logo\">");

            // JSCRIPT
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");
           
            client.println("</div></body></html>");     
            
            //pegar valor servoPos
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              
              //Mexer servo
              myservo.write(valueString.toInt());
              Serial.println(valueString); 
            }         
            client.println();
            // Break loop
            break;
          } else { // ELSE COM CLEAR
            currentLine = "";
          }
        } else if (c != '\r') {  // IF Não conseguir nada retornar um caractere
          currentLine += c;      // Adicionar isso no final da currentLine
        }
      }
    }
    // Clear Header
    header = "";
    // Fechar conexão
    client.stop();
    Serial.println("Cliente Desconectado.");
    Serial.println("");
  }
}

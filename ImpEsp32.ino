//Bibliotecas necessárias
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

#define pin_rele1 5 // Geladeira
#define pin_rele2 4 // Lâmpada

// Informações de conexão Wi-Fi
const char* ssid = "Nome_da_Rede";
const char* password = "Senha_da_Rede";

// Informações do servidor MQTT
const char* mqttServer = "mqtt.tago.io";
const int mqttPort = 1883;
const char* mqttUser = "Token";
const char* mqttPassword = "Token-Gerado-No-TagoIO";

// Tópicos MQTT
const char* topic_temp = "esp32/temp/iSpindel";
const char* topic_temp1 = "esp32/temp1";
const char* topic_hist1 = "esp32/hist1";
const char* topic_dur1 = "esp32/dur1";
const char* topic_temp2 = "esp32/temp2";
const char* topic_hist2 = "esp32/hist2";
const char* topic_dur2 = "esp32/dur2";
const char* topic_temp3 = "esp32/temp3";
const char* topic_hist3 = "esp32/hist3";
const char* topic_dur3 = "esp32/dur3";
const char* topic_avancar = "esp32/avancar";

Preferences preferences;

//Variáveis Globais:
int avancar, etapa_atual=1;
float temp,temp1=0.0,temp2=0.0,temp3=0.0,hist1=0.0,hist2=0.0,hist3=0.0;
double dur1=0.0,dur2=0.0,dur3=0.0;
unsigned long etapa1_inicio = 0,etapa2_inicio = 0,etapa3_inicio = 0,tempo_decorrido1=0,tempo_decorrido2=0,tempo_decorrido3=0;
String mensagem_temp,mensagem_temp1, mensagem_hist1, mensagem_dur1,mensagem_temp2, mensagem_hist2,mensagem_dur2,mensagem_temp3,
mensagem_hist3,mensagem_dur3, mensagem_avancar;

// Cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  
  Serial.begin(9600); //  A taxa de transmissão será de 9600 bits por segundo.
  
  pinMode(pin_rele1,OUTPUT); // Inicializando os pinos 5 (pin_rele1) e 4(pin_rele2) dos relés como saida (OUTPUT).
  pinMode(pin_rele2,OUTPUT);  
  
  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  
  // Conexão MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT.");
      client.subscribe(topic_temp);
      client.subscribe(topic_temp1);
      client.subscribe(topic_hist1);
      client.subscribe(topic_dur1);
      client.subscribe(topic_temp2);
      client.subscribe(topic_hist2);
      client.subscribe(topic_dur2);
      client.subscribe(topic_temp3);
      client.subscribe(topic_hist3);
      client.subscribe(topic_dur3);
      client.subscribe(topic_avancar);
    } else {
      Serial.print("Falha na conexão MQTT. ");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }

  // Inicializa a memória NVS e Recupera as variáveis globais da memória NVS, se existirem.
  preferences.begin("my-app", false); 
  temp1 = preferences.getFloat("temp1", 0.0);
  temp2 = preferences.getFloat("temp2", 0.0);
  temp3 = preferences.getFloat("temp3", 0.0);
  hist1 = preferences.getFloat("hist1", 0.0);
  hist2 = preferences.getFloat("hist2", 0.0);
  hist3 = preferences.getFloat("hist3", 0.0);
  dur1 = preferences.getDouble("dur1", 0.0);
  dur2 = preferences.getDouble("dur2", 0.0);
  dur3 = preferences.getDouble("dur3", 0.0);
  avancar = preferences.getInt("avancar", 0);
  etapa_atual = preferences.getInt("etapa_atual", 1);
  preferences.end();

 //Verificando em qual etapa estava antes da queda de energia.
  if((hist1>0.0 || dur1>0.0)&& etapa_atual == 1){
    client.publish("esp32/etapa", "Etapa 1 retornando.");
    etapa1_inicio = millis();
  }
  if((hist2>0.0 || dur2>0.0) && etapa_atual == 2){
    client.publish("esp32/etapa", "Etapa 2 retornando.");
    etapa2_inicio = millis();
  } 
  if((hist3>0.0 || dur3>0.0) && etapa_atual == 3){
    client.publish("esp32/etapa", "Etapa 3 retornando.");
    etapa3_inicio = millis();
  }
  if(etapa_atual == 4){
    client.publish("esp32/etapa", "Fermentação concluida!");
  }
}

void loop() {  
  // Verifica se o cliente está conecatdo ao broker MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Responsável por manter a comunicação e o processamento das msg MQTT. 
}

void reconnect() {
  //Responsável por estabelecer a conexão com o broker MQTT
  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT.");
      client.subscribe(topic_temp);
      client.subscribe(topic_temp1);
      client.subscribe(topic_hist1);
      client.subscribe(topic_dur1);
      client.subscribe(topic_temp2);
      client.subscribe(topic_hist2);
      client.subscribe(topic_dur2);
      client.subscribe(topic_temp3);
      client.subscribe(topic_hist3);
      client.subscribe(topic_dur3);
      client.subscribe(topic_avancar);
    }else {
      Serial.print("Falha na conexão MQTT. ");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void salvar_nvs() {
  //Inicia a área de armazenamento NVS com o namespace "my-app". 
  //O segundo parâmetro, false, indica que não deve ser criado um espaço de armazenamento EEPROM emulada.
  preferences.begin("my-app", false);
  preferences.putFloat("temp1", temp1); //Armazena os valores das variáveis no espaço de armazenamento NVS com as suas respectivas chaves.
  preferences.putFloat("temp2", temp2);
  preferences.putFloat("temp3", temp3);
  preferences.putFloat("hist1", hist1);
  preferences.putFloat("hist2", hist2);
  preferences.putFloat("hist3", hist3);
  preferences.putDouble("dur1", dur1);
  preferences.putDouble("dur2", dur2);
  preferences.putDouble("dur3", dur3);
  preferences.putInt("avancar", avancar);
  preferences.putInt("etapa_atual", etapa_atual);
  preferences.end();
}

void limpar_nvs() {
 preferences.begin("my-app", false);
 preferences.clear(); // Exclui todos os valores da memória NVS.
 preferences.end();
}

void callback(char* topic, byte* payload, unsigned int length) { // Processa os tópicos MQTT recebidos. 
  
  String msg = ""; // Converter o payload em uma string
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i]; 
  }

  //Identificando os tópicos MQTT enviados pelo Broker
  if (String(topic) == topic_temp) {
    mensagem_temp = msg;
    temp = mensagem_temp.toFloat();
  } else if (String(topic) == topic_temp1) {
    mensagem_temp1 = msg;
    temp1 = mensagem_temp1.toFloat();
  } else if (String(topic) == topic_hist1) {
    mensagem_hist1 = msg;
    hist1 = mensagem_hist1.toFloat();
  } else if (String(topic) == topic_dur1) {
    mensagem_dur1 = msg;
    dur1 = mensagem_dur1.toDouble();
    etapa1_inicio = millis(); //Obtendo o tempo em milissegundos desse instant.
    etapa_atual=1; //Iniciando na etapa 1.
  } else if (String(topic) == topic_temp2) {
    mensagem_temp2 = msg;
    temp2 = mensagem_temp2.toFloat();
  } else if (String(topic) == topic_hist2) {
    mensagem_hist2 = msg;
    hist2 = mensagem_hist2.toFloat();
  } else if (String(topic) == topic_dur2) {
    mensagem_dur2 = msg;
    dur2 = mensagem_dur2.toDouble();
  } else if (String(topic) == topic_temp3) {
    mensagem_temp3 = msg;
    temp3 = mensagem_temp3.toFloat();
  } else if (String(topic) == topic_hist3){
    mensagem_hist3 = msg; 
    hist3 = mensagem_hist3.toFloat();
  } else if (String(topic) == topic_dur3) {
    mensagem_dur3 = msg;
    dur3 = mensagem_dur3.toDouble();
  }else if (String(topic) == topic_avancar){
   mensagem_avancar = msg;
   avancar = mensagem_avancar.toInt();
  }
  
  salvar_nvs();  //Salva os dados na NVS.
  
  if(!temp){
    Serial.println("temp não foi enviado..."); //Aguarda o envio da temperatura da iSpindel.
  }else{ 
      if(etapa_atual == 1){  
      client.publish("esp32/etapa", "Etapa 1");          
      if (millis() - etapa1_inicio <= dur1 * 24 * 60 * 60 * 1000) { // Verifica se ainda está dentro do período da etapa 1
      // ETAPA 1       
      if (temp >= (temp1 + hist1)) {
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); // Desliga L
        client.publish("esp32/etapa", "Etapa 1 - G on e L off.");
      }else if(temp>=(temp1-hist1/2) && temp<=(temp1+hist1) && digitalRead(pin_rele1)==HIGH){
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); // Desliga L
        client.publish("esp32/etapa", "Etapa 1 - G on e L off.");
      }else if (temp <= (temp1 - hist1)) {
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); // Liga L
        client.publish("esp32/etapa", "Etapa 1 - G off e L on.");
      }else if(temp>=(temp1-hist1) && temp<=(temp1+hist1/2) && digitalRead(pin_rele2)==HIGH){
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); // Liga L
        client.publish("esp32/etapa", "Etapa 1 - G off e L on.");
      }else{
        digitalWrite(pin_rele1,LOW);  //G e L ficam deligados. 
        digitalWrite(pin_rele2,LOW);
        client.publish("esp32/etapa", "Etapa 1 - G off e L off.");
      }

      tempo_decorrido1 = (millis() - etapa1_inicio);      
      if(tempo_decorrido1 >= 10800000){ //Verifica se o tempo em milissegundos é maior ou igual a 10800000, que corresponde a 3 horas.
        dur1=dur1-0.125; // Decrementa 0,125 (3 horas) de dur1. 
        salvar_nvs();
        etapa1_inicio = millis(); //Recebe o o tempo em milissegundos desse instante.
        tempo_decorrido1=0;
      }
     
      if(avancar == 2){ // Avançar para etapa 2.
        // Finaliza a primeira etapa.
        client.publish("esp32/etapa", "Etapa 1 terminou.");   
        if(dur2==0.0){ //Verifica se existe a segunda etapa 2.
          client.publish("esp32/etapa", "Etapas concluidas!");
          etapa_atual=4;
          salvar_nvs();
        }else{ 
         //Avança para a segunda etapa
         etapa_atual=2;
         salvar_nvs();
         etapa2_inicio = millis();
         if (temp >= (temp2 + hist2)) {
           digitalWrite(pin_rele1, HIGH); //Liga G
           digitalWrite(pin_rele2,LOW); //Desliga L
           client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
          }else if(temp>=(temp2-hist2/2) && temp<=(temp2+hist2) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
          }else if (temp <= (temp2 - hist2)) {
            digitalWrite(pin_rele1, LOW);  // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
          }else if(temp>=(temp2-hist2) && temp<=(temp2+hist2/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
          }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 2 - G off e L off.");
          } 
        }           
     }         
    }else{
      // O tempo da etapa 1 foi finalizado.
      client.publish("esp32/etapa", "Etapa 1 terminou."); 
      if(dur2==0.0){ //Verifica se existe a segunda etapa 2.
          client.publish("esp32/etapa", "Etapas concluidas!");
          etapa_atual=4;
          salvar_nvs();
        }else{
         etapa_atual=2;
         salvar_nvs();
         etapa2_inicio = millis();
         if (temp >= (temp2 + hist2)) {
           digitalWrite(pin_rele1, HIGH); //Liga G
           digitalWrite(pin_rele2,LOW); //Desliga L
           client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
          }else if(temp>=(temp2-hist2/2) && temp<=(temp2+hist2) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); //Desliga L
            client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
          }else if (temp <= (temp2 - hist2)) {
            digitalWrite(pin_rele1, LOW); //Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
          }else if(temp>=(temp2-hist2) && temp<=(temp2+hist2/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); //Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
          }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 2 - G off e L off.");
          } 
        }  
     }
   } else if(etapa_atual == 2){ 
     client.publish("esp32/etapa", "Etapa 2");   
       
     if (millis() - etapa2_inicio <= dur2 * 24 * 60 * 60 * 1000) { // Verifica se ainda está dentro do período da etapa 2 -> dur2 * 24 * 60 * 60 * 1000
       // ETAPA 2 
     
      if (temp >= (temp2 + hist2)) {
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); //Desliga L
        client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
      }else if(temp>=(temp2-hist2/2) && temp<=(temp2+hist2) && digitalRead(pin_rele1)==HIGH){
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); //Desliga L
        client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
      }else if (temp <= (temp2 - hist2)) {
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); //Liga L
        client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
      }else if(temp>=(temp2-hist2) && temp<=(temp2+hist2/2) && digitalRead(pin_rele2)==HIGH){
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); //Liga L
        client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
      }else{
        digitalWrite(pin_rele1,LOW); // G e L desligados
        digitalWrite(pin_rele2,LOW);
        client.publish("esp32/etapa", "Etapa 2 - G off e L off.");
      }

      tempo_decorrido2 = (millis() - etapa2_inicio);
      
      if(tempo_decorrido2 >= 10800000){ //Verifica se o tempo em milissegundos é maior ou igual a 10800000, que corresponde a 3 horas.
        dur2=dur2-0.125; // Decrementa 0,125 (3 horas) de dur2. 
        salvar_nvs();
        etapa2_inicio = millis(); //Recebe o o tempo em milissegundos desse instante.
        tempo_decorrido2=0;
      }
  

      if(avancar == 3){
        client.publish("esp32/etapa", "Etapa 2 terminou.");   
        if(dur3==0.0){ //Verifica se existe a segunda etapa 3.
          client.publish("esp32/etapa", "Etapas concluidas!");
          etapa_atual=4;
          salvar_nvs();
        }else{
          etapa_atual=3;
          salvar_nvs();
          etapa3_inicio = millis(); 
          if (temp >= (temp3 + hist3)) {
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
          }else if(temp>=(temp3-hist3/2) && temp<=(temp3+hist3) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
          }else if (temp <= (temp3 - hist3)) {
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
          }else if(temp>=(temp3-hist3) && temp<=(temp3+hist3/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
          }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 3 - G off e L off.");
          } 
        }
      }
      
     }else{
        client.publish("esp32/etapa", "Etapa 2 terminou."); 
        if(dur3==0.0){ //Verifica se existe a segunda etapa 3.
          client.publish("esp32/etapa", "Etapas concluidas!");
          etapa_atual=4;
          salvar_nvs();
        }else{
          etapa_atual=3;
          salvar_nvs();
          etapa3_inicio = millis(); 
          if (temp >= (temp3 + hist3)) {
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
          }else if(temp>=(temp3-hist3/2) && temp<=(temp3+hist3) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
          }else if (temp <= (temp3 - hist3)) {
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); //  Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
          }else if(temp>=(temp3-hist3) && temp<=(temp3+hist3/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); //  Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
          }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 3 - G off e L off.");
          }  
        } 
     }
   }else if(etapa_atual == 3){  
    client.publish("esp32/etapa", "Etapa 3");    
    if (millis() - etapa3_inicio <= dur3 * 24 * 60 * 60 * 1000) { // Verifica se ainda está dentro do período da etapa 3
     //ETAPA 3 
    
       if (temp >= (temp3 + hist3)) {
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
       }else if(temp>=(temp3-hist3/2) && temp<=(temp3+hist3) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
       }else if (temp <= (temp3 - hist3)) {
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
       }else if(temp>=(temp3-hist3) && temp<=(temp3+hist3/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
        }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 3 - G off e L off.");
       }  

       tempo_decorrido3 = (millis() - etapa3_inicio);
      
      if(tempo_decorrido3 >= 10800000){ //Verifica se o tempo em milissegundos é maior ou igual a 10800000, que corresponde a 3 horas.
        dur3=dur3-0.125; // Decrementa 0,125 (3 horas) de dur3. 
        salvar_nvs();
        etapa3_inicio = millis(); //Recebe o o tempo em milissegundos desse instante.
        tempo_decorrido3=0;
      }

    if(avancar == 4){
        client.publish("esp32/etapa", "Etapa 3 terminou."); 
        client.publish("esp32/etapa", "Fermentação concluida!"); 
        etapa_atual=4;
        salvar_nvs();        
   }
        
   }else{
    client.publish("esp32/etapa", "Etapa 3 terminou."); 
    client.publish("esp32/etapa", "Etapas concluidas!");   
    etapa_atual=4; // Etapas concluidas
    salvar_nvs();
   }
  }else{
      
      if((temp1>=0.0 && hist1>0.0 && dur1>=0.0) && (temp2==0.0 && hist2==0.0 && dur2==0.0) && (temp3==0.0 && hist3==0.0 && dur3==0.0)){
        // Se teve somente uma etapa de fermentação, então continua o controle da tempertura na etapa1.
       if (temp >= (temp1 + hist1)) {
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); // Desliga L
        client.publish("esp32/etapa", "Etapa 1 - G on e L off.");
       }else if(temp>=(temp1-hist1/2) && temp<=(temp1+hist1) && digitalRead(pin_rele1)==HIGH){
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); // Desliga L
        client.publish("esp32/etapa", "Etapa 1 - G on e L off.");
       }else if (temp <= (temp1 - hist1)) {
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); // Liga L
        client.publish("esp32/etapa", "Etapa 1 - G off e L on.");
       }else if(temp>=(temp1-hist1) && temp<=(temp1+hist1/2) && digitalRead(pin_rele2)==HIGH){
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH);// Liga L
        client.publish("esp32/etapa", "Etapa 1 - G off e L on.");
       }else{
        digitalWrite(pin_rele1,LOW); // G e L desligados
        digitalWrite(pin_rele2,LOW);
        client.publish("esp32/etapa", "Etapa 1 - G off e L off.");
       }
      }else if((temp1>=0.0 && hist1>0.0 && dur1>=0.0) && (temp2>=0.0 && hist2>0.0 && dur2>=0.0) && (temp3==0.0 && hist3==0.0 && dur3==0.0)){
        // Se teve somente duas etapas de fermentação, então continua o controle da tempertura na etapa2.
                 
       if (temp >= (temp2 + hist2)) {
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); //  Desliga L
        client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
       }else if(temp>=(temp2-hist2/2) && temp<=(temp2+hist2) && digitalRead(pin_rele1)==HIGH){
        digitalWrite(pin_rele1, HIGH); // Liga G
        digitalWrite(pin_rele2,LOW); // Desliga L
        client.publish("esp32/etapa", "Etapa 2 - G on e L off.");
       }else if (temp <= (temp2 - hist2)) {
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); // Liga L
        client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
       }else if(temp>=(temp2-hist2) && temp<=(temp2+hist2/2) && digitalRead(pin_rele2)==HIGH){
        digitalWrite(pin_rele1, LOW); // Desliga G
        digitalWrite(pin_rele2,HIGH); // Liga L
        client.publish("esp32/etapa", "Etapa 2 - G off e L on.");
       }else{
        digitalWrite(pin_rele1,LOW); // G e L desligados
        digitalWrite(pin_rele2,LOW);
        client.publish("esp32/etapa", "Etapa 2 - G off e L off.");
       }

      }else{
        // Se teve três etapas de fermentação, então continua o controle da tempertura na etapa3.
       if (temp >= (temp3 + hist3)) {
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Desliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
       }else if(temp>=(temp3-hist3/2) && temp<=(temp3+hist3) && digitalRead(pin_rele1)==HIGH){
            digitalWrite(pin_rele1, HIGH); // Liga G
            digitalWrite(pin_rele2,LOW); // Deliga L
            client.publish("esp32/etapa", "Etapa 3 - G on e L off.");
       }else if (temp <= (temp3 - hist3)) {
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
       }else if(temp>=(temp3-hist3) && temp<=(temp3+hist3/2) && digitalRead(pin_rele2)==HIGH){
            digitalWrite(pin_rele1, LOW); // Desliga G
            digitalWrite(pin_rele2,HIGH); // Liga L
            client.publish("esp32/etapa", "Etapa 3 - G off e L on.");
        }else{
            digitalWrite(pin_rele1,LOW); // G e L desligados
            digitalWrite(pin_rele2,LOW);
            client.publish("esp32/etapa", "Etapa 3 - G off e L off.");
       }  
     }

     client.publish("esp32/etapa", "Fermentação concluida!"); // Enviando fermentação concluida para o broker (TagoIO).

     if(avancar == 5){ //Encerra a fermentação, excluindo todos os dados da memória NVS.
       client.publish("esp32/etapa", "Deseja iniciar uma nova fermentação?");
       limpar_nvs();
    }
  }
 }
}

// Importando as classes necessárias do pacote @tago-io/sdk
const { Analysis, Device, Utils, Services } = require("@tago-io/sdk");

async function mqttPushExample(context, scope) {
  
  if (!scope.length) {
    return context.log("This analysis must be triggered by a dashboard.");
  }

  // Encontrando as variáveis da receita.
  const temp_e1 = scope.find((x) => x.variable === "temp_etapa1") || scope[0];
  
  const hist_e1 = scope.find((x) => x.variable === "histerese_etapa1") || scope[0];

  const dur_e1 = scope.find((x) => x.variable === "duracao_etapa1") || scope[0];
  
  const temp_e2 = scope.find((x) => x.variable === "temp_etapa2") || scope[0];
 
  const hist_e2 = scope.find((x) => x.variable === "histerese_etapa2") || scope[0];

  const dur_e2 = scope.find((x) => x.variable === "duracao_etapa2") || scope[0];

  const temp_e3 = scope.find((x) => x.variable === "temp_etapa3") || scope[0];

  const hist_e3 = scope.find((x) => x.variable === "histerese_etapa3") || scope[0];
 
  const dur_e3 = scope.find((x) => x.variable === "duracao_etapa3") || scope[0];

 //Verificando se existe valores nas variáveis. 
 // Se não existir, será atribuido zero.
 if(!temp_e1.value){
    temp_e1.value = 0;
 }
 
 if(!temp_e2.value){
    temp_e2.value = 0;  
 }

 if(!temp_e3.value){
    temp_e3.value = 0;  
 }

  if(!hist_e1.value){
    hist_e1.value = 0;  
 }

if(!hist_e2.value){
    hist_e2.value = 0;  
 }
 
 if(!hist_e3.value){
    hist_e3.value = 0;  
 }
 
 if(!dur_e1.value){
    dur_e1.value = 0;  
 }

if(!dur_e2.value){
    dur_e2.value = 0;  
 }
 
 if(!dur_e3.value){
    dur_e3.value = 0;  
 }

  const options = {
    retain: false,
    qos: 0,
  };

  // Cria uma instância da classe MQTT do serviço TagoIO, passando o token do contexto.
  const MQTT = new Services({ token: context.token }).MQTT;

// Publica as mensagens nos tópicos MQTT corrspondentes.
  MQTT.publish({
      bucket: temp_e1.device, 
      message: temp_e1.value,
      topic: "esp32/temp1",
      options,
  }).then(context.log, context.log)

    MQTT.publish({
      bucket: hist_e1.device, 
      message: hist_e1.value,
      topic: "esp32/hist1",
      options,
  }).then(context.log, context.log)

      MQTT.publish({
      bucket: dur_e1.device, 
      message: dur_e1.value,
      topic: "esp32/dur1",
      options,
  }).then(context.log, context.log)

      MQTT.publish({
      bucket: temp_e2.device,
      message: temp_e2.value,
      topic: "esp32/temp2",
      options,
  }).then(context.log, context.log)

      MQTT.publish({
      bucket: hist_e2.device, 
      message: hist_e2.value,
      topic: "esp32/hist2",
      options,
  }).then(context.log, context.log)
 
  MQTT.publish({
      bucket: dur_e2.device, 
      message: dur_e2.value,
      topic: "esp32/dur2",
      options,
  }).then(context.log, context.log)

        MQTT.publish({
      bucket: temp_e3.device, 
      message: temp_e3.value,
      topic: "esp32/temp3",
      options,
  }).then(context.log, context.log)

      MQTT.publish({
      bucket: hist_e3.device, 
      message: hist_e3.value,
      topic: "esp32/hist3",
      options,
  }).then(context.log, context.log)
 
  MQTT.publish({
      bucket: dur_e3.device, 
      message: dur_e3.value,
      topic: "esp32/dur3",
      options,
  }).then(context.log, context.log)
}

//Exportando uma instância da classe Analysis com a função mqttPushExample como argumento
module.exports = new Analysis(mqttPushExample);
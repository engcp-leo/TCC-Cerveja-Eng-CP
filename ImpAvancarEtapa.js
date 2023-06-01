//Importando as classes necessárias do pacote @tago-io/sdk
const { Analysis, Device, Utils, Services } = require("@tago-io/sdk"); 

async function mqttPushExample(context, scope) {
  
  if (!scope.length) { // Verificando se o escopo está vazio e retornando uma mensagem de log se for o caso
    return context.log("This analysis must be triggered by a dashboard.");
  }

  //Procurando pela variável "avancar" no escopo ou usando o primeiro item do escopo caso não seja encontrada
  const avancar_etapa = scope.find((x) => x.variable === "avancar") || scope[0];

 // Verificando se a variável avancar_etapa não está definida no escopo e retornando uma mensagem de log se for o caso
  if (!avancar_etapa) { 
    return context.log("Couldn't find any variable in the scope.");
  }

 const env_vars = Utils.envToJson(context.environment); // Convertendo as variáveis de ambiente em um objeto JavaScript

 // Verificando se a variável device_token não está definida nas variáveis de ambiente e retornando uma mensagem de log se for o caso
  if (!env_vars.device_token) {
    return context.log("Device token not found on environment parameters");
  }

// Criando uma nova instância da classe Device com o token do dispositivo
 const device_iSpindel = new Device({ token: env_vars.device_token });

 const etapa_atual = await device_iSpindel.getData({variable: "etapa"});
 
 // Criando uma nova instância da classe Notification do serviço TagoIO
 const notification = new Services({ token: context.token }).Notification;

 //Verificando se a etapa atual é a etapa 1 e se a opção escolhida é diferente de 2. Em caso de erro, 
 // envia uma notificação e retorna uma mensagem de log.
 if((etapa_atual[0].value == "Etapa 1" || etapa_atual[0].value == "Etapa 1 - G on e L off." || 
  etapa_atual[0].value == "Etapa 1 - G off e L on." || etapa_atual[0].value == "Etapa 1 - G off e L off." || 
  etapa_atual[0].value == "Etapa 1 retornando.") && avancar_etapa.value != 2){
   console.log("\n Erro na escolha das oções de etapas...");
    const service_response = await notification.send({
      title: "Alerta",
      message: "Não é possivel avançar para a etapa escolhida, pois no momento está na etapa 1.",
      ref_id: env_vars.dashboard_id || undefined,
    });
    return context.log(service_response);
 } 
 
 // Verificando se a etapa atual é a etapa 2 e se a opção escolhida é diferente de 3. Em caso de erro, 
 // envia uma notificação e retorna uma mensagem de log.
 if ((etapa_atual[0].value == "Etapa 2" || etapa_atual[0].value == "Etapa 2 - G on e L off." ||
   etapa_atual[0].value == "Etapa 2 - G off e L on." || etapa_atual[0].value == "Etapa 2 - G off e L off." || 
   etapa_atual[0].value == "Etapa 2 retornando.") && avancar_etapa.value != 3){
   console.log("\n Erro na escolha das oções de etapas...");
    const service_response = await notification.send({
      title: "Alerta",
      message: "Não é possivel avançar para a etapa escolhida, pois no momento está na etapa 2.",
      ref_id: env_vars.dashboard_id || undefined,
    });
    return context.log(service_response);
 } 

 // Verificando se a etapa atual é a etapa 3 e se a opção escolhida é diferente de 4. Em caso de erro, 
 // envia uma notificação e retorna uma mensagem de log.
  if ((etapa_atual[0].value == "Etapa 3" || etapa_atual[0].value == "Etapa 3 - G on e L off." || 
   etapa_atual[0].value == "Etapa 3 - G off e L on." || etapa_atual[0].value == "Etapa 3 - G off e L off." || 
   etapa_atual[0].value == "Etapa 3 retornando.") && avancar_etapa.value != 4){
   console.log("\n Erro na escolha das oções de etapas...");
    const service_response = await notification.send({
      title: "Alerta",
      message: "Não é possivel avançar para a etapa escolhida, pois no momento está na etapa 3.",
      ref_id: env_vars.dashboard_id || undefined,
    });
    return context.log(service_response);
 } 
 
 // Verificando se a opção escolhida está fora do intervalo válido (1-5). Em caso de erro, 
 // envia uma notificação e retorna uma mensagem de log.
 if(avancar_etapa.value < 1 || avancar_etapa.value > 5){
    console.log("\n Erro na escolha das oções de etapas...");
    const service_response = await notification.send({
      title: "Alerta",
      message: "Não é possivel avançar para a etapa escolhida.",
      ref_id: env_vars.dashboard_id || undefined,
    });
    return context.log(service_response);
 }

 // Cria um objeto com as opções para a publicação MQTT. 
  const options = {
    retain: false,
    qos: 0,
  };

 // Cria uma instância da classe MQTT do serviço TagoIO, passando o token do contexto.
  const MQTT = new Services({ token: context.token }).MQTT;

// Publica uma mensagem no tópico "esp32/avancar" do MQTT com os valores da variável avancar_etapa. 
// O objeto options define as opções para a publicação. Em seguida, registra o resultado da operação no log.
   MQTT.publish({
      bucket: avancar_etapa.device, 
      message: avancar_etapa.value,
      topic: "esp32/avancar",
      options,
  }).then(context.log, context.log)
}

//Exportando uma instância da classe Analysis com a função mqttPushExample como argumento
module.exports = new Analysis(mqttPushExample);
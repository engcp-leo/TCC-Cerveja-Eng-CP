// Importa as classes Analysis, Device, Utils e Services do SDK do TagoIO.
const { Analysis, Device, Utils, Services } = require("@tago-io/sdk");

async function mqttPushExample(context, scope) {
  
  // Verifica se o scope está vazio, se estiver vazio, registra uma mensagem de log e retorna.
  if (!scope.length) {
    return context.log("This analysis must be triggered by a dashboard.");
  }

  // Procura em scope a variável com o nome 'temperature' e a atribui à variável my_temp.
  const my_temp = scope.find((x) => x.variable === "temperature") || scope[0];

 // Verifica se my_temp está vazio, indicando que a variável 'temperature' não foi encontrada em scope.
  if (!my_temp) {
    return context.log("Couldn't find any variable in the scope.");
  }

 // Converte as variáveis de ambiente do contexto em um objeto JSON e atribui a env_vars.
 const env_vars = Utils.envToJson(context.environment);

// Verifica se a chave 'device_token' não está presente em env_vars.
  if (!env_vars.device_token) {
    return context.log("Device token not found on environment parameters");
  }

// Cria uma instância da classe Device do TagoIO, passando o token do dispositivo recuperado de env_vars.
 const device_iSpindel = new Device({ token: env_vars.device_token });

 const temp_atual = await device_iSpindel.getData({variable: "temperature"});
 
  // Cria um objeto com as opções para a publicação MQTT.
  const options = {
    retain: false,
    qos: 0,
  };

  const MQTT = new Services({ token: context.token }).MQTT;

 // Publica uma mensagem no tópico "esp32/temp/iSpindel" do MQTT com os valores da variável temp_atual.
  MQTT.publish({      
      bucket: temp_atual[0].device, 
      message: temp_atual[0].value,
      topic: "esp32/temp/iSpindel",
      options,
  }).then(context.log, context.log)
}

//Exportando uma instância da classe Analysis com a função mqttPushExample como argumento
module.exports = new Analysis(mqttPushExample);
// Importando as classes necessárias do pacote @tago-io/sdk
const { Analysis, Account, Device, Utils } = require("@tago-io/sdk");

async function myAnalysis(context, scope) {

  const env_vars = Utils.envToJson(context.environment); //Convertendo as variáveis de ambiente em um objeto JavaScript

  if (!env_vars.account_token) { // Verificando se a variável account_token está definida nas variáveis de ambiente
    throw context.log("Missing account_token in the environment variables");
  }

  const device_id = scope[0].device  //Obtendo o ID do dispositivo a partir do primeiro registro do escopo

  // Criando uma instância da classe Account com o token da conta
  const account = new Account({ token: env_vars.account_token });
  
  // Excluindo todos os dados do dispositivo especificado pelo device_id
  const result = await account.devices.emptyDeviceData(device_id);

  context.log(result); // Registrando o resultado da operação no log

}

//Exportando uma instância da classe Analysis com a função myAnalysis como argumento
module.exports = new Analysis(myAnalysis); 
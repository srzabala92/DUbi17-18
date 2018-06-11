/*Librería Api Rest*/
#include "ApiRest.h"
ApiRest::ApiRest(){
	humedad = 0.0, temperatura = 0.0 ,ver = 0.0;
	/** Definir umbrales de temperatura y humedad máximos y mínimos,
según las especificaciones del fabricante.*/
	MAX_HUM = 80, MIN_HUM = 0 , MAX_TMP = 50, MIN_TMP = 0;
}
void ApiRest::configurar(){
	//INICIALIZACIÓN BRIDGE
	//Serial.print("Inicializando Bridge....");
	//Bridge.begin();
	//Serial.print("OK!\n");
	//INICIALIZANDO API REST (LEVANTANDO SERVIDOR)
	//Serial.print("Arrancando servicio Api-REST -> Servidor escuchando en localhost....");
 	server.listenOnLocalhost();
  server.begin();
  	//Serial.print("OK!\n");

}
void ApiRest::atenderClientes(){
  BridgeClient client = server.accept();
  // There is a new client?
  if (client) {
    // Process request
    procesar(client);
    // Close connection and free resources.
    client.stop();
    //Serial.println("Cliente procesado con éxito");
  }
  delay(50); // Poll every 50ms
}
void ApiRest::procesar(BridgeClient client){
  // read the command
  //client.println("API Proyecto DU - Sergio Zabala Mesonero");
  //client.print("---------------------------------------\n");
  client.flush();
  String command = client.readStringUntil('/');
  // Si la petición es sobre la temperatura
  if (command.startsWith("temp")) {
    //client.print("Valor actual de temperatura: ");
    procesarTemperatura(client);
  }
 // Si la petición es sobre la humedad
  if (command.startsWith("hum")) {
    //client.print("Valor actual de humedad: ");
    procesarHumedad(client);
  }
  // Si la petición es sobre la versión actual
  if (command.startsWith("version")) {
    client.print(String(ver));
  }
}
/**Procesamiento de la temperatura**/

void ApiRest::procesarTemperatura(BridgeClient client){
  //client.print("  grados C \n");
  String command = client.readStringUntil('/');
  if (command.startsWith("alta"))
  {
    procesarTempMAXmin(1,client);
    
  }
  else if (command.startsWith("baja"))
  {
    procesarTempMAXmin(0,client);
  }
  else client.print(temperatura);
}

/**Procesamiento de umbrales máximo y mínimo de la temperatura.
 * Este método se encarga de devolver el valor de los umbrales
 * o bien de cambiarlos, según la dirección que haya introducido el
 * cliente.
 * tipo: 0- Umbral mínimo 1-Umbral Máximo
 */
void ApiRest::procesarTempMAXmin(int tipo,BridgeClient client){
  String command = client.readStringUntil('/');
  command.trim();
  //client.flush();
  if(command != "")
  {
    if (tipo == 0)
    {
      MIN_TMP = command.toInt();
      client.print("El umbral minimo  de la temperatura es ahora: ");
      client.print(String(MIN_TMP)); 
    }
    if (tipo == 1)
    {
      MAX_TMP = command.toInt();
      client.print("El umbral maximo  de la temperatura es ahora: ");
      client.print(String(MAX_TMP) + "grados C");
    }
  }else 
  {
    if (tipo == 1) client.println(String(MAX_TMP));
    else client.println(String(MIN_TMP));
  }
}
/**Procesamiento de la Humedad**/
void ApiRest::procesarHumedad(BridgeClient client){
  String command = client.readStringUntil('/');
  if (command.startsWith("alta"))
  {
    procesarHumMAXmin(1,client);
    
  }
  else if (command.startsWith("baja"))
  {
    procesarHumMAXmin(0,client);
  }
  else  client.print(humedad);//client.print(" \n");

}
/**Procesamiento de umbrales máximo y mínimo de la humedad.
 * Este método se encarga de devolver el valor de los umbrales
 * o bien de cambiarlos, según la dirección que haya introducido el
 * cliente.
 * tipo: 0- Umbral mínimo 1-Umbral Máximo
 */
void ApiRest::procesarHumMAXmin(int tipo,BridgeClient client){
  String command = client.readStringUntil('/');
  command.trim();
  //client.flush();
  if(command != "")
  {
    if (tipo == 0)
    {
      MIN_HUM = command.toInt();
      client.print("El umbral minimo  de la humedad es ahora: ");
      client.print(String(MIN_HUM) + "%"); 
    }
    if (tipo == 1)
    {
      MAX_HUM = command.toInt();
      client.print("El umbral maximo de la humedad es ahora: ");
      client.print(String(MAX_HUM) + " % \n");
    }
  }else 
  {

    if (tipo == 1) client.println(String(MAX_HUM));
    else client.println(String(MIN_HUM));
  }
}
/**Métodos para obtener los valores de los umbrales y tenerlos actualizados**/
int ApiRest::umbralTempMax(){
  return MAX_TMP;
}
int ApiRest::umbralTempMin(){
  return MIN_TMP;
}
int ApiRest::umbralHumMax(){
  return MAX_HUM;
}
int ApiRest::umbralHumMin(){
  return MIN_HUM;
}
void ApiRest::actualizarTemp(float temp){
  temperatura = temp;
}
void ApiRest::actualizarHum(float hum){
  humedad = hum;
}
void ApiRest::actualizarVersion ( float _ver){
  ver = _ver;
}

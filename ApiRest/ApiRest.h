/* Librería ApiRest
 * Proyecto Dispositivos Ubicuos - Curso 2017/18
 * Autor: Sergio Zabala Mesonero
 */
#include "Arduino.h"
#ifndef ApiRest_h
#define ApiRest_h
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <LiquidCrystal.h>
#include <Process.h>

class ApiRest
{
public: 
 	ApiRest();
 	void atenderClientes();
 	void configurar();
 	void procesar(BridgeClient client);
 	void procesarTemperatura(BridgeClient client);
 	void procesarTempMAXmin(int tipo,BridgeClient client);
 	void procesarHumedad(BridgeClient client);
	void procesarHumMAXmin(int tipo,BridgeClient client);
	int umbralTempMax();
	int umbralTempMin();
	int umbralHumMax();
	int umbralHumMin();
	void actualizarTemp(float temp);
	void actualizarHum(float hum);
	void actualizarVersion ( float _ver);

private:
	float humedad , temperatura ,ver ;
	int  MAX_HUM , MIN_HUM  , MAX_TMP , MIN_TMP ;//Se guardarán como atributos de la clase ApiRest los valores umbral humedad y temperatura
	BridgeServer server;

};
#endif
/* Librería ProyectoDU
 * Será la encargada de gestionar las funciones más importantes
 * correspondientes al proyecto de la asignatura 
 * 'Dispositivos Ubicuos'
 * Autor: Sergio Zabala Mesonero
 */
#include "Arduino.h"
#include <ApiRest.h>
#ifndef ProyectoDU_h
#define ProyectoDU_h

/**Modo Depuración: Si está descomentado se activará.
 *Con el Modo de Depuración activo deberá esperar por el Monitor 
 *serie para continuar con el sketch
 */
#define DEBUG 

class ProyectoDU
{
  public : 
  ProyectoDU();
  void inicializarLCD();
  void configuracionInicial();
  bool leerValores(float _humedad,float _temperatura);
  void controlParpadeos();
  void mostrarHTLCD();
  void crearBD();
  void insertarEnBD();
  void escucharClientesAPI();
  void actualizacionSincronizacion();
  void enviarEmail(bool correcto);
  void envioABroker();
  void envioError();
  private:
  LiquidCrystal lcd;          //Variable que almacenará una instancia LiquidCrystal,
                               //para la gestión de la pantalla LCD.
  float humedad ,temperatura;//Variables para almacenar valores humedad y temperatura
  //String email;//Cadena con el mensaje a enviar,tanto en caso de error como en caso de éxito
  String mensaje;//Mensaje a mostrar por la LCD cuando haya problemas en la lectura
  /**Variables para el control de parpadeo en caso de error
   * Son contadores que determinarán el número de lecturas 
   * (iteraciones del bucle) que tiene que parpadear o no
   */
  int parpadea , noParpadea ;
  bool bloqueo; //Bandera para bloquear que haga 10 lecturas parpadeando o sin parpadear
  bool lectura;
  /** Variable para el proceso que llama a sistema Linux.*/
  Process proceso;
  ApiRest api = ApiRest();//Variable para el control de la API, contendrá 
                          // también los umbrales 
  
};
#endif
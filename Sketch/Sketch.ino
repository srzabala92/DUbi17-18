
/** Proyecto Dispositivos Ubicuos
 *  Alumno: Sergio Zabala Mesonero
 *  Entrega 2- 
 *  Extensión de la anterior entrega con nuevas funcionalidades:
 *  1) Sincronización de los Dispositivos mediante un reloj
 *  2) Actualización de los dispositivos con las versiones más recientes-
 *  3) Leer,publicar valores mediante una API-Rest
 *  4) Enviar información a servicio centralizado
 *  ----------------------------------------------
 *  Circuito:
 *  Pin D4 Shield - Pin D2 Arduino 
 *  Pin D5 Shield - Pin D3 Arduino 
 *  Pin D6 Shield - Pin D4 Arduino 
 *  Pin D7 Shield - Pin D5 Arduino 
 *  Pin D8 (RS) Shield - Pin D6 Arduino 
 *  Pin D9 (E) Shield - Pin D7 Arduino 
 *  Pin D10 (Nivel de luz LCD) Shield - GND
 *  Pin 5V,GND,RST Shield - Pines 5V,GND,RESET Arduino
 *  Pin A0 Shield - Pin A0 Arduino
 *  --------------------------------------------------
 *  Pin + DHT 11 - Pin 5V Shield
 *  Pin - DHT11 - PIN GND Shield
 *  Pin Datos DHT11 - Pin 8 Arduino
 */
#include <ProyectoDU.h>
#include <DHT.h>

/** Crear e inicializar sensor**/
#define TIPO_DHT DHT11
#define PIN_DHT 8
DHT dht(PIN_DHT,TIPO_DHT);

/** Se declara una instancia de la clase 'ProyectoDU',
 *  que contiene las funciones más importantes según 
 *  los requisitos del proyecto.
 */
ProyectoDU DU = ProyectoDU();

/**Variable para contar los 30 segundos para insertar datos en la BD.
 * Se comparará con el tiempo absoluto de ejecución del programa.
 */
unsigned long instanteAnterior=0,currentMillis = 0;
unsigned long interval = 30000;
/**Variable que cuenta cuántas veces han pasado 30 segundos. 
 * Esta se utilizará para 2 casos:
 * 1) Si ha pasado un múltiplo de 4, quiere decir que han pasado 2 minutos, para controlar
 * el envío de mensajes al servidor centralizado de Xively.
 * 2) Si ha pasado un múltiplo de 2880 (las veces que pasan 30 segundos al día), se realizará la ejecución
 * de los scripts de actualización y sincronización.
 */
int contSeg = 1 ;
/**Variables para el control de los envíos de e-mails:
 * 'ultimoEmail' - Marca temporal del último email enviado (deberá ser una hora mínimo para poder volver a enviar otro correo).
 * 'anteriorLectura' - Bandera que indica si la anterior lectura fue correcta o no.
 */

bool anteriorLectura = true , primerEmail = true;
//-----------------------------SETUP---------------------------------

void setup() {
  //Serial.begin(9600);
 Bridge.begin();
 //INICIALIZACIÓN ENTORNO DEL PROGRAMA
 DU.configuracionInicial();
 
}
//--------------------------------BUCLE DE SERVICIO----------------------------------------------

void loop() {
  currentMillis = millis();
  DU.escucharClientesAPI();//Levantar API para que puedan conectarse los clientes
  delay(100);//Lectura cada 0.1 s
  if(DU.leerValores(dht.readTemperature(),dht.readHumidity())){
    
    if( (currentMillis - instanteAnterior) > interval){
       instanteAnterior = currentMillis;
       DU.insertarEnBD();
       if(contSeg % 2 == 0)
              DU.envioABroker();
       if(contSeg % 120 ==0 )
              primerEmail = true;
       if(contSeg % 2880 ==0) 
              DU.actualizacionSincronizacion();
       contSeg++;
    }
     //if(!anteriorLectura ) { delay(1000);DU.enviarEmail(true);anteriorLectura = true;delay(100);}
  }else{
    if(primerEmail){
      DU.enviarEmail(false);
      anteriorLectura = false;
      primerEmail = false;
      delay(100);
      DU.envioError();
    }
    
  }
  DU.controlParpadeos();
  DU.mostrarHTLCD();//Mostrar valores en LCD
}



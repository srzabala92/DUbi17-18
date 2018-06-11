/*Implementación Librería ProyectoDU*/
#include "ProyectoDU.h"

/**Crear e inicializar LCD Display.
 * Los valores son (RS=6,E=7,D4=2,D5=3,D6=4,D7=5).
 */
	ProyectoDU::ProyectoDU():lcd(6,7,2,3,4,5){
		humedad = 0.0;
		temperatura = 0.0;
		parpadea = 0 ;
		noParpadea = 0;
		bloqueo = false;
		lectura = false;
		//email="";
		mensaje="";
	}
/**
* Inicializa el LCD de 16*2 para poder empezar a imprimir mensajes.
*/
	void ProyectoDU::inicializarLCD(){
 	//Inicialización del LCD Display
 		//Serial.print("Inicializando Display LCD....");
 		lcd.begin(16,2);
 		//Serial.print("OK!\n");
 	}
/**
 * Método que lee los valores de humedad y temperatura
 * y los almacena en las variables globales 'humedad' y 'temperatura'
 * Return: 'true' si no hay errores (no se pasa el Umbral minimo o máximo
 * y el sensor está conectado)
 * 'false' si hay errores en alguno de los valores
 */
	bool ProyectoDU::leerValores(float _humedad,float _temperatura){
		//Serial.print("Tomando valores de temperatura y humedad...");
		humedad = _humedad;
		temperatura = _temperatura;
		api.actualizarTemp(temperatura);
		api.actualizarHum(humedad);
		//Para el control de parpadeos
		lectura = false;
		 //Control de errores
	  	if( !(humedad>=0)  || !(temperatura >=0))
	  	{//Si el sensor se desconecta
	    
	  		//email = "'Subject: Error al leer dispositivo \r\n\r\n No se pudieron tomar valores,revise la conexión del dispositivo DHT11.'";
	  		mensaje = "DHT11-NO-OK";
	      return false;
	 	}
	  	else if(humedad > api.umbralHumMax()) 
	  	{
	    	
	     	//email = "'Subject: Humedad demasiado alta \r\n\r\n Revise el nivel de humedad de la sala.'";
	     	mensaje = "HUM-ALTA";

	      return false;
	  	}
	 	else if(humedad < api.umbralHumMin())
	  	{
	    	
			//email = "'Subject: Humedad demasiado baja \r\n\r\n Revise el nivel de humedad de la sala.'";	 
			mensaje = "HUM-BAJA"; 
			return false;
	  	}
	  	else if(temperatura > api.umbralTempMax())
	  	{
	    	
	    	//email = "'Subject: Temeratura demasiado alta \r\n\r\n Revise la temperatura de la sala.'";
	      	mensaje = "TEMP-ALTA";
	      	return false;
	  	}
	  	else if(temperatura < api.umbralTempMin())
	  	{
	    	
 		    //email = "'Subject: Temperatura demasiado baja \r\n\r\n Revise la temperatura de la sala.'";
 		    mensaje = "TEMP-BAJA";
	      return false;
	  	}
	  	//Serial.print("OK!\n");//Lectura correcta
	  	lectura = true;
	  	return true;
	}
/**
 * Método para controlar las veces que tiene que estar sin parpadear
 * o parpadeando un número determinado de lecturas (iteraciones del lop)
 * Para ello controla los valores de la lecura anterior y la actual.
 * -Si lectura actual es correcta  - No parpadea durante 10 lecturas
 * -Si lectura actual es errónea - Indica al programa 
 * que parpadee 10 lecturas consecutivas sean cuales sean las lecturas posteriores
 */
	void ProyectoDU::controlParpadeos(){
		//Se bloquea para que esté 10 lecturas parpadeando o no consecutivamente
	  if(bloqueo){
	    if (parpadea !=0 || noParpadea !=0){
	      if(parpadea > 0){//mientras esto ocurra, tendrá que estar parpadeando (apagar,encender) hasta que se cumplan 10 ciclos
	        lcd.noDisplay();
  			delay(100);
  			lcd.display();
	        parpadea --;
	      }
	      if(noParpadea > 0){
	        //lcd.display();
	        noParpadea --;
	      }
	    } else bloqueo = false;
	  }
	  //Cuando se terminan las 10 lecturas,se puede volver a comprobar
	  //que sean correctas para seguir o no parpadeando
	  if (!bloqueo) {
	    if(lectura){//Si los valores leidos son correctos
	      noParpadea = 10;
	      bloqueo = true;
	    }
	    else{
	      parpadea = 10;
	      bloqueo = true;
	    }
	  }
	}
/**
 * Muestra los valores de temperatura (En ºC)
 * y humedad (en %)
 * Precondiciones: Los datos de temperatura y humedad han sido 
 * leídos previamente
 * 
 */
	void ProyectoDU::mostrarHTLCD(){
		lcd.clear();
	 	if(lectura){	
	 	 //Mostrar temperatura
	 	 lcd.setCursor(1,0);
	  	 lcd.print("Temp:" + String(temperatura) + "*C");
	  	 //Mostrar humedad
	  	 lcd.setCursor(2,1);
	  	 lcd.print("Hum: " + String(humedad) + "%");
	 	}else{
	 	 lcd.setCursor(3,0);
	 	 lcd.print("ERROR:");
	 	 lcd.setCursor(1,1);
	 	 lcd.print(mensaje);
	 	 mensaje="";
		}
	}
/**Base de Datos**/
/**Método para la creación de una base de datos "sensor.db"
 * que almacenará la fecha y hora,el Identificador y los datos de temperatura y humedad
 * a cada décima de segundo (100 ms)
 */
	void ProyectoDU::crearBD(){

	   proceso.runShellCommand("sqlite3 /tmp/DHT11/sensor.db 'DROP TABLE MEDICIONES;'");
	   proceso.runShellCommand("mkdir /tmp/DHT11/");
	   //Crear Tabla "Mediciones"
		delay(2000);
	   //Serial.print("Creando Base de Datos...."); 
	   proceso.runShellCommand("sqlite3 /tmp/DHT11/sensor.db 'CREATE TABLE MEDICIONES(ID INTEGER PRIMARY KEY AUTOINCREMENT,FECHA DATETIME NOT NULL,TEMPERATURA REAL NOT NULL,HUMEDAD REAL NOT NULL);'");
	  	
	   //Serial.print("OK!\n");
	}
/*
 * Inserción en Base de Datos
 * Precondiciones: Los datos de temperatura y humedad se insertan de las lecturas
 * del sensor DHT11,por lo que se han tenido que realizar previamente.
 * El identificador se genera automátciamente por la BD y la fecha corresponede a la fecha del sistema,
 * que tambien la calculará la propia BD
 */
	void ProyectoDU::insertarEnBD(){

	 //Serial.print("Insertando valores en la Base de Datos...." );
 	 proceso.runShellCommand("sqlite3 /tmp/DHT11/sensor.db 'INSERT INTO MEDICIONES(FECHA,TEMPERATURA,HUMEDAD)VALUES(CURRENT_TIMESTAMP,"+ String(temperatura)+","+String(humedad)+");'");
     //Serial.print("OK!\n");

	}

/** Función para realizar la configuración inicial de todo el programa.
 *	Los pasos que se realizan son:
 *	1 - Inicializar LCD
 *	2 - Creación de la Base de Datos
 *  3 - Levantar servicio Api Rest
**/
	void ProyectoDU::configuracionInicial(){
		api.configurar();
		inicializarLCD();
  		//proceso.runShellCommand("ntpd -qn -p 0.pool.ntp.org");
		crearBD();
		proceso.runShellCommand("cat /etc/version.txt");
  		if (proceso.available()){
   		 api.actualizarVersion(proceso.parseFloat());//Actualizar la version que se consulta desde la API
  		}
	}
/** Método para llamar a la escucha de clientes de la API-REST
 *  dentro del sketch principal del programa **/
 	void ProyectoDU::escucharClientesAPI(){
 		api.atenderClientes();
 	}
/** Método que realiza la gestión de versiones del programa, llamando al script 
  * encargado de revisar el servidor en busca de actualizaciones. Dicho script se encargará de descargar los
  * ficheros de la última versión (scripts y sketch) a la placa. 
  * Posteriormente, se hará una llamada al script que relanza el sketche.
  *	Este proceso se realizará una vez al día (cada 86400000 ms) **/
 	void ProyectoDU::actualizacionSincronizacion(){
 		//Sincronización
 		//Serial.print("--------Resincronizando dispositivo...");
  		proceso.runShellCommand("ntpd -qn -p 0.pool.ntp.org");
 		//Serial.print("OK!\n");
 		//Búsqueda de actualizaciones
 		//Serial.print("--------Buscando nuevas actualizaciones...");
 		proceso.runShellCommand("perl /etc/script/controlVersiones.pl");
 		//Serial.print("OK!\n");
 		//Actualizar versión en la API
 		
 	}
/**	Función para enviar emails en caso de que haya anomalías en la lectura.
  *	Tambien enviará un email al destinatario cuando se haya solucionado el problema y la lectura 
  *	vuelva a ser correcta
**/
 	void ProyectoDU::enviarEmail(bool correcto){
 		if (correcto) proceso.runShellCommand("echo -e 'Subject: Problema solucionado. \r\n\r\n Las lecturas vuelven a ser correctas.' | sendmail sezabalam@alumnos.unex.es");
 		else proceso.runShellCommand("echo -e 'Subject: Problema en la lectura:  \r\n\r\n Revise el dispositivo y los niveles de la sala.' | sendmail sezabalam@alumnos.unex.es");

 	}
 	/*void ProyectoDU::enviarEmailNO_OK(){
 		proceso.runShellCommand("echo -e 'Subject: Problema en la lectura. \r\n\r\n"+mensaje+" - Revise el dispositivo y los niveles de la sala.' | sendmail sezabalam@alumnos.unex.es");

 	}*/

/** Envío de los valores al broker IOT de Xively. Esta función se llamará cada 2 minutos **/
 	void ProyectoDU::envioABroker(){
 		proceso.runShellCommand("/bin/sh /etc/script/mosquitto/mosquitto_mqtt.sh " + String(temperatura) + " " + String(humedad));
 	}
 		void ProyectoDU::envioError(){
 		proceso.runShellCommand("/bin/sh /etc/script/mosquitto/error_mqtt.sh '"+String(mensaje)+"'");
 	}
 	
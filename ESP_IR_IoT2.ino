#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DS1820.h>
#include <IRremoteESP8266.h>
#include <SimpleDHT.h>

/*----------------------------Board (Hardware Version) -----------------------------------------*/
#define _BOARD_V1_
//#define _BOARD_V2_
//#define IR_RECEIVER_ 7
//#define IR_TO_HMI_ 7
//#define _DEBUG
#define _DHT11_
//#define _LOCAL_HMI_

/* General vars for setup and config --------------------------------------------------------*/

#define _NUM_ATENTS 30              // N Atents before start AP Mode
#define _INTERVAL_SEND_T1 59        // Time in s before send info    
#define _IND_INTERVAL1 10           // Interval before reset IND 
#define _INTERVAL_SEND_T2 90        // Time in s before send info    
#define _IND_INTERVAL2 10           // Interval before reset IND 
#define _CHAVE "kjfsdklas"          // Security key  
#define _RESET_AP 5                 // Number of minutes to operate in AP mode before reset 
#define CSS_FLAG 1                  //Flag to define if/or not use off CSS 
#define _WebClient1 _GlobalFlags[4] //WebClient1 is stored in Global Flag #4   
#define _WebClient2 _GlobalFlags[5] //WebClient2 is stored in Global Flag #5   
#define _Device_Id  _GlobalFlags[1] //_Device_Id is stored in Global Flag #1  
#define _ALARM_NUMBER 5             //Number off available alarms
#define _ALARM_EVENT  10            //Number off minutes before repeat alarm  
//Neneral flags ------------------------------------------------------------------------
bool APFLAG = 0; // Flag to enter AP mode
bool PWFLAG = 0; // Flag to ask for password

//Time vars in seconds-----------------------------------------------------------------
//Loop 1
unsigned long _timeNow1 = 0;
unsigned long _timeLast1 = 0;
int _seconds1 = 0; 
int _ind1=0;
//Loop 2
unsigned long _timeNow2 = 0;
unsigned long _timeLast2 = 0;
int _seconds2 = 0; 
int _ind2=0;

//global vars -------------------------------------------------------------------------
unsigned long var_test;
float room_temperature=0.0;
int room_humidity=0;

//Login and Password for protection from external access--
char _LoginPass[15];
char _GlobalFlags[15];
char _ssid[15],_password[15];
String _ipn3,_ipn4,_gtn3,_gtn4;
  
/*-------------------------- GPIO OBJECTS -----------------------------------------------------*/
/*                           BOARD_V1                                                          */
/*---------------------------------------------------------------------------------------------*/
#ifdef _BOARD_V1_
      #define _FIRMWARE_VER "<p>Fw 1.42c - EN - Hw V1 </p>" 
      #define _BRAND_ "<H2>IoT Tecnologies By 2AC - Hw V1</H2><br>"
      
      IRsend irsend(5); //IR emitter led on pin D1 (100R to GND)----- GPIO 5
      
      #ifdef _DHT11_ 
          #define pinDHT11 4  //DHT11 on pin D2 --------------------  GPIO 4
          SimpleDHT11 dht11;      
      #else 
          Ds1820Read ds(4); //DS18S20P on pin D2 (R=4.7K to 3.3VCC)-  GPIO 4
      #endif     
      
      byte _gpio[]  =    {16, 5, 4, 0, 2,14,12,13,15};     // IO Adress
      byte _reserved[] = { 0, 1, 1, 1, 0, 0, 0, 1, 0};     // Reserved IO
#endif      
/*-------------------------- GPIO OBJECTS -----------------------------------------------------*/
/*                           BOARD_V2                                                          */
/*---------------------------------------------------------------------------------------------*/
#ifdef _BOARD_V2_
      #define _FIRMWARE_VER "<p>Fw 1.42c - EN - Hw V2 </p>" 
      #define _BRAND_ "<H2>IoT Tecnologies By 2AC - Hw V2 </H2><br>"
      #define RELE 5    //RELE - Pin D1 - GPIO 5
       
      IRsend irsend(12); //IR emitter led on pin D6 (100R to GND)-- GPIO 12
      
      #ifdef _DHT11_ 
          #define pinDHT11 4  //DHT11 on pin D2 -------------------  GPIO 4
          SimpleDHT11 dht11;      
      #else 
          Ds1820Read ds(4); //DS18S20P on pin D2 (R=4.7K to 3.3VCC)-  GPIO 4
      #endif    
      
      byte _gpio[]  =    {16, 5, 4, 0, 2,14,12,13,15};     // IO Adress
      byte _reserved[] = { 0, 0, 1, 1, 0, 0, 1, 1, 0};     // Reserved IO
#endif      
//Alarm Vars and gpio on vars --------------------------------------------------------
byte _gpio_on[15];                                  // GPIO On and off   
byte _alarm_on[_ALARM_NUMBER];                      // Alarms to handle 
byte _alarm_cfg[_ALARM_NUMBER];                     // Alarm parameters
byte _alarm_srv[_ALARM_NUMBER];                     // Alarm server
byte _alarm_repeat[_ALARM_NUMBER];                  // Alarm Repeat after event.
//Web Server on port 80 --------------------------------------------------------------
ESP8266WebServer server(80);

// NETWORK: Details for Web Client Connection-----------------------------------------
  char _webs1[30];    // Host / Server
  char _webp1[30];    // Web Page
  int _porta1;        // Port 

  char _webs2[30];    // Host / Server
  char _webp2[30];    // Web Page
  int _porta2;        // Port 
// String for Update On Air (OTA) ----------------------------------------------------  
  const char* _serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
  
// NETWORK: Static IP details for server...-------------------------------------------
IPAddress _ip(192, 168, 1, 50);            // Static IP adress
IPAddress _gateway(192, 168, 1, 1);        // Static GateWay 
IPAddress _subnet(255, 255, 255, 0);       // Static NetMasc 

/*-------------------------Infrared works if included in software --------------------------------*/
#ifdef IR_RECEIVER_
    #include <IRremoteESP8266.h>

    IRrecv irrecv(_gpio[IR_RECEIVER_]);
    byte _irr_flag_;      
    #define _PIN_IR_RECEIVER IR_RECEIVER_
    
#else
    #define _PIN_IR_RECEIVER 100
#endif    
/*-------------------------Infrared to HMI CONSOLE ----------- --------------------------------*/
#ifdef IR_TO_HMI_
    #include <IRremoteESP8266.h>

    IRrecv irrecv(_gpio[IR_TO_HMI_]);
    byte _irr_flag_;      
    #define _PIN_IR_RECEIVER IR_TO_HMI_
#endif    

// Setup - Runs only one time at startup  -----------------------------------------------------------------------------------------------------
void setup(void){
  
   var_test=0; //variavel global
  
  //Start Serial port (just for debug)
  Serial.begin(9600);
  //Prepare EPROM
  EEPROM.begin(512);

  //read ssid password and other data from eeprom
  for(int i=0;i<15;i++){
    _ssid[i]= (char)EEPROM.read(i);
    _password[i] = (char)EEPROM.read(i+15);
    _LoginPass[i] = (char)EEPROM.read(i+30);
    _GlobalFlags[i]= (char)EEPROM.read(i+45);
    _gpio_on[i]=(byte) EEPROM.read(i+60);
  }
  
  //Read WebClient Configuration
  for(int i=0;i<30;i++){
    _webs1[i]=  (char)EEPROM.read(i+75);     // 75 -> 104
    _webp1[i]=  (char)EEPROM.read(i+105);    //105 ->  135
    _webs2[i]=  (char)EEPROM.read(i+140);    //140 -> 174
    _webp2[i]=  (char)EEPROM.read(i+175);    //175 -> 204  
    }
   
  char apoio1[5];
  char apoio2[5];
  
  //Port number sored in text format
  
  for(int i=0;i<5;i++){
    apoio1[i]=(char)EEPROM.read(i+135);        //135 ->139 
    apoio2[i]=(char)EEPROM.read(i+205);        //205 ->209 

    _alarm_on[i]=(char)EEPROM.read(i+210);     // 210 -> 214
    _alarm_cfg[i]=(char)EEPROM.read(i+215);    // 215 -> 220
    _alarm_srv[i]=(char)EEPROM.read(i+220);    // 220 ->225
    
    }
    
   _porta1=atoi(apoio1); 
   _porta2=atoi(apoio2);


   #ifdef _DEBUG
    Serial.println("Hello from IoT");
    Serial.println("");
    Serial.println(_ssid); 
    Serial.println(_password); 
    Serial.println(_LoginPass); 
    Serial.print("192.168.");   
    Serial.print(_GlobalFlags[0],DEC);
    Serial.print(".");
    Serial.println(_GlobalFlags[1],DEC);
    Serial.print("192.168.");      
    Serial.print(_GlobalFlags[2],DEC);
    Serial.print(".");
    Serial.println(_GlobalFlags[3],DEC);
   #endif //DEBUG
   
   _ipn3=(int)_GlobalFlags[0];
   _ipn4=(int)_GlobalFlags[1];
   _gtn3=(int)_GlobalFlags[2];
   _gtn4=(int)_GlobalFlags[3];
   
  Serial.print("PORT:");
  Serial.println(_porta1);

  //Set new IP and GateWay
 IPAddress _ip(192, 168,(byte)_GlobalFlags[0],(byte)_GlobalFlags[1]);       // Static IP adress
 IPAddress _gateway(192, 168,(byte)_GlobalFlags[2],(byte)_GlobalFlags[3]);  // Static GateWay  
            
  // prepare GPIO (D1, D2, D3, D4, D8)
  for(int i=0;i<9;i++){
    if(i != _PIN_IR_RECEIVER){
      pinMode(_gpio[i], OUTPUT); 
      digitalWrite(_gpio[i], 0); 
    }
  }

      
   //Prepare WI-FI DHCP mode 
  WiFi.mode(WIFI_STA); 
  // Static IP Setup Info Here...
  WiFi.begin(_ssid, _password);
  Serial.println("");
  // Wait for connection
  int ct=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    ct++;
    if(ct>_NUM_ATENTS) {APFLAG=1; break;}
  }
if(APFLAG==0){
  WiFi.config(_ip, _gateway, _subnet);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }

else{
    WiFi.mode(WIFI_AP); 
    WiFi.softAP("IotACO","1111122222");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
 }

 
  #ifdef IR_RECEIVER_
     _irr_flag_=0;             
      irrecv.enableIRIn();    // Start the receiver
      Serial.println("Starting IR Receiver");
  #endif

   #ifdef IR_TO_HMI_
     _irr_flag_=0;             
      irrecv.enableIRIn();    // Start the receiver
      Serial.println("Starting IR Receiver HMI mode");
  #endif

  server.on("/", handleRoot);                   //Root -> Main loop
  server.on("/login", handleLogin);             //Handle Login
  server.on("/config", handleChangeCfg);        //Change config
  server.on("/storeconf", handleStoreCfg);      //Store Configutatios
  server.on("/changepass",handleChangepass);    //Change Passcode
  server.on("/storepass",handleStorepass);      //Store Passcode
  server.on("/t",handleRoomTemperature);        //Show temperature - static
  server.on("/t2",handleRoomTemperature2);      //Show temperature - refresh
  server.on("/misc",handleMisc);                //Show misc information
  server.on("/webclient",handleChangWebClient); //Change Web Client Config
  server.on("/storewc",handleStoreWebClient);   //Store WebClient
  server.on("/alarm",handleAlarm);              //Change alarm configuration
  server.on("/storealarm",handleStoreAlarm);    //Store alarm configurations
  server.on("/hmir1",handlehmir1);              //Receive data from hosts      
  //--------------------------------------------------- START UPDATE CODE -----------------------------------------------  
  server.on("/up", HTTP_GET, [](){
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", _serverIndex);
    });

  server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });  
    
  //--------------------------------------------------- END UPDATE CODE -----------------------------------------------

  

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("HTTP server started");
}
/*Loop - Loops all the time ---------------------------------------------------------------------------------------------------------------*/
void loop(void){

  //Handle WebServer ------------------------------------------------------------------------------------------------------------------------
server.handleClient();

 //Handle Sensor reading --------------------------------------------------------------------------------------------------------------------
 var_test++;
 if (var_test > 50000){
  


    #ifdef _DHT11_ 
        byte temperature = 0;
        byte humidity = 0;
        int auxt;
        if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
            Serial.println("Read DHT11 failed.");
        }else{
            auxt=(int)temperature;
            room_temperature=(float)(temperature*1.0);
            room_humidity=(int)humidity;
        }
   #else 
        ds.Ds1820GetTemp();
        room_temperature=ds.celsius;
   #endif
  
  var_test=1;
  }
 //Handle Time1 -------------------------------------------------------------------------------------------------------------------------------
 _timeNow1 = millis()/1000; // the number of milliseconds that have passed since boot
 _seconds1 = _timeNow1 - _timeLast1;
  if (_seconds1 > _INTERVAL_SEND_T1) {
    _timeLast1 = _timeNow1;
    _ind1++;
    if(_ind1 > _IND_INTERVAL1) _ind1=0; 

    /*Reset ESP if in AP mode after #seconds*/
    if( (_ind1 > _RESET_AP) && ( APFLAG != 0) ){  //Reset AP is only availlable in this loop
      ESP.reset();   
    }
    
    /*Send data to WebServer 1*/ 
    if(_WebClient1==1){
      SendInfo(1,_webs1,_webp1,_porta1,_ind1);  
      }

     /*...Handle Alarm Counter....*/ 
     HandleAlarmCounter();
    
   }
 //Handle Timer2 -------------------------------------------------------------------------------------------------------------------------------
 _timeNow2 = millis()/1000; // the number of milliseconds that have passed since boot
 _seconds2 = _timeNow2 - _timeLast2;
  if (_seconds2 > _INTERVAL_SEND_T2) {
    _timeLast2 = _timeNow2;
    _ind2++;
    if(_ind2 > _IND_INTERVAL2) _ind2=0; 
    

    /*Send data to WebServer 2*/ 
    if(_WebClient2==1){
      SendInfo(1,_webs2,_webp2,_porta2,_ind2);  
      }
     
   }
//Handle Alarms -------------------------------------------------------------------------------------------------------------------------------   
     HandleAllAlarms();  

//Handle Infra Red Receiver -------------------------------------------------------------------------------------------------------------------
#ifdef IR_RECEIVER_ 
  decode_results results; // Somewhere to store the results

  if (irrecv.decode(&results)) { // Grab an IR code
     
     byte irr_flag= Ir__Receiver_Decode(&results);
     
     if(irr_flag){      
        for(int i=0;i<9;i++){
          if( _gpio_on[i] == 1 ){             //If the output is enabled
             byte io = irr_flag & 0xF0;       //Get the IO port number from
             io = (io >> 4);                  //Code
             io --;
             byte op = (irr_flag & 0x0F) -1;
             digitalWrite(_gpio[io], op);          
          }
        }
     }   
    irrecv.resume(); // Prepare for the next value
    }    
#endif    
//Handle Infra Red receiver for HMI Conole -----------------------------------------------------------------------------------------------------
#ifdef IR_TO_HMI_
   decode_results results; // Somewhere to store the results
   
  if (irrecv.decode(&results)) { // Grab an IR code
    Ir__Receiver_Decode2(&results);
    irrecv.resume(); // Prepare for the next value     
    }    
#endif
//Handle data from serial Port  ----------------------------------------------------------------------------------------------------------------
handlehmis1();           
}  
/******************************************************************** END *********************************************************************/

/*
 *  This sketch sends data via HTTP GET requests to http://abelpi:4080/tempr.php.
 *    char* _host;                //Default Value= "abelpi";
 *    char* _webpage;             //Default Value= "/tempr.php/";
 *    int _httpPort;              //Default Value= 4080;
 *
 */

void SendInfo(byte OpReturn,char *host, char *webpage, int httpPort, int ind) {
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = webpage;
  url += "?temp=";
  url += room_temperature;
  url += "&hum=";
  url += room_humidity;
  url += "&id=";
  url += ind;
  url += "&device=";
  url += (byte)_Device_Id;  
  url += "&chave=";
  url += _CHAVE;
  
  
  //Serial.print("Requesting URL: ");
  //Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print the last one to Serial
  String line; 
  while(client.available()){
    line = client.readStringUntil('\r');
       #ifdef _DEBUG
        Serial.println(line);  
       #endif 
  }
    Serial.print(line);  

  //Runs Operation from server:
  if (OpReturn == 1){  SendCommand(line);  }
    
  //Serial.println();
  //Serial.println("closing connection");
}

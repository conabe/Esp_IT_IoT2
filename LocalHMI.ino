/*--------------------------------------------------------------------------------------------
 * HMI Interface whith Hosts (For now uses a defined list)
 * Send and receive data from Hosts
 * 
 --------------------------------------------------------------------------------------------*/
#ifdef _LOCAL_HMI_

/*Send HMI Order ----------------------------------------------------------------------------*/
void SendHmiOrder(char *host,char *order) {
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/";
  url += "?pin=";
  url += order;

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
  }
  //Serial.println();
  //Serial.println("closing connection");
}
/*Send data to Hosts-------------------------------------------------------------------------*/
void handlehmis1(void)
{
    while (Serial.available() > 0) {                                 // if there's any serial available, read it:
    int id = Serial.parseInt();                                      // look for the next valid integer in the incoming serial stream:
    int value = Serial.parseInt();                                   // do it again:
    if (Serial.read() == '\n') {                                     // look for the newline.
    if((value > 1) && (value <9) && (id >0)  && (id<99) ){ 
            Serial.println(id, DEC);                  
            Serial.println(value, DEC); 
            String str="192.168.1.";
            str+=id;
            String order;
            switch (value) {
                case 3:
                  order="LGOn24H";
                  break;
                case 4:
                  order="LGOn18C";
                  break;
                case 5:
                  order="OFF_LG";
                  break;
                case 6:
                  order="OFF4";
                  break;    
                case 7:
                  order="ON4";
                  break;                                    
              }
            //Serial.println(str);              
            //Serial.println(order);
            SendHmiOrder(&str[0],&order[0]);
          } 
        }
   } 
}
/*Receive data from Hosts -------------------------------------------------------------------*/
void handlehmir1()
{
  String device,value;
  String message;

     
  if (server.hasArg("device") && server.hasArg("temp")){

    device=server.arg("device");
    value=server.arg("temp");
    Serial.print(device);
    Serial.print(",");
    value.replace(".", "");
    Serial.print(value);
    Serial.print("\n");
    message="Ok";
  }else{
      message="KO";
  }
    
  server.send(404, "text/plain", message);  
}
/*----------------------------------------NOT DEFINE -------------------------------------------*/
#else
void handlehmis1(void)
{
return;  
}
void handlehmir1()
{
handleNotFound(); 
}
#endif

/*Send IR Comand to HMI  -------------------------------------------------------------------*/
#ifdef IR_TO_HMI_ 

void Ir__Receiver_Decode2 (decode_results *results)
{

byte irr_flag;
switch (results->value) {
  
 case 0xF9708F: irr_flag=11; break; //Right
 case 0xF9609F: irr_flag =10; break;  //Left

 case 0xF98877: irr_flag=0x00;  break;
 case 0xF9A05F: irr_flag=0x01;  break;
 case 0xF9906F: irr_flag=0x02;  break;
 case 0xF9A857: irr_flag=0x03;  break;
 case 0xF920DF: irr_flag=0x04;  break;
 case 0xF910EF: irr_flag=0x05;  break;
 case 0xF928D7: irr_flag=0x06;  break;
 case 0xF900FF: irr_flag=0x07;  break;
 case 0xF908F7: irr_flag=0x08;  break;
 case 0xF930CF: irr_flag=0x09;  break;
 default:       irr_flag=0x99;  break;
 }

if(irr_flag!=0x99) {
    Serial.print(11);
    Serial.print(",");
    Serial.print(irr_flag);
    Serial.print("\n");
    }
}
#endif
/****************************************** END **********************************************/
/*
 *Comando Grande  
 case 0xF9708F: irr_flag=11; break; //Right
 case 0xF9609F: irr_flag =10; break;  //Left

 case 0xF98877: irr_flag=0x00;  break;
 case 0xF9A05F: irr_flag=0x01;  break;
 case 0xF9906F: irr_flag=0x02;  break;
 case 0xF9A857: irr_flag=0x03;  break;
 case 0xF920DF: irr_flag=0x04;  break;
 case 0xF910EF: irr_flag=0x05;  break;
 case 0xF928D7: irr_flag=0x06;  break;
 case 0xF900FF: irr_flag=0x07;  break;
 case 0xF908F7: irr_flag=0x08;  break;
 case 0xF930CF: irr_flag=0x09;  break;
 default:       irr_flag=0x99;  break;
 */
 /*
  * Comando Pequeno

 case 0xFEA05F : irr_flag=11; break; //Right
 case 0xFEE01F: irr_flag =10; break;  //Left

 case 0xFE708F: irr_flag=0x00;  break;
 case 0xFEE817: irr_flag=0x01;  break;
 case 0xFE6897: irr_flag=0x02;  break;
 case 0xF9A857: irr_flag=0x03;  break;
 case 0xFE28D7: irr_flag=0x04;  break;
 case 0xFEC837: irr_flag=0x05;  break;
 case 0xFE48B7: irr_flag=0x06;  break;
 case 0xFE8877: irr_flag=0x07;  break;
 case 0xFE08F7: irr_flag=0x08;  break;
 case 0xFEF00F: irr_flag=0x09;  break;
 default:       irr_flag=0x99;  break;

  */


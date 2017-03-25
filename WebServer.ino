/* *********************************************************************************************************************************************
 * WebPages for all functions --> Handle*** take care of the most of functions
 ***********************************************************************************************************************************************/
 /*Rotina Para gerir alarmes ------------------------------------------------------------------------------------------------------------------*/
void handleAlarm(){
  String content = "<html>";

  if(CSS_FLAG){content += cssstyle();}
  
  content += "<h2>Alarmes</h2>"; 
  content += "<body><form action='/storealarm' method='POST'>";

  /*Create 5 entry to config IO and Temp Alarms*/
    for(int i=0;i<_ALARM_NUMBER;i++){
      /*Intert Alarm number and Type*/
      content += F("Alarme #");
      content.concat(i);   
      content += F("<select name='alarm");
      content.concat(i);    
      content += "'>";
      content += F("<option value='OFF'>OFF</option>");
      content += F("<option value='HI' ");
      if(_alarm_on[i]==1){
        content += F("selected");
      } 
      content +=F(">Input Hi</option>");   
      content += F("<option value='LO' ");
      if(_alarm_on[i]==2){
        content += F("selected");
      } 
      content +=F(">Input Lo</option>");          
      content += F("<option value='TPH' ");
      if(_alarm_on[i]==3){
        content += F("selected");
      } 
      content += F(">Temperatura superior</option>");      
      content += F("<option value='TPL' ");
      if(_alarm_on[i]==4){
        content += F("selected");
      } 
      content += F(">Temperatura inferior</option>");  
      content += F("</select>");
      /*Intert Alarm Parameter*/
     content += F("<input type='text' class='c2' name='cfg");
     content.concat(i);           
     content +="' VALUE='";
     content +=_alarm_cfg[i];
     content +="'>"; 
      /*Intert Alarm server*/ 
      content += F("<select name='srv");
      content.concat(i);    
      content += "'>";
      content += F("<option value='1'>1</option>");     
      content += F("<option value='2' ");
      if(_alarm_srv[i]==1){
        content += F("selected");
      } 
      content += F(">2</option>");      
      content += F("<option value='12' ");
      if(_alarm_srv[i]==2){
        content += F("selected");
      } 
      content += F(">1 and 2</option>");  
      content += "</select><br>";     
  }

  content += F("<p><input type='submit'  class='b1' value='submit'></form>" "</p>");

  for(int i=0;i<_ALARM_NUMBER;i++){
    if(_alarm_repeat[i] != 0){
      content.concat(i);
    }
  }
  content += "<br><a href='/'>Return</a><br>";
       
  server.send(200, "text/html", content);
}

/*Rotina Para gravar configuracao de alarmes ---------------------------------------------------------------------------------------------------------*/
void handleStoreAlarm(){
  int i;

  String alarm,cfg,srv,aux;

    for(i=0;i<_ALARM_NUMBER;i++){
      alarm="alarm";
      cfg="cfg";
      srv="srv";
      alarm.concat(i);
      cfg.concat(i);
      srv.concat(i);
      /*Intert Alarm number and Type*/
      if(server.arg(alarm)=="HI"){
        _alarm_on[i]=1;
      }else if(server.arg(alarm)=="LO"){
        _alarm_on[i]=2;
      }else if(server.arg(alarm)=="TPH"){
        _alarm_on[i]=3;
      }else if(server.arg(alarm)=="TPL"){
        _alarm_on[i]=4;        
      }else{
        _alarm_on[i]=0;
      }
      /*Intert Alarm server*/ 
       if(server.arg(srv)=="2"){
        _alarm_srv[i]=1;
      }else if(server.arg(srv)=="12"){
        _alarm_srv[i]=2;
      }else{
        _alarm_srv[i]=0;
      }     
       /*Intert Alarm Parameter*/     
       aux=server.arg(cfg); 
       _alarm_cfg[i]=(byte)aux.toInt();

       /*Check Errors and save to Eprom*/ 
       if(checkIOAlarms(i)==0){
          EEPROM.put(i+210, _alarm_on[i]); 
          EEPROM.put(i+215, _alarm_cfg[i]); 
          EEPROM.put(i+220, _alarm_srv[i]);        
          }
    }

    

  EEPROM.commit();
  String content = F("<html><body>Done Saving. <br>");
  content += F("Can return to operation <a href='/'>here</a> or switch power off</body></html>");
  server.send(200, "text/html", content);  
    
}
//Check if header is present and correct  --------------------------------------------------------------------------------------------------------
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect --------------------------------------------------------------------------------------------------------
void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == _LoginPass ){
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }
  String content = F("<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>");
  content += F("User:<input type='text' name='USERNAME'><br>");
  content += F("Pass:<input type='password' name='PASSWORD'><br>");
  content += F("<input type='submit' name='SUBMIT' value='Submit'></form>");
  content += msg;
  content+= F("<br></body></html>");
  server.send(200, "text/html", content);
}
//no need authentification  --------------------------------------------------------------------------------------------------------------------
void handleNotFound(){
  String message = "Handle not Fount IoT Tecnologies";
  server.send(404, "text/plain", message);
}

/*Rotina Para gerir o novo WEP e SSID e outras configuracoes -----------------------------------------------------------------------------------*/
void handleChangeCfg(){
  String content = "<html>";

  if(CSS_FLAG){content += cssstyle();}
  
  content += F("<h2>Configuracoes</h2>"); 
  content += F("<body><form action='/storeconf' method='POST'>");
  content += F("<p>SSID   :<input type='text' name='SSID' VALUE='");
  content += _ssid;
  content += F("'></p><p>PWEP   :<input type='text' name='WEP_PASS' VALUE='");
  content += _password;
  content += F("'></p><p>IP: 192.168.<input type='text' class='c2' name='IPN3' VALUE='");
  content += _ipn3;
  content += F("'>.<input type='text' class='c2' name='IPN4' VALUE='");
  content +=_ipn4;
  content += F("'></p><p>Gateway:192.168.<input type='text' class='c2' name='GTN3' VALUE='");
  content +=_gtn3;
  content += F("'>.<input type='text' class='c2' name='GTN4' VALUE='");
  content +=_gtn4;
  content +="'></p>";
  content += F("<p><input type='checkbox' name='extras' value='PassOn'> Autentication On</p>");

  /*Create Select entry to config IO*/
    for(int i=0;i<9;i++){
      if(_reserved[i]==0){
      content += "Gpio ";
      content.concat(i);   
      content += "<select name='gpio";
      content.concat(i);    
      content += "'>";
      content += "<option value='OFF'>NC</option>";
      content += "<option value='ON' ";
      if(_gpio_on[i]==1){
        content += "selected";
      } 
      content +=">Output</option>";     
      content += "<option value='IN' ";
      if(_gpio_on[i]==2){
        content += "selected";
      } 
      content += ">Input</option>";      
 
      content += "</select><br>";
      }
  }

  content += F("<p><input type='submit'  class='b1' value='submit'></form>" "</p>");

  content += F("<a href='/'>Return</a><br>");
  content += F("<a href='/t2'>Temperatura</a><br>");  
  content += F("<a href='/webclient'>Web Client</a><br>");
  content += F("<a href='/changepass'>Change Pass Word</a><br>");
  content += F("<a href='/alarm'>Alarmes</a><br>");  
  content += F("<a href='/misc'>Diversos</a><br>");
       
       
  server.send(200, "text/html", content);
}

/*Rotina Para gravar o novo WEP e SSID,IP, etc..  -------------------------------------------------------------------------------------------------------*/
void handleStoreCfg(){
  int i;
  String ssid,password;
  String gpio;

  
  if (server.hasArg("SSID") && server.hasArg("WEP_PASS")&& server.hasArg("IPN4")&& server.hasArg("IPN3")&& server.hasArg("GTN3")&& server.hasArg("GTN4")){
    
    ssid=server.arg("SSID");
    password=server.arg("WEP_PASS");
    _ipn3=server.arg("IPN3");
    _ipn4=server.arg("IPN4");
    _gtn3=server.arg("GTN3");
    _gtn4=server.arg("GTN4");

    for(i=0;i<9;i++){
      gpio="gpio";
      gpio.concat(i);
      if(server.arg(gpio)=="ON"){
        _gpio_on[i]=1;
      }else if(server.arg(gpio)=="IN"){
        _gpio_on[i]=2;
      }
      else{
        _gpio_on[i]=0;
      }

    }
    
    EEPROM.put(45,(byte)_ipn3.toInt());
    EEPROM.put(46,(byte)_ipn4.toInt());
    EEPROM.put(47,(byte)_gtn3.toInt());
    EEPROM.put(48,(byte)_gtn4.toInt());
    
    for(i=0;i<15;i++){
       EEPROM.put(i, ssid[i]); 
       _ssid[i]=ssid[i];
       EEPROM.put(i+15, password[i]);
       _password[i]=password[i];        
       EEPROM.put(i+60, _gpio_on[i]);
    }
    Serial.println(_ssid);
    Serial.println(_password); 
  }
  EEPROM.commit();
  String content = "<html><body>Done Saving. <br>";
  content += "Can return to operation <a href='/'>here</a> or switch power off</body></html>";
  server.send(200, "text/html", content);  
    
}
//Rotina para gerir a alteracao de password  ----------------------------------------------------------------------------------------------------
void handleChangepass(){

  String content = "<html>";

  if(CSS_FLAG){content += cssstyle();}
  
  content += F("<H2>Alterar Password</H2> <br>");
  content += F("<form action='/storepass' method='POST'>");
  content += F("<p>OLD_PASS :<input type='password' name='OLD_PASS'></p>");
  content += F("<p>NEW_PASS1:<input type='password' name='NEW_PASS1'></p>");
  content += F("<p>NEW_PASS2:<input type='password' name='NEW_PASS2'></p>");
  content += F("<p><input type='submit' class='b1'  value='submit'></form>" "</p>");
  server.send(200, "text/html", content);
}

//Rotina Para gravar o novo PassWord  --------------------------------------------------------------------------------------------------------
void handleStorepass(){
  int i;
  String old_pass,new_pass1,new_pass2;
  bool flag=0;
  
  if (server.hasArg("OLD_PASS") && server.hasArg("NEW_PASS1") && server.hasArg("NEW_PASS2")){
    if( (server.arg("OLD_PASS") ==  _LoginPass) && (server.arg("NEW_PASS1") == server.arg("NEW_PASS2"))){
      old_pass=server.arg("OLD_PASS");
      new_pass1=server.arg("NEW_PASS1");
      new_pass2=server.arg("NEW_PASS2");    

      Serial.println(old_pass);
      Serial.println(new_pass1); 
      Serial.println(new_pass2);     
    
      for(i=0;i<15;i++){
        EEPROM.put(i+30, new_pass1[i]);  
        _LoginPass[i]=(char) new_pass1[i];       
        }
       
      EEPROM.commit();
      String content = "<html><body>Sucess!!!<br>";
      content += "Can return to operation <a href='/'>here</a> </body></html>";
      server.send(200, "text/html", content);  
      flag=1;
    }
  }
if(flag==0){
  String content = F("<html><body>Please verify old pass or confirmation <br>");
  content += F("Can return to operation <a href='/'>here</a> </body></html>");
  server.send(200, "text/html", content);   
  }
    
}

// Rotina para devolver a temperatura --------------------------------------------------------------------------------------------------------
void handleRoomTemperature()
{
  String content = " ";
  content.concat(room_temperature);
  server.send(200, "text/html", content);
}
// Rotina para devolver a temperatura com auto-refresh ---------------------------------------------------------------------------------------
void handleRoomTemperature2()
{
  String content = "<html> <head> <meta http-equiv='refresh' content='30'/></head>";
  content.concat(room_temperature);
  content +="C ";
  content.concat(room_humidity);
  content +="%";
  server.send(200, "text/html", content);
}
// Show misc information about system -----------------------------------------------------------------------------------------------------
void handleMisc()
{
  String content = "<html><body>";
  content +=_BRAND_ ;
  content += WiFi.localIP().toString();
  content += "<br>SID:";
  content += _ssid; 
  content += "<br>WEP:"; 
  content += _password; 
  content += "<br>IP:192.168.";   
  content += (int)_GlobalFlags[0];
  content += ".";
  content += (int)_GlobalFlags[1];
  content += "<br>Gateway:192.168.";      
  content += (int)_GlobalFlags[2];
  content += ".";
  content += (int)_GlobalFlags[3];
  content += "<br>NETMASK:";
  content += _subnet.toString();
  content +="<br>";
  content += __DATE__;
  content +="<br>"; 
  content += __TIME__;
  content +=_FIRMWARE_VER;
  content += F("<br>Can return to operation <a href='/'>here</a> </body></html>");
  server.send(200, "text/html", content);
}

//Rotina para gerir a alteracao do WebClient ----------------------------------------------------------------------------------------------------
void handleChangWebClient(){

  String content = "<html>";

  if(CSS_FLAG){content += cssstyle();}
  
  content += "<H2>Alterar Web Client</H2> <br>";
  //Client 1 ----------------------------------------------------------------
  content += "<form action='/storewc' method='POST'>";
  content += "<p>WebClient :<input type='text' name='WEBS1' value='";
  content += _webs1; 
  content += "'></p><p>WebPage:<input type='text' name='WEBP1' value='";
  content += _webp1;   
  content += "'></p><p>Porta:<input type='text' name='PORTA1' value='";
  content.concat(_porta1);  
  content += "'><p><input type='checkbox' name='WCON1' value='ON' "; 
  if(_WebClient1==1){
        content += "checked";
      } 
  content += "> Web Client ON";
//Client 2 ----------------------------------------------------------------
  content += "<p>WebClient :<input type='text' name='WEBS2' value='";
  content += _webs2; 
  content += "'></p><p>WebPage:<input type='text' name='WEBP2' value='";
  content += _webp2;   
  content += "'></p><p>Porta:<input type='text' name='PORTA2' value='";
  content.concat(_porta2);  
  content += "'><p><input type='checkbox' name='WCON2' value='ON' "; 
  if(_WebClient2==1){
        content += "checked";
      } 
  content += "> Web Client ON";


  
  content += "</p><p><input type='submit' class='b1'  value='submit'></form>" "</p>";
  server.send(200, "text/html", content);
}

//Rotina Para gravar o novo WebClient  --------------------------------------------------------------------------------------------------------
void handleStoreWebClient(){
  int i;
  String webs1,webp1,porta1,webs2,webp2,porta2;
  int iporta1,iporta2;
  bool flag=0;
  
  if (server.hasArg("WEBS1") && server.hasArg("WEBP1") && server.hasArg("PORTA1")){
      webs1=server.arg("WEBS1");
      webp1=server.arg("WEBP1");
      porta1=server.arg("PORTA1");    

      webs2=server.arg("WEBS2");
      webp2=server.arg("WEBP2");
      porta2=server.arg("PORTA2");         

      Serial.println(webs1);
      Serial.println(webp1); 
      Serial.println(porta1);     
      _porta1=porta1.toInt();
      
      Serial.println(webs2);
      Serial.println(webp2); 
      Serial.println(porta2);     
      _porta2=porta2.toInt();      
    
      for(i=0;i<30;i++){
        EEPROM.put(i+75, webs1[i]);   //from 75 to 104
        EEPROM.put(i+105, webp1[i]);  //from 105 to 134
        EEPROM.put(i+140, webs2[i]);  //from 140 -> 174
        EEPROM.put(i+175, webp2[i]);  //from 175 -> 204       
        _webs1[i]=webs1[i];
        _webp1[i]=webp1[i];     
        _webs2[i]=webs2[i];
        _webp2[i]=webp2[i];    
           
        }
        
      for(i=0;i<5;i++){
        EEPROM.put(135+i, porta1[i]);    //from 135 to 139
        EEPROM.put(205+i, porta2[i]);    //from 205 to 209        
        }

      if(server.arg("WCON1")=="ON"){
        _WebClient1=1;
        }else{
        _WebClient1=0;
        }
        
      if(server.arg("WCON2")=="ON"){
        _WebClient2=1;
        }else{
        _WebClient2=0;
        }        
        
      EEPROM.put(49,_WebClient1);    //FLAG
      EEPROM.put(50,_WebClient2);    //FLAG    
        
      EEPROM.commit();
      String content = "<html><body>Sucess!!!<br>";
      content += "Can return to operation <a href='/'>here</a> </body></html>";
      server.send(200, "text/html", content);  
      flag=1;
  }
if(flag==0){
  String content = "<html><body>Please verify data <br>";
  content += "Can return to operation <a href='/'>here</a> </body></html>";
  server.send(200, "text/html", content);   
  }
    
}
//----------------------------------------------------------- END ---------------------------------------------------------------------------

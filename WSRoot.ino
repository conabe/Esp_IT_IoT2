/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* All the web pages that not the root are in another file                                                                                     */   
/*root page can be accessed only if authentification is ok  -----------------------------------------------------------------------------------*/
// All the processing at root is dealing by this function.(ACO)
void handleRoot(){

// ------------Authentication function OFF if is in comment
if(PWFLAG==1){
  Serial.println("Enter handleRoot");
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
}
  String content = "<html>";

  if(CSS_FLAG){content += cssstyle();}
  
  content += "<body>";
  content +=_BRAND_;
  
  if(PWFLAG==1){//------------Authentication function OFF if is in comment 
    content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  }
  //Create all the push button for the on/off operation
 content += "<table>";
 content += "<tr><th>LG Air Conditioner:</th><th><a href='?pin=LGOn24H'><button class='b1'>ON Heat 24C</button></a></th></tr>"; 
 content += "<tr><th>LG Air Conditioner:</th><th><a href='?pin=LGOn18C'><button class='b1'>ON Cool 20C</button></a></th></tr>";
 content += "<tr><th>LG Air Conditioner:</th><th><a href='?pin=OFF_LG'><button class='b1'>OFF System </button></a></th></tr>"; 

/*
 content += "<p>Force Send Temperature :<a href='?pin=SENDT'><button>SEND Temp </button></a></p>";  
 content += "<p>Samsung Air Conditioner:<a href='?pin=SSOn24H'><button>ON Heat 24C</button></a></p>"; 
 content += "<p>Samsung Air Conditioner:<a href='?pin=SSOn20C'><button>ON Cool 20C</button></a></p>";
 content += "<p>Samsung Air Conditioner:<a href='?pin=OFF_SS'><button>OFF System </button></a></p>"; 
 content += "<p>Samsung Air Conditioner:<a href='?pin=SSQuiet'><button>Quiet Mode </button></a></p>"; 

 content += "<p>LG TV:<a href='?pin=TVOFF'><button>On Off</button></a></p>";
 content += "<p>LG TV:<a href='?pin=TVUP'><button>Sound UP </button></a></p>"; 
 content += "<p>LG TV:<a href='?pin=TVDOWN'><button>Sound Down </button></a></p>"; 
 */
 content += "<tr><th>Saidas Binarias :</th>";
 for(int i=0;i<9;i++){
  if(_gpio_on[i]==1){
    content += "<tr><th></th><th><a href='?pin=ON";
    content.concat(i);
    content += "'><button class='b2'> ON";
    content.concat(i);
    content += "</button></a> <a href='?pin=OFF";
    content.concat(i);
    content += "'><button class='b2'>OFF";
    content.concat(i);
    content += "</button></a></th></tr>";
  } 
 }

 
 content += "</tr></table>";
      
  if(PWFLAG==1){//------------Authentication function OFF if is in comment 
  content += "<p>Alterar Password <a href='/changepass'>here</a></p>";
  } 
 
  //Printout the Network power strength
  content += "<p>Power Strength";
  content.concat(WiFi.RSSI());
  content += " </p> ";

  // Read the first line of the request
  String req = server.arg(0);
  Serial.println(req);
  
 //Request Command.........................   
  if(SendCommand(req) != 0) {
    content += "<p>Achieved</p>";
  }

  //Send html to server.........................   
  content += "</body></html>";
  server.send(200, "text/html", content);
                    
}
//-------------------------------------------------------------------SendCommand --------------------------------------------------------
//Send Comannd to the device or global VAR and return Operation code
byte SendCommand(String req)
{
    byte Op_send = 0;
    
// Match the request
    
   if (req.indexOf("LGOn24H") != -1){
      irsend.sendRaw(LGOn24H, sizeof(LGOn24H)/sizeof(int), 38);
      Op_send=1;
      }
   else if (req.indexOf("OFF_LG") != -1){   
      irsend.sendLG(0x88C0051,28);
      Op_send=2;
    }
   else if (req.indexOf("LGOn18C") != -1){
      irsend.sendRaw(LGon18C, sizeof(LGon18C)/sizeof(int), 38);    
      Op_send=3; 
      }  
   else if (req.indexOf("SENDT1") != -1){
      SendInfo(0,_webs1,_webp1,_porta1,1);  
      }  
    else if (req.indexOf("SENDT2") != -1){
      SendInfo(0,_webs2,_webp2,_porta2,1);  
      }           
   /*
   else if (req.indexOf("TVUP") != -1){
      irsend.sendRaw(tvup, sizeof(tvup)/sizeof(int), 38 );  
      Op_send=4;   
      }  

   else if (req.indexOf("TVDOWN") != -1){
      irsend.sendRaw(tvdown, sizeof(tvdown)/sizeof(int), 38);  
      Op_send=5;   
      }  
    else if (req.indexOf("TVOFF") != -1){
      irsend.sendRaw(tvoff, sizeof(tvoff)/sizeof(int), 38);  
      Op_send=6;   
      }             
      
   else if (req.indexOf("SSOn24H") != -1){
      irsend.sendRaw(SSOn24H, sizeof(SSOn24H)/sizeof(int), 38); 
      Op_send=7;    
      }   
   else if (req.indexOf("SSOn20C") != -1){
      irsend.sendRaw(SSOn20C, sizeof(SSOn20C)/sizeof(int), 38 ); 
      Op_send=8;    
      }      

   else if (req.indexOf("OFF_SS") != -1){
      irsend.sendRaw(SSOff, sizeof(SSOff)/sizeof(int), 38);    
      Op_send=9; 
      } 

   else if (req.indexOf("SSQuiet") != -1){
      irsend.sendRaw(SSQuiet, sizeof(SSQuiet)/sizeof(int), 38);  
      Op_send=10;   
      } 
   */                     
   else if (req.indexOf("AUTENT") != -1){
      PWFLAG=1;     
      Op_send=11;
      }   
   else if (req.indexOf("NAUTEN") != -1){
      PWFLAG=0;  
      Op_send=12;   
      }
   else if (req.indexOf("SRESET") != -1){
      ESP.reset(); 
      Op_send=13;
      }   
     
  String _on,_off;   // Set output values for GPIO
  for(int i=0;i<9;i++){
    _on="ON";
    _off="OFF";
    _on.concat(i);
    _off.concat(i);
    if (req.indexOf(_on) != -1){
      digitalWrite(_gpio[i], 0);
      Op_send=20+i;

      }  
   else if (req.indexOf(_off) != -1){
      digitalWrite(_gpio[i], 1);
      Op_send=20+i;
      }     
  
 }
       
       
  return (Op_send);
}
//------------------------------------------------------------------------ END ---------------------------------------------------------

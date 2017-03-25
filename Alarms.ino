/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* Handle Alarms                                                                                                                               */  
/* Handle Input/Output Alarm ------------------------------------------------------------------------------------------------------------------*/
#define ALARM_CODE 112
void handleIOAlarm(byte number,byte port, byte state)
{
      
      pinMode(_gpio[port], INPUT);

      byte buttonState= (byte)digitalRead(_gpio[port]); 
      if(buttonState == state){
         room_temperature=(float)number;
         Serial.print("ALARM #");
         Serial.println(number);
         Serial.print("On Port");
         Serial.println(port);                  
          
         /*Set Time*/ 
         _alarm_repeat[number]=_ALARM_EVENT; 
         /*Send Alarm*/
         if(_alarm_srv[number]==1){
            SendInfo(0,_webs2,_webp2,_porta2,ALARM_CODE); 
            Serial.println("Send to S2");   
         }else if(_alarm_srv[number]==2){
            SendInfo(0,_webs1,_webp1,_porta1,ALARM_CODE); 
            SendInfo(0,_webs2,_webp2,_porta2,ALARM_CODE);   
            Serial.println("Send to S1 and S2");                    
         }else{
            SendInfo(0,_webs1,_webp1,_porta1,ALARM_CODE); 
            Serial.println("Send S1");             
            }
        }

}
/*Handle All Alarms ----------------------------------------------------------------------------------------------------------------------*/
void HandleAllAlarms(void)
{
  for(int i=0;i<_ALARM_NUMBER;i++){

  /*If IO Alarm-----------------------------------*/
  if((_alarm_on[i]==1)&&(_alarm_repeat[i] == 0)){
    
    handleIOAlarm(i,_alarm_cfg[i],1);
    _gpio_on[_alarm_cfg[i]]=2; // Set GPIO to input mode.
    
  }else if((_alarm_on[i]==2)&&(_alarm_repeat[i] == 0)){
    
    handleIOAlarm(i,_alarm_cfg[i],0);
    _gpio_on[_alarm_cfg[i]]=2; // Set GPIO to input mode.
  }
  /*If temperature Alarm*/

  
  }
}
/*HandleAlarmCounter----------------------------------------------------------------------------------------------------------------------*/
void HandleAlarmCounter(void)
{
   for(int i=0;i<_ALARM_NUMBER;i++){

  /*Set and Reset event counter*/
  if(_alarm_repeat[i] > 0){
    _alarm_repeat[i]--;
     Serial.println(_alarm_repeat[i]);
    }     
  }
}
/*CheckIOAlarms----------------------------------------------------------------------------------------------------------------------*/
/*Returns 0 if sucess                                                                                                                */
byte checkIOAlarms(int i)
{
      if((_alarm_on[i] == 1) || (_alarm_on[i]==2)){
        if(_alarm_cfg[i] > 8){ /*If out of range*/
           _alarm_on[i] = 0;
           return(1);
        }

        if(_reserved[_alarm_cfg[i]] == 1 ){/*If used a reserved IO*/
           _alarm_on[i] = 0;
           return(1);          
        }        
      }
      _alarm_repeat[i]=0;

    return(0);
}



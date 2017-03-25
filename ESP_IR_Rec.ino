/*
* IRremoteESP8266: IRrecvDumpV2 - dump details of IR codes with IRrecv
* An IR detector/demodulator must be connected to the input RECV_PIN.
* Version 0.1 Sept, 2015
* Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
*/

/*------------- Presente the code for the operation ---------------------------*/
#ifdef IR_RECEIVER_ 
byte Ir__Receiver_Decode (decode_results *results)
{


switch (results->value) {

 case 0xF9708F: _irr_flag_=0x01; break; //On
 case 0xF9609F: _irr_flag_=0x02; break; //Off

 case 0xF98877: _irr_flag_+=0x10;  break;
 case 0xF9A05F: _irr_flag_+=0x20;  break;
 case 0xF9906F: _irr_flag_+=0x30;  break;
 case 0xF9A857: _irr_flag_+=0x40;  break;
 case 0xF920DF: _irr_flag_+=0x50;  break;
 case 0xF910EF: _irr_flag_+=0x60;  break;
 case 0xF928D7: _irr_flag_+=0x70;  break;
 case 0xF900FF: _irr_flag_+=0x80;  break;
 case 0xF908F7: _irr_flag_+=0x90;  break;
 case 0xF930CF: _irr_flag_+=0xa0;  break;
 default: break;
 }

 if( (_irr_flag_ > 0x10) && ( _irr_flag_ & 0x0F) ){
        byte aux=_irr_flag_;
        _irr_flag_=0x00;
        return(aux);
        }
return(0);        
}
#endif


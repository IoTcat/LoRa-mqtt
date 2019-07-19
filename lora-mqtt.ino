
#define LORA_SOCKET_IP "2.2.2.2"

#include "lora-mqtt.h"


LoRaMQTT mqtt;

void msg(String subject, String msg){
    Serial.println(subject);
    Serial.println(msg);
}


void setup(){
    Serial.begin(115200);
   if(!LoRa.begin(433E6) ){
       Serial.println("LoRa Failuer");
       while(1);
   }
   LoRa.receive();
    mqtt.ini();
    mqtt.subscribe("lalala");
    mqtt.onReceived(msg);
    //mqtt.publish("lalala", "aaaaa");
}


void loop(){

    mqtt.core();
}
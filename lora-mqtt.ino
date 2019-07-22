
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
    mqtt.subscribe("#");
    //mqtt.onReceived(msg);
    //mqtt.publish("lalala", "aaaaa");
}


void loop(){
    if(mqtt.isNewMsg()){
        String s, c;
        mqtt.getNewMsg(s, c);
        Serial.println(s);
        Serial.println(c);
        mqtt.publish("res/json", "lalla");
    }
    mqtt.core();
}
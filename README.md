# LoRa-mqtt

[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FIoTcat%2FLoRa-mqtt.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FIoTcat%2FLoRa-mqtt?ref=badge_shield)
![size](https://badge-size.herokuapp.com/iotcat/LoRa-mqtt/master/lora-mqtt.h)
 
Use MQTT in LoRa communicationg..

[简体中文](./zh.md)

## Usage - with Callback Functions
```C++

#define LORA_SOCKET_IP "1.0.0.1" //need to be unique

#include "lora-mqtt.h"

LoRaMQTT mqtt;

void setup(){
    mqtt.ini(); //initalization
    mqtt.onReceived(doIfRec); //register callback functions which will be executed when received message
    mqtt.subscribe("Subject"); //subscribe MQTT subject
}

void doIfRec(String subject, String content){
    //When received message this function will be executed.
    
    mqtt.publish("Subject", "Content"); //publish a mqtt message
}

void loop(){
    //No delay() could be used in loop()
    mqtt.core(); //Mqtt service core
}

```
## Usage - with If in loop()
```C++

#define LORA_SOCKET_IP "1.0.0.1" //need to be unique

#include "lora-mqtt.h"

LoRaMQTT mqtt;

void setup(){
    mqtt.ini(); //initalization
    mqtt.subscribe("Subject"); //subscribe MQTT subject
}

void loop(){
    //When received message this function will be executed.
    
    if(mqtt.isNewMsg()){
        String subject, content;
        mqtt.getNewMsg(subject, content);
        Serial.println(subject, content);
    }
    
    mqtt.core(); //Mqtt service core
}

```


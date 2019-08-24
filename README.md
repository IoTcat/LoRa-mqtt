# LoRa-mqtt

## 使用案例 - 使用回调函数
```C++

#define LORA_SOCKET_IP "1.0.0.1" //需唯一

#include "lora-mqtt.h"

LoRaMQTT mqtt;

void setup(){
    mqtt.ini(); //初始化
    mqtt.onReceived(doIfRec); //注册回调函数，当接收到新消息时触发
    mqtt.subscribe("主题名称"); //订阅mqtt主题
}

void doIfRec(String subject, String content){
    //当收到新消息时执行这个函数中的内容
    
    mqtt.publish("主题", "内容"); //发布mqtt消息
}

void loop(){
    //注意loop()全局不得有delay，否则会导致接收不稳定
    mqtt.core(); //循环组件
}


```
## 使用案例 - 使用条件触发
```C++

#define LORA_SOCKET_IP "1.0.0.1" //需唯一

#include "lora-mqtt.h"

LoRaMQTT mqtt;

void setup(){
    mqtt.ini(); //初始化
    mqtt.subscribe("主题名称"); //订阅mqtt主题
}

void loop(){
    //注意loop()全局不得有delay，否则会导致接收不稳定
    
    if(mqtt.isNewMsg()){
        String subject, content;
        mqtt.getNewMsg(subject, content);
        Serial.println(subject, content);
    }
    
    mqtt.core(); //循环组件
}



```

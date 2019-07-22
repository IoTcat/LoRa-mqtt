#ifndef __LORA_SOCKET_H__
#define __LORA_SOCKET_H__

#include <LoRa.h>
#include "vector.h"
#include "stringVec.h"

#ifndef LORA_SOCKET_IP
#define LORA_SOCKET_IP "0.0.0.0"
#endif

#ifndef MAX_RECEIVE_STACK_SIZE
#define MAX_RECEIVE_STACK_SIZE 5
#endif

#ifndef MAX_SEND_STACK_SIZE
#define MAX_SEND_STACK_SIZE 5
#endif

#ifndef MAX_TCP_TRY_TIMES
#define MAX_TCP_TRY_TIMES 8
#endif


class LoRaSocket {
   public:
    LoRaSocket(){}
    static void core();
    static void ini();
    static void udp(const String& msg, const String& to = "0.0.0.0");
    static void tcp(const String& msg, const String& to);
    static void rtcp(const String& msg);
    inline static void onReceived(void (*f)(String, String, String, String)){
        _f = f;
    };
    inline bool isNewMsg(){
        isLoopMode = true;
        return (newType == "") ? false : true;
    };
    inline const String getNewMsg(){
        String msg = newMsg;
        clearNewMsg();
        return msg;
    };
    inline void getNewMsg(String& msg, String& from, String& to, String& type){
        msg = newMsg;
        from = newFrom;
        to = newTo;
        type = newType;
        clearNewMsg();
    };

   private:
    static StringVec tcp_sendingStack, tcp_receiveStack;
    static Vector<unsigned int> tcp_sendingTryTimes;
    static void(*_f)(String, String, String, String);
    static String newMsg, newFrom, newTo, newType;
    static bool isLoopMode;
    inline void clearNewMsg(){
        newMsg = "";
        newFrom = "";
        newTo = "";
        newType = "";
    };
    inline static void _onReceived(const String& msg, const String& from, const String& to, const String& type){
        newMsg = msg;
        newFrom = from;
        newTo = to;
        newType = type;
        if(!isLoopMode) (*_f)(msg, from, to, type);
    };
  /* LoRa Functions */
    static void LoRa_tx_mode();
    static void LoRa_rx_mode();
    static void send(const String& s);
    static void send(const char *s);
    static const String receiveMsg();
    /* Package Functions */
    inline static const String getIPHeader(const String& to = "0.0.0.0"){
        return to + "|" + LORA_SOCKET_IP + "|";
    };
    inline static bool isGoodPackage(const String& s){
        String body = s.substring(0, s.length() - 1);
        if(s.substring(s.length() - 1, s.length()) != hash(body)){
            return false;
        }
        return true;
    };
    inline static const String getToIP(const String& s){
        unsigned short left = s.indexOf('|');
        unsigned short right = s.indexOf('|', left + 1);
        return s.substring(left + 1, right);
    };
    inline static const String getFromIP(const String& s){
        unsigned short left = s.indexOf('|', s.indexOf('|') + 1);
        unsigned short right = s.indexOf('|', left + 1);
        return s.substring(left + 1, right);
    };
    inline static const String getType(const String& s){
        return s.substring(0, s.indexOf('|'));
    };
    inline static const String getContent(const String& s){
        unsigned short left = s.indexOf('|', s.indexOf('|', s.indexOf('|') + 1) + 1);
        unsigned short right = s.indexOf('|', left + 1);
        return decode(s.substring(left + 1, right));
    };
    inline static const String getTcpKey(const String& s){
        int left = s.indexOf('|', s.indexOf('|', s.indexOf('|', s.indexOf('|') + 1) + 1) + 1);
        int right = s.indexOf('|', left + 1);
        return s.substring(left + 1, right);
    };
    /* receive Functions */
    static void getMsg(const String& msg);
    /* tcp stack functions */
    static void checkSendStack(){
        for(unsigned int i = 0; i < tcp_sendingStack.Size(); i ++){
            send("tc"+tcp_sendingStack[i]);
            tcp_sendingTryTimes[i] += 1;

            if(tcp_sendingTryTimes[i] >= MAX_TCP_TRY_TIMES){
                tcp_sendingStack.Erase(i);
                tcp_sendingTryTimes.Erase(i);
            }
        }
    };
    inline static void receiveStackClassify(){
        if(tcp_receiveStack.Size() > MAX_RECEIVE_STACK_SIZE) tcp_receiveStack.Erase(0);
    };
    inline static void sendStackClassify(){
        if(tcp_sendingStack.Size() > MAX_SEND_STACK_SIZE) {
            tcp_sendingStack.Erase(0);
            tcp_sendingTryTimes.Erase(0);
        }
    };
    static void removeByKey(const String& key);
    /* tools */
    static const String hash(const String& s);
    static const String encode(const String& s){
        return s;
    };
    static const String decode(const String& s){
        return s;
    }
    static const String generateRandomKey(){
        String o = "";
        for(unsigned short i = 0; i < 4; i ++){
            o += char(random(26) + 97);
        }
        return o;
    }

    /* timer */
    static void setInterval(void (*function)(void), const int delay){
        static unsigned long startTime = millis(); 

        if(millis() - startTime > delay){
            (*function)();
            startTime = millis();
        }
    }


};

void LoRaSocket::getMsg(const String& msg){

    if(!isGoodPackage(msg)) return;
    if(getToIP(msg) != LORA_SOCKET_IP && getToIP(msg) != "0.0.0.0") return;

    if(getType(msg) == "udp") _onReceived(getContent(msg), getFromIP(msg), getToIP(msg), "udp");
    if(getType(msg) == "tcp"){
        rtcp(msg);
        if(tcp_receiveStack.Find(msg) != -1) return;
        _onReceived(getContent(msg), getFromIP(msg), getToIP(msg), "tcp");
        receiveStackClassify();
    }
    if(getType(msg) == "rtcp"){
        removeByKey(getContent(msg));
    }

}

void LoRaSocket::udp(const String& msg, const String& to){
    char *c;
    c = (char*)malloc((msg.length()+39)*sizeof(char));
    sprintf(c, "udp|%s%s|", getIPHeader(to).c_str(), encode(msg).c_str());
    sprintf(c, "%s%s", c, hash(c).c_str());
    send(c);
    free(c);
};


void LoRaSocket::tcp(const String& msg, const String& to){
    String fin = "tcp|" + getIPHeader(to) + encode(msg) + "|" + generateRandomKey() + "|";
    fin += hash(fin);
    tcp_sendingStack.PushBack(fin);
    tcp_sendingTryTimes.PushBack(0);
    sendStackClassify();
    send(fin);
};


void LoRaSocket::rtcp(const String& msg){
    tcp_receiveStack.PushBack(msg);
    String fin = "rtcp|" + getIPHeader(getFromIP(msg)) + getTcpKey(msg) + "|";
    fin += hash(fin);
    send(fin);
}

StringVec LoRaSocket::tcp_sendingStack, LoRaSocket::tcp_receiveStack;
Vector<unsigned int> LoRaSocket::tcp_sendingTryTimes;
void (*LoRaSocket::_f)(String, String, String, String);
String LoRaSocket::newMsg = "", LoRaSocket::newFrom = "", LoRaSocket::newTo = "", LoRaSocket::newType = "";
bool LoRaSocket::isLoopMode = false;

void LoRaSocket::ini() {

    LoRa_rx_mode();
}

void LoRaSocket::core() {

    /* Listen Msg */
    if(LoRa.parsePacket()){
        getMsg(receiveMsg());
    }

    /* check tcp stack */
    setInterval(checkSendStack, 1000);

}

void LoRaSocket::LoRa_tx_mode(){
    LoRa.idle();
}

void LoRaSocket::LoRa_rx_mode(){
    LoRa.receive();
}

void LoRaSocket::send(const char *s){
    LoRa_tx_mode();
    delay(200);
    LoRa.beginPacket();Serial.println(s);
    LoRa.print(s);
    LoRa.endPacket();
    delay(200);
    LoRa_rx_mode();
}

void LoRaSocket::send(const String& s){
  LoRa_tx_mode();
  delay(200);
  LoRa.beginPacket();Serial.println(s);
  LoRa.print(s);
  LoRa.endPacket();
  delay(200);
  LoRa_rx_mode();
}


const String LoRaSocket::receiveMsg(){
    String s = "";
    while (LoRa.available()) {
        s += (char)LoRa.read();
    }
    return s;
}

const String LoRaSocket::hash(const String& s){
    unsigned char hashVal = 'k';
    for(unsigned short i = 0; i < s.length(); i ++){
        hashVal ^= s.charAt(i);
    }
    hashVal = hashVal % 26 + 97;
    return String((char)hashVal);
}


void LoRaSocket::removeByKey(const String& key){
    for(unsigned int i = 0; i < tcp_sendingStack.Size(); i++){
        if(getTcpKey(tcp_sendingStack[i]) == key) {
            tcp_sendingStack.Erase(i);
            return;
        }
    }
}

#endif //__LORA_SOCKET_H__
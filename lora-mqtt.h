#ifndef __LORA_MATT_H__
#define __LORA_MATT_H__

#define MAX_STRINGVEC_SIZE 20
#include "lora-socket.h"


class LoRaMQTT{

public:

    inline void ini(){
        socket.ini();
        socket.onReceived(_onReceived);
    }

    inline void core(){
        socket.core();
    }


    inline void publish(const String& subject, const String& content){
        String s = subject + "$" + content;
        socket.udp(s);
    }

    inline void subscribe(const String& subject){
        this->_subjects.PushBack(subject);
    }

    inline static void onReceived(void (*f)(String, String)){
        _f = f;
    }



private:
    static LoRaSocket socket;
    static StringVec _subjects;
    static void (*_f)(String, String);

    inline static void _onReceived(String msg, String from, String to, String type){

        String subject = msg.substring(0, msg.indexOf('$'));
        String content = msg.substring(msg.indexOf('$') + 1, msg.length());

        if(_subjects.Find(subject) != -1){
            _f(subject, content);
        }
    };


};


LoRaSocket LoRaMQTT::socket;
StringVec LoRaMQTT::_subjects;
void (*LoRaMQTT::_f)(String, String);


#endif //__LORA_MATT_H__
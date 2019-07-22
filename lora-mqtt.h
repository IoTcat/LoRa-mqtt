#ifndef __LORA_MATT_H__
#define __LORA_MATT_H__

#define MAX_STRINGVEC_SIZE 2
#include "lora-socket.h"


class LoRaMQTT{

public:

    inline void ini(){
        socket.ini();
        socket.onReceived(_onReceived);
    }

    inline void core(){
        if(isLoopMode){
            if(socket.isNewMsg()){
                String msg = "", from = "", to = "", type = "";
                socket.getNewMsg(msg, from, to, type);
                _onReceived(msg, from, to, type);
            }
        }
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
    inline bool isNewMsg(){
        isLoopMode = true;
        return (newSubject == "") ? false : true;
    };
    inline void getNewMsg(String& subject, String& content){
        subject = newSubject;
        content = newContent;
        clearNewMsg();
    }


private:
    static LoRaSocket socket;
    static StringVec _subjects;
    static void (*_f)(String, String);
    static String newSubject, newContent;
    static bool isLoopMode;

    inline void clearNewMsg(){
        newSubject = "";
        newContent = "";
    };

    inline static void _onReceived(String msg, String from, String to, String type){
        
        if(msg.indexOf('$') == -1) return;
        String subject = msg.substring(0, msg.indexOf('$'));
        String content = msg.substring(msg.indexOf('$') + 1, msg.length());

        if(_subjects.Find("#") != -1 || _subjects.Find(subject) != -1){
            newSubject = subject;
            newContent = content;
            if(!isLoopMode) _f(subject, content);
        }
    };


};


LoRaSocket LoRaMQTT::socket;
StringVec LoRaMQTT::_subjects;
void (*LoRaMQTT::_f)(String, String);
String LoRaMQTT::newSubject = "", LoRaMQTT::newContent = "";
bool LoRaMQTT::isLoopMode = false;

#endif //__LORA_MATT_H__
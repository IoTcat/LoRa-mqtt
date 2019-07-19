#ifndef __STRINGVEC__
#define __STRINGVEC__



#ifndef MAX_STRINGVEC_SIZE
#define MAX_STRINGVEC_SIZE 10
#endif

class StringVec{
public: 
    StringVec():_size(0){
        this->clear();
    };

    inline void PushBack(const String& ss){
        if(this->Size() == MAX_STRINGVEC_SIZE){
            this->shift();
        }
        this->_s[this->Size()] = ss;
        this->_size ++;
    }

    const int Find(const String& ss){
        for(unsigned i = 0; i < this->Size(); i ++){
            if(this->_s[i] == ss){
                return i;
            }
        }
        return -1;
    }

    void Erase(const int& pos){
        if(pos < 0 || pos >= this->Size()) return;
        for(unsigned int i = pos+1; i < this->Size(); i ++){
            this->_s[i-1] = this->_s[i];
        }
        this->_size --;
    }

    const String shift(){
        String s = this->_s[0];
        if(this->Size() != 0){
            for(unsigned int i = 1; i < this->Size(); i ++){
                this->_s[i-1] = this->_s[i];
            }
            this->_size --;
        }
        return s;
    }

    void clear(){
        for(unsigned int i = 0; i < this->Size(); i ++){
            this->_s[i] = "";
        }
    }

    inline const int Size(){
        return this->_size;
    }


    String& operator[](int i){
        if(i < 0 || i >= this->Size()) return this->_s[0];
        return this->_s[i];
    }

private:
    String _s[MAX_STRINGVEC_SIZE];
    int _size;

};




#endif //__STRINGVEC__
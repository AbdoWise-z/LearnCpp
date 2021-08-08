#pragma once

#include<memory.h>
#include<iostream>

//copys bits form the src address to the dst addres
//dst : the distination address
//src : the source address
//bits : the number of bits to copy
void bitcpy(void* dst , const void* src , int bits){
    char* c_dst = (char*) dst;
    char* c_src = (char*) src;

    for (int i = 0;i < bits;i++){
        c_dst[i / 8] = (c_dst[i / 8] & ~(0x01 << (i % 8) )) | (c_src[i / 8] & (0x01 << (i % 8) ));
    }
}

//copys bits form the src address to the dst addres
//dst : the distination address
//src : the source address
//bits : the number of bits to copy
//offset : the offset of the first bit to start with from src and dst
void bitcpy(void* dst , const void* src , int bits , int offset){ //offset is in bits
    
    char* c_dst = (char*) dst;
    char* c_src = (char*) src;
    int s;
    for (int i = 0;i < bits;i++){
        s = i + offset;
        c_dst[s / 8] = (c_dst[s / 8] & ~(0x01 << (s % 8) )) | (c_src[s / 8] & (0x01 << (s % 8) ));
    }
}


//copys bits form the src address to the dst addres
//dst : the distination address
//src : the source address
//bits : the number of bits to copy
//dst_offset : offset of the first bit to start with in the distination
//src_offset : offset of the first bit to start with in the source
void bitcpy(void* dst , const void* src , int bits , int dst_offset , int src_offset){
    char* c_dst = (char*) dst;
    char* c_src = (char*) src;
    int db , sb;
    for (int i = 0;i < bits;i++){
        db = i + dst_offset;
        sb = i + src_offset;
        c_dst[db / 8] = ( c_dst[db / 8] & (~(0x01 << (db % 8))) ) | ( ( (c_src[sb/8] >> (sb % 8)) & 0x01) << db );
    }
}

//a class that holds some binary data
//'s' must be a factor of 8

template<unsigned int s>
class bitset{
    char data[s/8];
    //ex:
    //  data[3]  data[2]  data[1]  data[0]
    // 00001010 01010010 01110000 01010101
    //
    //

public:
    bitset(){
        if (s % 8 != 0 || s == 0){
            throw "bitset size must be a factor of 8";
        }
    }

    bitset(const bitset<s>& other){
        memcpy(data , other.data , s / 8);
    }

    template<typename T>
    bitset(const T& t){
        memcpy(data , &t , min(sizeof(T) , s / 8));
    }

    constexpr int size(){
        return s;
    }

    bool operator[](int b){
        return (data[b / 8] >> b % 8) & 0x01;
    }

    void set(int b , bool on){
        if (on)
            data[b / 8] = data[b / 8] | 0x01 << (b % 8);
        else
            data[b / 8] = data[b / 8] & (~(0x01 << (b % 8)));
    }

    void clear(){
        for (int i = 0;i < s / 8;i++){
            data[i] = 0x00;
        }
    }

    void setAll(bool on){
        for (int i = 0;i < s / 8;i++){
            data[i] = on ? 0xff : 0x00;
        }
    }

    bitset<s>& operator<<(unsigned int f){
        if (f != 0){
            if (f >= s){
                clear();
            }else{
                char temp[s/8];
                
                memcpy(temp , data , s / 8);

                clear();
                bitcpy(data , temp , s - f , f , 0);
            }
        }

        return *this;
    }

    bitset<s>& operator>>(unsigned int f){
        if (f != 0){
            if (f >= s){
                clear();
            }else{
                char temp[s/8];
                
                memcpy(temp , data , s / 8);

                clear();
                bitcpy(data , temp , s - f , 0 , f);
            }
        }

        return *this;
    }

    bitset<s>& operator&(const bitset<s> t){
        for (int i = 0;i < s / 8;i++){
            data[i] = data[i] & t.data[i];
        }
        return *this;
    }

    bitset<s>& operator^(const bitset<s> t){
        for (int i = 0;i < s / 8;i++){
            data[i] = data[i] ^ t.data[i];
        }
        return *this;
    }

    bitset<s>& operator|(const bitset<s> t){
        for (int i = 0;i < s / 8;i++){
            data[i] = data[i] | t.data[i];
        }
        return *this;
    }

    template<class T>
    bitset<s>& operator=(const T& t){
        bitcpy(data , &t , s);
        return *this;
    }

    
    template<typename T>
    T getAs(){
        int ts = sizeof(T);
        T* val = (T*) alloca(ts);
        memset(val , 0 , ts);
        memcpy(val , data , min(ts , s / 8));
        return *val;
    }

    char& getByte(int b){
        return data[b];
    }

    const void* getData() const {
        return (void*) data;
    }

private:
    inline static int min(int a , int b){
        return a > b ? b : a;
    }
};

template<unsigned int j>
void print_bits(const bitset<j>& bits){
    const char* ptr = (const char*) bits.getData();
    int t = 0;
    for (int i = 0;i < j;i++){
        t = j - i - 1;
        std::cout << (ptr[t / 8] >> (t % 8) & 0x01);
    }
    std::cout << std::endl;
}

template<typename T>
void print_bits(const T& obj){
    const char* ptr = (const char*) &obj;
    int t = 0;
    int j = sizeof(T) * 8;
    for (int i = 0;i < j;i++){
        t = j - i - 1;
        std::cout << (ptr[t / 8] >> (t % 8) & 0x01);
    }
    std::cout << std::endl;
}


typedef bitset<8>       unit8;
typedef bitset<16>      unit16;
typedef bitset<24>      unit24;
typedef bitset<32>      unit32;
typedef bitset<64>      unit64;
typedef bitset<128>     unit128;
typedef bitset<256>     unit256;
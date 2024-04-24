#ifndef CLOCK_H
#define CLOCK_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <map>
#include <string>
#include "definition.h"
#include "hash_class.h"



class Recent_Sketch{
public :
    unsigned int clock_pos;
    double clock_pos2;
    double prev_clock_pos2;
    unsigned int len;
    // 更新周期
    unsigned int step;
    unsigned int cycle_num;
    unsigned int cycle_num2;
    int row_length;
    int hash_number;
    int field_num;
    unsigned long long int last_time;

    /// @brief 
    std::map<std::string, int> element_count_;

    /// @brief element_count_ update time
    int element_count_step_;

    /// @brief 
    int c1_;

    /// @brief 
    int c2_;

    /// @brief 
    std::map<std::string, int> frecuency_confirmations_;


    // c = 500000
    // l = スケッチの全体のサイズ
    Recent_Sketch(unsigned int c, unsigned int l, int _row_length, int _hash_number, int _field_num):
        len(l),step(l*(_field_num-1)/c),row_length(_row_length),hash_number(_hash_number),field_num(_field_num){
        // clock_pos2 = 0;
        // cycle_num2 = 0;
        // 本当にこの実装で大丈夫か？という懸念もある
        // タイマーにスケッチを持たせる方法も存在する
        // その場合は、処理は人間に理解しやすい方法になるが
        // このプログラムに対してかなりの変更をくわえる必要がある。
        // 変更が多くなるので、現時点ではあまり現実的ではない
        // 懸念事項として9.9から11にとんだときに、10は常に使われない状態になる
        clock_pos = 0;
        clock_pos2 = 0;
        last_time = 0;
        cycle_num = 0;
        cycle_num2 = 0;
        prev_clock_pos2 = 0;
    }
    int Mid(int *num);
};

class Recent_Counter: public Recent_Sketch{
public:
    int field_num;
    struct Unit{
        int* count;
        int field_num;
        int Total(){
            int ret = 0;
            for(int i = 0;i < field_num;++i){
                ret += count[i];
            }
            return ret;
        }
    };
    Unit* counter;
    Unit* counter2;

    /// @brief used for proposed method when call frecuency query
    struct Frecuency {
        /// @brief key of array for correction
        int key;
        /// @brief correction of frecuency
        int count;
    };
    
    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num);
    ~Recent_Counter();
    void Clock_Go(unsigned long long int num);
    void Clock_Go(unsigned long long int num, bool counter2_flag);
    void CM_Init(const unsigned char* str, int length, unsigned long long int num);//CM Sketch update an item
    void CO_Init(const unsigned char* str, int length, unsigned long long int num);//Count Sketch update an item
    void CU_Init(const unsigned char* str, int length, unsigned long long int num);//CU Sketch update an item
    int CO_Query(const unsigned char* str, int length);//Count Sketch query an item
    unsigned int Query(const unsigned char* str, int length, bool display_min_pos);//CM(CU) Sketch update an item

    /// @brief CM Sketch Query an item by Async Sliding Sketch
    /// @param str target string
    /// @param length target string num
    /// @return The frequency in a Sketch
    unsigned int AsyncSS_CM_Query(const unsigned char* str, int length);

    /// @brief Async SS update an item
    /// @param str 
    /// @param length 
    /// @param num 
    void AsyncSS_CM_Init(const unsigned char* str, int length, unsigned long long int num);

    /// @brief Initialize element_count_
    void InitilizeElementCount();
};

#endif  // CLOCK_H

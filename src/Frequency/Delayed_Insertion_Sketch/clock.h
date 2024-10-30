#ifndef CLOCK_H
#define CLOCK_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "definition.h"
#include "hash_class.h"

//typedef std::array<const unsigned char, DATA_LEN> packet_str;
using packet_str = std::array<unsigned char, DATA_LEN>;

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
    unsigned long long int last_time2;

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
        last_time2 = 0;
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
        int recently_reset_time;
        int Total(){
            int ret = 0;
            for(int i = 0;i < field_num;++i){
                ret += count[i];
            }
            return ret;
        }
    };

    struct Unit2{
        int count;
    };

    struct ArrayHasher {
        std::size_t operator()(const std::array<char, DATA_LEN>& a) const {
            std::size_t h = 0;

            for (auto e : a) {
                h ^= std::hash<char>{}(e) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };


    Unit* counter;
    Unit* counter2;
    Unit2* counter_DE;

    /// @brief used for proposed method when call Frequency query
    struct Frequency {
        /// @brief key of array for correction
        // const unsigned char* key;
        std::string key;
        /// @brief correction of Frequency
        int count;
    };


    /// @brief correction sketch
    //std::vector<Frequency> element_count_2_;

    std::map<packet_str, int> element_count_2_;

    /// @brief correction sketch
    //std::vector<Frequency> element_count_2_;

    /// @brief element_count_ update time
    int element_count_step_;
    
    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num, int element_count_step);
    ~Recent_Counter();
    void Clock_Go(unsigned long long int num);
    void Clock_Go(unsigned long long int num, bool counter2_flag);
    void CM_Init(const unsigned char* str, int length, unsigned long long int num);//CM Sketch update an item
    void CO_Init(const unsigned char* str, int length, unsigned long long int num);//Count Sketch update an item
    void CU_Init(const unsigned char* str, int length, unsigned long long int num);//CU Sketch update an item
    int CO_Query(const unsigned char* str, int length);//Count Sketch query an item
    unsigned int Query(const unsigned char* str, int length, bool display_min_pos);//CM(CU) Sketch update an item

    /// @brief CM Sketch Query an item by Delayed Insertion
    /// @param str target string
    /// @param length target string num
    /// @return The frequency in a Sketch
    unsigned int DelayedInsertion_CM_Query(const unsigned char* str, int length);

    /// @brief Delayed Insertion SS update an item
    /// @param str 
    /// @param length 
    /// @param num 
    void DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num);

    /// @brief Initialize element_count_
    void Initilize_ElementCount(int length, unsigned long long int num);

    int GetTargetKeyIndex(const unsigned char* str);
    int GetTargetKeyIndex(std::string str);
    packet_str GetTargetKey(const unsigned char* str);
};

#endif  // CLOCK_H

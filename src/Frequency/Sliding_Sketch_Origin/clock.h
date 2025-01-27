#ifndef CLOCK_H
#define CLOCK_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>
#include <chrono>
#include "definition.h"
#include "hash_class.h"



class Recent_Sketch{
public :
    unsigned int clock_pos;
    unsigned int len;
    double step;
    unsigned int cycle_num;
    int row_length;
    int hash_number;
    int field_num;
    unsigned long long int last_time;
    std::vector<int> hit_counts;

    /// @brief スケッチへの挿入実行時間
    std::chrono::microseconds insertion_time;
    /// @brief クエリの実行時間駅粗供養
    std::chrono::microseconds query_time;

    Recent_Sketch(unsigned int c, unsigned int l, int _row_length, int _hash_number, int _field_num):
        len(l),step((double)l*(double)(_field_num-1)/(double)c),row_length(_row_length),hash_number(_hash_number),field_num(_field_num)
        ,insertion_time(0),query_time(0){
        clock_pos = 0;
        last_time = 0;
        cycle_num = 0;
        hit_counts.resize(hash_number, 0);
        std::cout << "step:" << step << std::endl;
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
#ifdef CHECK_COLLISION_HASH
    typedef std::array<const unsigned char, 4> in_str;
    std::vector<std::map<unsigned int, std::vector<in_str>>> collision_hash_;
    int collision_count_;
    int collision_element_access_count_;
#endif // CHECK_COLLISION_HASH

    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num);
    ~Recent_Counter();
    void Clock_Go(double num);
    void CM_Init(const unsigned char* str, int length, unsigned long long int num);//CM Sketch update an item
    void CO_Init(const unsigned char* str, int length, unsigned long long int num);//Count Sketch update an item
    void CU_Init(const unsigned char* str, int length, unsigned long long int num);//CU Sketch update an item
    int CO_Query(const unsigned char* str, int length);//Count Sketch query an item
    unsigned int Query(const unsigned char* str, int length);//CM(CU) Sketch update an item
    void DumpHitCount();
};

#endif // CLOCK_H

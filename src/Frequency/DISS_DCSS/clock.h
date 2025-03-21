#ifndef CLOCK_H
#define CLOCK_H

#include <iostream>
#include <string.h>
#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "definition.h"
#include "hash_class.h"

/// @brief 入力要素の構造
using packet_str = std::array<unsigned char, DATA_LEN>;

class Recent_Sketch{
public :
    unsigned int clock_pos;
    double clock_pos2;
    double prev_clock_pos2;
    unsigned int len;
    // 更新周期
    double step;
    unsigned int cycle_num;
    unsigned int cycle_num2;
    int row_length;
    int hash_number;
    int field_num;
    unsigned long long int last_time;
    unsigned long long int last_time2;
    std::vector<std::map<int,int>> hash_count;

    /// @brief スケッチへの挿入実行時間
    std::chrono::microseconds insertion_time;
    /// @brief クエリの実行時間駅粗供養
    std::chrono::microseconds query_time;

    std::vector<int> frequency_confirmations;

    // c = 500000
    // l = スケッチの全体のサイズ
    Recent_Sketch(unsigned int c, unsigned int l, int _row_length, int _hash_number, int _field_num):
        len(l),step((double)l*(double)(_field_num-1)/(double)c),row_length(_row_length),hash_number(_hash_number),field_num(_field_num)
        ,insertion_time(0),query_time(0){
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
        hash_count.resize(hash_number);
        frequency_confirmations.assign(row_length, 0);
    }
    int Mid(int *num);
};

class Recent_Counter: public Recent_Sketch{
public:
    int field_num;

    /// @brief NewOld struct
    // struct NewOld{
    //     int new_count;
    //     int old_count;
    // };

    struct Unit{
        int* count;
        // /// @brief NewとOldのみのカウント
        // NewOld count2;
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

    /// @brief 補正スケッチ
    std::map<packet_str, int> element_count_2_;

    /// @brief 補正スケッチの更新間隔
    int element_count_step_;
    
    Recent_Counter(int c, int l, int _row_length, int _hash_number, int _field_num, int element_count_step);
    ~Recent_Counter();
    void Clock_Go(double num);
    void Clock_Go(unsigned long long int num, bool counter2_flag);
    void CM_Init(const unsigned char* str, int length, unsigned long long int num);//CM Sketch update an item
    void CO_Init(const unsigned char* str, int length, unsigned long long int num);//Count Sketch update an item
    void CU_Init(const unsigned char* str, int length, unsigned long long int num);//CU Sketch update an item
    int CO_Query(const unsigned char* str, int length);//Count Sketch query an item
    unsigned int Query(const unsigned char* str, int length, bool display_min_pos);//CM(CU) Sketch update an item

    /// @brief CM Sketch Query an item by Delayed Insertion
    /// @param str target string
    /// @param length target string num
    /// @return The `ency in a Sketch
    unsigned int DelayedInsertion_CM_Query(const unsigned char* str, int length);

    /// @brief Delayed Insertion SS update an item
    /// @param str 
    /// @param length 
    /// @param num 
    void DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num);

    /// @brief Initialize element_count_
    void Initilize_ElementCount(int length, unsigned long long int num, unsigned long long int time);

    int GetTargetKeyIndex(const unsigned char* str);
    int GetTargetKeyIndex(std::string str);
    packet_str GetTargetKey(const unsigned char* str);

    /// @brief Hashカウントをダンプする
    void DumpHashCount();
};




#endif  // CLOCK_H

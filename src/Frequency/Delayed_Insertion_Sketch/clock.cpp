#include "clock.h"

#include <math.h>                                                               
#include <algorithm>

struct Place{
    unsigned int serial;
    unsigned int pos;
};
bool operator < (Place a, Place b){
    return a.serial < b.serial;
}

int Recent_Sketch::Mid(int *num){
    if(hash_number & 1){
        return max(num[hash_number >> 1], 0);
    }
    else{
        return max(0, (num[hash_number >> 1] + num[(hash_number >> 1) - 1]) >> 1);
    }
}

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num, int element_count_step):
    Recent_Sketch(c,l,_row_length,_hash_numberber,_field_num)
    , element_count_step_(element_count_step) {
    counter = new Unit [l];
    counter_DE = new Unit2[l];
    field_num = _field_num;
    row_length = _row_length;
    for(int i = 0; i < l; i++){
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }
}

Recent_Counter::~Recent_Counter(){
    delete [] counter;
}

void Recent_Counter::CM_Init(const unsigned char* str, int length, unsigned long long int num){
    unsigned int position;
    // 自信のインスタンスのメンバ変数clock_posが，速さ1か速さ1.1の領域のどちらを参照しているか判定する
    // 速さ1の領域を参照している場合は，Clock_Go(num * step)をcall
    // 速さ1.1の領域を参照している場合は，Clock_Go(num * step * 1.1)をcall
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        // std::cout << "i: " << i << " position: " << position << std::endl;
        // yesterdayかtodayかの判定
        if (position % 2 == 0) {
            counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
        } else {
            // 飛んだケースは存在するか？
            counter[position].count[(cycle_num2 + (position < (int)clock_pos2)) % field_num] += 1;
        }
    }
}

void Recent_Counter::CU_Init(const unsigned char* str, int length, unsigned long long int num){
    // clock_posの初期値は0だった
    // row_lengthは固定
    int k = clock_pos / row_length;
    // ここでclock_posを更新している
    Clock_Go(num * step);
    // k * row_length は多分オフセット
    unsigned int position = Hash(str, k ,length) % row_length + k * row_length;
    // yesterdayかtodayかの判定
    if(position < clock_pos){
        k = (k + 1) % hash_number;
        position = Hash(str, k ,length) % row_length + k * row_length;
    }

    unsigned int height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
    counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;

    for(int i = (k + 1) % hash_number;i != k;i = (i + 1) % hash_number){
        position = Hash(str, i ,length) % row_length + i * row_length;
        if(counter[position].count[(cycle_num + (position < clock_pos)) % field_num] <= height){
            height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
            counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
        }
    }
}

unsigned int Recent_Counter::Query(const unsigned char* str, int length, bool display_min_pos = false){
    unsigned int min_num = 0x7fffffff;
    unsigned int prev_min_num = 0x7fffffff;
    int min_pos = 0;

    for(int i = 0;i < hash_number;++i) {
        int pos = Hash(str, i, length) % row_length + i * row_length;
        min_num = min(counter[pos].Total(), min_num);
        if (min_num < prev_min_num || min_num == prev_min_num) {
            min_pos = pos;
        }
        prev_min_num = min_num;
    }
    if (display_min_pos) {
        char* clock_pos_str = "clock_pos1";
        if (min_pos % 2 != 0) {
            clock_pos_str = "clock_pos2";

        }
        // std::cout << "min_pos: " << clock_pos_str << " ";
    }

    return min_num;
}

static int Count_Hash[2] = {-1, 1};
void Recent_Counter::CO_Init(const unsigned char *str, int length, unsigned long long num){
    unsigned int position;
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] +=
                Count_Hash[(str[length - 1] + position) & 1];
    }
}


int Recent_Counter::CO_Query(const unsigned char *str, int length){
    int* n = new int[hash_number];
    memset(n, 0, hash_number * sizeof(int));

    for(int i = 0;i < hash_number;++i)
    {
        unsigned int position = Hash(str, i, length) % row_length + i * row_length;
        /*
        if(clock_pos >= position){
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number + clock_pos - position);
        }else{
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number * 2 - (position - clock_pos));

        }*/
        n[i] = counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1] ;
        
    }

    std::sort(n, n + hash_number);

    return Mid(n);
}

unsigned int Recent_Counter::DelayedInsertion_CM_Query(const unsigned char* str, int length) {
    int min_num = 0x7fffffff;
    for(int i = 0;i < hash_number;i++) {
        min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].count[0], min_num);
        //std::cout << counter[Hash(str, i, length) % row_length + i * row_length].count[0] << std::endl;
    }
    return min_num;
}

void Recent_Counter::Clock_Go(unsigned long long int num){
    for(;last_time < num;++last_time){
        // std::cout << "num: " << num << " clock_pos: " << clock_pos << " clock_pos2: " << clock_pos2 << std::endl;
        // std::cout << "cycle_num: " << cycle_num << " cycle_num2: " << cycle_num2 << std::endl;

        if (clock_pos % 2 == 0) {
            counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        }
        clock_pos = (clock_pos + 1) % len;
        if(clock_pos == 0){
            cycle_num = (cycle_num + 1) % field_num;
        }

        int c_p2 = (int)clock_pos2;
        int p_p2 = (int)prev_clock_pos2;
        // 正負で場合分けする
        // 負の場合はc_p2を書き換える？
        int n;
        if (c_p2 > p_p2) {
            n = (c_p2 - p_p2);
        } else {
            // 1+8-7=2
            n = (c_p2 + len - p_p2);
        }
        for(int i=0;i<n;i++) {
            int pos = (p_p2 + i + 1) % len; 
            if (pos % 2 != 0) {
                counter[pos].count[(cycle_num2 + 1) % field_num] = 0;
            }
        }

        prev_clock_pos2 = clock_pos2;
        clock_pos2 = fmodf(clock_pos2 + 1.1, len);
        c_p2 = (int)clock_pos2;
        p_p2 = (int)prev_clock_pos2;
        if(c_p2 == 0 || ((p_p2 > c_p2) && (c_p2 < 2))){
            cycle_num2 = (cycle_num2 + 1) % field_num;
        }
    }
}


void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
    Initilize_ElementCount(length, num * step);

    std::string str2 = std::string((char*)str); // string型へ変換

    if (element_count_.count(str2) > 0) {
        element_count_[str2]++;
        //std::cout << element_count_[str2] << std::endl;  
    } else {
        element_count_[str2] = 1;
        //std::cout << element_count_[str2] << std::endl;  
    }
    //std::cout << str << std::endl;
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num) {
    unsigned int position;
    //std::cout << element_count_step_ << std::endl;
    for(;last_time < num;++last_time){
        //std::cout << num << ":" << last_time << std::endl;
        if (last_time % element_count_step_) {
            //std::cout << "hoge" << std::endl;
            for (int i = 0; i < hash_number; i++) {
                int frequency_confirmations[row_length] = {0};
                
                for (auto itr = element_count_.begin(); itr != element_count_.end(); itr++) {
                    // キャッシュの衝突を検知する
                    auto uchrs = reinterpret_cast<unsigned char*>(const_cast<char*>(itr->first.c_str()));

                    position = Hash(uchrs, i, length) % row_length;
                    frequency_confirmations[position] = max(frequency_confirmations[position], (*itr).second);
                }

                for (int j = 0; j < row_length; j++) {
                    if (frequency_confirmations[j] > 0) {
                        //std::cout << "hoge" << std::endl;
                        counter[j + i * row_length].count[0] = counter[j + i * row_length].count[0] + frequency_confirmations[j];
                    }
                }
                //std::cout << "end2" << std::endl;
            }
            element_count_.clear();
        }
    }
    //std::cout << "end3" << std::endl;
}

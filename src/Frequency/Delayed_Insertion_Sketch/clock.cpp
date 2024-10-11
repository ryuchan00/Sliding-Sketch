#include "clock.h"

#include <math.h>

#include <algorithm>
#include <cstring>

struct Place {
  unsigned int serial;
  unsigned int pos;
};
bool operator<(Place a, Place b) {
  return a.serial < b.serial;
}

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num, int element_count_step) : Recent_Sketch(c, l, _row_length, _hash_numberber, _field_num), element_count_step_(element_count_step) {
  counter = new Unit[l];
  counter_DE = new Unit2[l];
  field_num = _field_num;
  row_length = _row_length;
  for (int i = 0; i < l; i++) {
    counter[i].count = new int[_field_num];
    counter[i].field_num = _field_num;
    memset(counter[i].count, 0, _field_num * sizeof(int));
  }
}

Recent_Counter::~Recent_Counter() {
  delete[] counter;
}

unsigned int Recent_Counter::DelayedInsertion_CM_Query(const unsigned char* str, int length) {
  int min_num = 0x7fffffff;
  std::string str2 = std::string((char*)str);  // string型へ変換
  
  // 補正カウント
  int correction_count = 0;
#ifdef NOT_USE_CORRECTION_SKETCH
#else
  correction_count = element_count_2_.at(GetTargetKey(str));
  // std::cout << "correction_count: " << correction_count << std::endl;
#endif  // NOT_USE_CORRECTION_SKETCH

  for (int i = 0; i < hash_number; i++) {
    unsigned int position = Hash(str, i, length) % row_length + i * row_length;
    int new_filed = (cycle_num + (position < clock_pos)) % field_num;

    min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].count[new_filed] + correction_count, min_num);
    // std::cout << "min_num: " << min_num << std::endl;
  }
  // std::cout << "counter:" << counter[0].count[0] << "+ correction_count: " << correction_count << std::endl;
  return min_num;
}

void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
  // std::cout << "step:" << step << std::endl;

  Initilize_ElementCount(length, num * step);
  Clock_Go(num * step);

#ifdef NOT_USE_CORRECTION_SKETCH
  unsigned int position;

  for(int i = 0;i < hash_number;++i){
    position = Hash(str, i, length) % row_length + i * row_length;
    counter[position].count[0] += 1;
  }
#else
  if (element_count_2_.count(GetTargetKey(str)) != 0) {
    element_count_2_.at(GetTargetKey(str))++;
  } else {
    element_count_2_.insert(std::make_pair(GetTargetKey(str), 1));
  }
  //std::cout << num << ":" << element_count_2_.at(GetTargetKey(str)) << std::endl;
#endif  // NOT_USE_CORRECTION_SKETCH
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num) {
  unsigned int position;
  int frequency_confirmations[row_length] = {0};
  // std::cout << "num:" << num << std::endl;

  for (; last_time2 < num; ++last_time2) {
#ifdef NOT_USE_CORRECTION_SKETCH

#else
    int new_counter = 0;
    int old_counter = 0;

    if (last_time2 % element_count_step_ == 0) {
    // if (num % element_count_step_ == 0) {
      for (int i = 0; i < hash_number; i++) {
        frequency_confirmations[row_length] = {0};

        for (const auto& [key, value] : element_count_2_) {
          // キャッシュの衝突を検知する
          unsigned char a[DATA_LEN];
          for (int i = 0; DATA_LEN > i; i++) {
            a[i] = key[i];
          }
          position = Hash(a, i, length) % row_length;
          // std::cout << position << std::endl;
          if (frequency_confirmations[position] > 0) {
            // std::cout << "Collision detected" << std::endl;
          }
          frequency_confirmations[position] = max(frequency_confirmations[position], value);
          // if (frequency_confirmations[position] > 0) {
          //   std::cout << "frequency_confirmations[" << position << "] = " << frequency_confirmations[position] << std::endl;
          // }
        }

        for (int j = 0; j < row_length; j++) {
          if (frequency_confirmations[j] > 0) {
            int new_field = (cycle_num + (position < clock_pos)) % field_num;
            int old_field = (cycle_num + (position < clock_pos) + 1) % field_num;

            // oldとnewにfrequency_confirmations[j]を分割する
            if (num - counter[j + i * row_length].recently_reset_time >= element_count_step_) {
              counter[j + i * row_length].count[old_field] = counter[j + i * row_length].count[old_field] + frequency_confirmations[j];
            } else {
              old_counter = frequency_confirmations[j] * (num - counter[j + i * row_length].recently_reset_time) / element_count_step_;
              new_counter = frequency_confirmations[j] - old_counter;
              counter[j + i * row_length].count[old_field] = counter[j + i * row_length].count[old_field] + old_counter;
              counter[j + i * row_length].count[new_field] = counter[j + i * row_length].count[new_field] + new_counter;
            }

            // std::cout << "counter[" << j + i * row_length << "].count[0] = " << counter[j + i * row_length].count[0] << std::endl;
          }
        }
      }
      element_count_2_.clear();
    }
#endif  // NOT_USE_CORRECTION_SKETCH
  }
}

// todo: DATA_LEN=8に依存しているコードなので，DETA_LENに合わせてできるようにする
packet_str Recent_Counter::GetTargetKey(const unsigned char* str) {
  //return {str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]};
  return {str[0], str[1], str[2], str[3]};
}

void Recent_Counter::Clock_Go(unsigned long long int num) {
  for (; last_time < num; ++last_time) {
    counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
    clock_pos = (clock_pos + 1) % len;

    counter[clock_pos].recently_reset_time = num / step;  // 最後にリセットされた時間を記録

    if (clock_pos == 0) {
      cycle_num = (cycle_num + 1) % field_num;
    }
  }
}

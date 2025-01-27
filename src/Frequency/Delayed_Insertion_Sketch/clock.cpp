#include "clock.h"

#include <math.h>

#include <algorithm>
#include <cstring>
// #include <iomanip> // setprecisionを使用するのに必要


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
#endif  // NOT_USE_CORRECTION_SKETCH

  std::chrono::high_resolution_clock::time_point start_time;
  std::chrono::high_resolution_clock::time_point end_time;
  start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < hash_number; i++) {
#ifdef ONLY_INPUT_MODE
    min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].Total() + correction_count, min_num);
#else
    unsigned int position = Hash(str, i, length) % row_length + i * row_length;
    int new_filed  = 0;

    min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].Total() + correction_count, min_num);
#endif  // ONLY_INPUT_MODE
  }

  end_time = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  query_time += duration;

  return min_num;
}
using std::micro;

void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
  std::chrono::high_resolution_clock::time_point start_time;
  std::chrono::high_resolution_clock::time_point end_time;
  std::chrono::microseconds duration;
  // std::chrono::duration<int, micro> duration;

  start_time = std::chrono::high_resolution_clock::now();

  Initilize_ElementCount(length, num * step, num);

  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  insertion_time += duration;

  Clock_Go(num * step);

#ifdef NOT_USE_CORRECTION_SKETCH
  int position;

  for(int i = 0;i < hash_number;++i){
    position = Hash(str, i, length) % row_length + i * row_length;
    counter[position].count[0] += 1;
  }
#else

  std::chrono::system_clock::time_point s1,s2,s3,s4;
  std::chrono::system_clock::time_point e1,e2,e3,e4;

  packet_str key = GetTargetKey(str);

  start_time = std::chrono::system_clock::now();

  s1 = std::chrono::system_clock::now();
  s4 = std::chrono::system_clock::now();
  bool cond = element_count_2_.count(key) > 0;
  e4 = std::chrono::system_clock::now();
  time4 += std::chrono::duration_cast<std::chrono::microseconds>(e4 - s4);
  if (cond) {
    s2 = std::chrono::system_clock::now();

    element_count_2_.at(key) += 1;

    e2 = std::chrono::system_clock::now();
    std::chrono::microseconds duration2 = std::chrono::duration_cast<std::chrono::microseconds>(e2 - s2);
    time2 += duration2; 
  } else {
    // e1 = std::chrono::high_resolution_clock::now();
    // time1 += std::chrono::duration_cast<std::chrono::microseconds>(e1 - s1);

    s3 = std::chrono::system_clock::now();

    element_count_2_.insert(std::make_pair(key, 1));

    e3 = std::chrono::system_clock::now();
    time3 += std::chrono::duration_cast<std::chrono::microseconds>(e3 - s3);
  }
  e1 = std::chrono::system_clock::now();
  time1 += std::chrono::duration_cast<std::chrono::microseconds>(e1 - s1);

  end_time = std::chrono::system_clock::now();
  // duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // insertion_time += duration;
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  increment_time = increment_time + std::chrono::duration_cast<std::chrono::microseconds>(e4 - s4)
  + std::chrono::duration_cast<std::chrono::microseconds>(e2 - s2) + std::chrono::duration_cast<std::chrono::microseconds>(e3 - s3);
  insertion_time = insertion_time + std::chrono::duration_cast<std::chrono::microseconds>(e4 - s4)
  + std::chrono::duration_cast<std::chrono::microseconds>(e2 - s2) + std::chrono::duration_cast<std::chrono::microseconds>(e3 - s3);
#endif  // NOT_USE_CORRECTION_SKETCH

  // start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < hash_number; i++) {
    int position = Hash(str, i, length) % row_length;
    // if (hash_count[i].count(position) > 0) {
    //   hash_count[i][position] += 1;
    // } else {
    //   hash_count[i].insert(std::make_pair(position, 1));
    // }
  }

  // end_time = std::chrono::high_resolution_clock::now();
  // duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // insertion_time += duration;
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num, unsigned long long int time) {
  std::chrono::high_resolution_clock::time_point return_write_start_time,insertion_correct_sketch_start_time,detect_hash_collision_start_time,initilize_element_count_start_time;
  std::chrono::high_resolution_clock::time_point return_write_end_time,insertion_correct_sketch_end_time,detect_hash_collision_end_time,initilize_element_count_end_time;
  std::chrono::microseconds duration;

  int position;
  // int frequency_confirmations[row_length] = {0};
  // std::vector<int> frequency_confirmations(row_length, 0);
  // int time = num / step;

  // initilize_element_count_start_time = std::chrono::high_resolution_clock::now();

  // for (; last_time2 < num; ++last_time2) {
#ifdef NOT_USE_CORRECTION_SKETCH

#else
    int new_counter = 0;
    int old_counter = 0;
    // insertion_correct_sketch_start_time = std::chrono::high_resolution_clock::now();

    if (time != 0 && time % element_count_step_ == 0) {

      for (int i = 0; i < hash_number; i++) {
        frequency_confirmations.assign(row_length, 0);

        int count = 0;

        // detect_hash_collision_start_time = std::chrono::high_resolution_clock::now();

        for (const auto& [key, value] : element_count_2_) {
          // キャッシュの衝突を検知する
          unsigned char a[DATA_LEN];
          for (int j = 0; DATA_LEN > j; j++) {
            a[j] = key[j];
          }
          position = Hash(a, i, length) % row_length;
          frequency_confirmations[position] = max(frequency_confirmations[position], value);
        }

        // detect_hash_collision_end_time = std::chrono::high_resolution_clock::now();
        // duration = std::chrono::duration_cast<std::chrono::microseconds>(detect_hash_collision_end_time - detect_hash_collision_start_time);
        // detect_hash_collision_time += duration;

        // return_write_start_time = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < row_length; j++) {
          int counter_position = j + i * row_length;

          if (frequency_confirmations[j] > 0) {
#ifdef ONLY_INPUT_MODE
            counter[counter_position].count[0] = counter[counter_position].count[0] + frequency_confirmations[j];
#else
            int new_field = 0;
            int old_field = 1;

            // oldとnewにfrequency_confirmations[j]を分割する
            int real_time = num / step;
            if (real_time >= counter[counter_position].recently_reset_time + element_count_step_) {
              counter[counter_position].count[new_field] = counter[counter_position].count[new_field] + frequency_confirmations[j];
            } else {
              double rate = static_cast<double>(real_time - counter[counter_position].recently_reset_time) / static_cast<double>(element_count_step_);
              new_counter = frequency_confirmations[j] * rate;

              old_counter = frequency_confirmations[j] - new_counter;
              counter[counter_position].count[old_field] = counter[counter_position].count[old_field] + old_counter;
              counter[counter_position].count[new_field] = counter[counter_position].count[new_field] + new_counter;
            }
#endif // ONLY_INPUT_MODE
          }
        }
        // return_write_end_time = std::chrono::high_resolution_clock::now();
        // duration = std::chrono::duration_cast<std::chrono::microseconds>(return_write_end_time - return_write_start_time);
        // return_write_time += duration;
      }
      element_count_2_.clear();

      // insertion_correct_sketch_end_time = std::chrono::high_resolution_clock::now();
      // duration = std::chrono::duration_cast<std::chrono::microseconds>(insertion_correct_sketch_end_time - insertion_correct_sketch_start_time);
      // insertion_correct_sketch_time += duration;
    }

    // initilize_element_count_end_time = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(initilize_element_count_end_time - initilize_element_count_start_time);
    // initilize_element_count_time += duration;

#endif  // NOT_USE_CORRECTION_SKETCH
  // }
}

// todo: DATA_LEN=8に依存しているコードなので，DETA_LENに合わせてできるようにする
packet_str Recent_Counter::GetTargetKey(const unsigned char* str) {
  // std::chrono::high_resolution_clock::time_point start_time;
  // std::chrono::high_resolution_clock::time_point end_time;
  // std::chrono::microseconds duration;
  // start_time = std::chrono::high_resolution_clock::now();

  using packet = std::array<unsigned char, DATA_LEN>;
  packet result{};
  for (std::size_t i = 0; i < DATA_LEN; ++i) {
      result[i] = str[i];
  }
  packet_str p = result;

  // end_time = std::chrono::high_resolution_clock::now();
  // duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // GetTargetKey_time += duration;

  return result;
}

void Recent_Counter::Clock_Go(double num) {
  // for (; last_time < num; ++last_time) {
  //   counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
  //   clock_pos = (clock_pos + 1) % len;

  //   counter[clock_pos].recently_reset_time = num / step;  // 最後にリセットされた時間を記録

  //   if (clock_pos == 0) {
  //     cycle_num = (cycle_num + 1) % field_num;
  //   }
  // }

  for (; last_time < num; ++last_time) {
    counter[clock_pos].count[1] = counter[clock_pos].count[0];
    counter[clock_pos].count[0] = 0;
    clock_pos = (clock_pos + 1) % len;

    counter[clock_pos].recently_reset_time = num / step;  // 最後にリセットされた時間を記録
  }
}

// 比較関数
bool compareByValue(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second > b.second; // 値に基づいて降順で比較
}

/// @brief Hashカウントをダンプする
void Recent_Counter::DumpHashCount() {

  for(int i = 0;i < hash_number;++i){
    // std::cout << "===========================hash_count[" << i << "]:" << i << std::endl;
    //int count = std::count_if(hash_count[i].begin(), hash_count[i].end(), [](int x) { return x != 0; });
    // std::cout << "hash_count[" << i << "].size():" << hash_count[i].size() << std::endl;

    // std::mapをstd::vectorにコピー
    std::vector<std::pair<int, int>> vec(hash_count[i].begin(), hash_count[i].end());

    // 値に基づいてソート
    std::sort(vec.begin(), vec.end(), compareByValue);

    for (const auto& pair : vec) {
      //std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    // for (const auto& pair : hash_count[i]) {
    //     std::cout << "キー: " << pair.first << ", 値: " << pair.second << std::endl;
    // }
  }
}

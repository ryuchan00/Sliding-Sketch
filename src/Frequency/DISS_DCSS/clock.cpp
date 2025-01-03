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

  for (int i = 0; i < hash_number; i++) {
#ifdef ONLY_INPUT_MODE
    min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].Total() + correction_count, min_num);
#else
    unsigned int position = Hash(str, i, length) % row_length + i * row_length;
    //int new_filed = (cycle_num + (position < clock_pos)) % field_num;
    int new_filed  = 0;

    min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].Total() + correction_count, min_num);
#endif  // ONLY_INPUT_MODE
  }
  return min_num;
}

void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
  Initilize_ElementCount(length, num * step);
  Clock_Go(num * step);

#ifdef NOT_USE_CORRECTION_SKETCH
  int position;

  for(int i = 0;i < hash_number;++i){
    position = Hash(str, i, length) % row_length + i * row_length;
    counter[position].count[0] += 1;
  }
#else
  if (element_count_2_.count(GetTargetKey(str)) > 0 ) {
    element_count_2_.at(GetTargetKey(str)) += 1;
    // std::cout << "count:" << element_count_2_.at(GetTargetKey(str)) << std::endl;
  } else {
    element_count_2_.insert(std::make_pair(GetTargetKey(str), 1));
  }
#endif  // NOT_USE_CORRECTION_SKETCH

  for (int i = 0; i < hash_number; i++) {
    int position = Hash(str, i, length) % row_length;
    if (hash_count[i].count(position) > 0) {
      hash_count[i][position] += 1;
    } else {
      hash_count[i].insert(std::make_pair(position, 1));
    }
  }
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num) {
  int position;
  // int frequency_confirmations[row_length] = {0};
  std::vector<int> frequency_confirmations(row_length, 0);

  for (; last_time2 < num; ++last_time2) {
#ifdef NOT_USE_CORRECTION_SKETCH

#else
    int new_counter = 0;
    int old_counter = 0;

    if (last_time2 != 0 && last_time2 % element_count_step_ == 0) {
      for (int i = 0; i < hash_number; i++) {
        // todo vectorを試してみる
        // frequency_confirmations[row_length] = {0};
        frequency_confirmations.assign(row_length, 0);

        // std::cout << "e c 2 size: " << element_count_2_.size() << std::endl;
        int count = 0;
        for (const auto& [key, value] : element_count_2_) {
          // キャッシュの衝突を検知する
          unsigned char a[DATA_LEN];
          for (int j = 0; DATA_LEN > j; j++) {
            a[j] = key[j];
          }
          position = Hash(a, i, length) % row_length;

          // if (frequency_confirmations[position] > 0) {
          //   std::cout << "Collision detected" << std::endl;
          //   std::cout << "key: " << a << std::endl;
          //   std::cout << "position: " << position << std::endl;
          //   count++;
          // } else {
          //   std::cout << "Collision not detected" << std::endl;
          // }

          frequency_confirmations[position] = max(frequency_confirmations[position], value);
        }

        for (int j = 0; j < row_length; j++) {
            int counter_position = j + i * row_length;

          if (frequency_confirmations[j] > 0) {
#ifdef ONLY_INPUT_MODE
            counter[counter_position].count[0] = counter[counter_position].count[0] + frequency_confirmations[j];
#else
            // int new_field = (cycle_num + (counter_position < clock_pos)) % field_num;
            int new_field = 0;
            // int old_field = (cycle_num + (counter_position < clock_pos) + 1) % field_num;
            int old_field = 1;

            // oldとnewにfrequency_confirmations[j]を分割する
            // std::cout << "num: " << num / step << std::endl;
            // std::cout << "counter[counter_position].recently_reset_time + element_count_step_: " << counter[counter_position].recently_reset_time + element_count_step_ << std::endl;
            int real_time = num / step;
            if (real_time >= counter[counter_position].recently_reset_time + element_count_step_) {
              counter[counter_position].count[new_field] = counter[counter_position].count[new_field] + frequency_confirmations[j];
            } else {
              // new_counter = 0;
              // new_counter = frequency_confirmations[j];
              double rate = static_cast<double>(real_time - counter[counter_position].recently_reset_time) / static_cast<double>(element_count_step_);
              // std::cout << "real_time: "  <<  real_time << " reset_time:" << counter[counter_position].recently_reset_time << " element_count_step_:" << element_count_step_ << std::endl;
              // std::cout << "rate: "  <<  rate << std::endl;
              new_counter = frequency_confirmations[j] * rate;

              // new_counter = frequency_confirmations[j] * 1.0;
              old_counter = frequency_confirmations[j] - new_counter;
              counter[counter_position].count[old_field] = counter[counter_position].count[old_field] + old_counter;
              counter[counter_position].count[new_field] = counter[counter_position].count[new_field] + new_counter;
            }
#endif // ONLY_INPUT_MODE
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
  using packet = std::array<unsigned char, DATA_LEN>;
  packet result{};
  for (std::size_t i = 0; i < DATA_LEN; ++i) {
      result[i] = str[i];
  }
  packet_str p = result;
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
    if (clock_pos % 2 == 0) {
        counter[clock_pos].count[1] = counter[clock_pos].count[0];
        counter[clock_pos].count[0] = 0;
        counter[clock_pos].recently_reset_time = num / step;  // 最後にリセットされた時間を記録
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
          counter[pos].count[1] = counter[pos].count[0];
          counter[pos].count[0] = 0;
          counter[pos].recently_reset_time = num / step;  // 最後にリセットされた
      }
    }

    prev_clock_pos2 = clock_pos2;
    clock_pos2 = fmodf(clock_pos2 + 1.1, len);
    // c_p2 = (int)clock_pos2;
    // p_p2 = (int)prev_clock_pos2;
    // if(c_p2 == 0 || ((p_p2 > c_p2) && (c_p2 < 2))){
    //     cycle_num2 = (cycle_num2 + 1) % field_num;
    // }

    // counter[clock_pos].count[1] = counter[clock_pos].count[0];
    // counter[clock_pos].count[0] = 0;
    // clock_pos = (clock_pos + 1) % len;

  }
}

// 比較関数
bool compareByValue(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second > b.second; // 値に基づいて降順で比較
}

/// @brief Hashカウントをダンプする
void Recent_Counter::DumpHashCount() {

  for(int i = 0;i < hash_number;++i){
    std::cout << "===========================hash_count[" << i << "]:" << i << std::endl;
    //int count = std::count_if(hash_count[i].begin(), hash_count[i].end(), [](int x) { return x != 0; });
    std::cout << "hash_count[" << i << "].size():" << hash_count[i].size() << std::endl;

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

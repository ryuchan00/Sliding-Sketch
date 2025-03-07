#include "clock.h"

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

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num):
    Recent_Sketch(c,l,_row_length,_hash_numberber,_field_num){
    counter = new Unit [l];
    field_num = _field_num;
    for(int i = 0; i < l; i++){
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }

#ifdef CHECK_COLLISION_HASH
    collision_hash_.resize(hash_number);
#endif // CHECK_COLLISION_HASH
}

Recent_Counter::~Recent_Counter(){
    delete [] counter;
}

void Recent_Counter::CM_Init(const unsigned char* str, int length, unsigned long long int num){
    unsigned int position;
    Clock_Go((double)num * step);

    std::chrono::high_resolution_clock::time_point insertion_start_time;
    std::chrono::high_resolution_clock::time_point insertion_end_time;
    std::chrono::microseconds duration;
    insertion_start_time = std::chrono::high_resolution_clock::now();

    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;

#ifdef CHECK_COLLISION_HASH
        if (collision_hash_.at(i).find(position) != collision_hash_.at(i).end()) {
            in_str in = {str[0],str[1],str[2],str[3]};
            auto result = std::find(collision_hash_.at(i).at(position).begin(), collision_hash_.at(i).at(position).end(), in);
            if (result == collision_hash_.at(i).at(position).end()) {
                // std::cout << "not found" << std::endl;
                collision_hash_.at(i).at(position).push_back(in);
                collision_count_++;
            } else {
                // std::cout << "found" << std::endl;
            }
        } else {    
            std::vector <in_str> vec = {{str[0],str[1],str[2],str[3]}};
            collision_hash_.at(i).insert(std::make_pair(position, vec));
        }

#endif // CHECK_COLLISION_HASH

    }

    insertion_end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(insertion_end_time - insertion_start_time);
    insertion_time += duration;
}

void Recent_Counter::CU_Init(const unsigned char* str, int length, unsigned long long int num){
    int k = clock_pos / row_length;
    Clock_Go((double)num * step);
    unsigned int position = Hash(str, k ,length) % row_length + k * row_length;
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

unsigned int Recent_Counter::Query(const unsigned char* str, int length){
    unsigned int min_num = 0x7fffffff;
    unsigned int position;

#ifdef GET_USING_FREQUENCY_PREIOD
    // いくつ離れているか？
    int distance = 0;
    unsigned int before_min_num = 0x7fffffff;
    int min_hash_number = 0;
    int current_index;

    // current_index = (clock_pos / row_length + hash_number) % hash_number;
    // position = Hash(str, current_index, length) % row_length + current_index * row_length;

    // std::cout << "---" << std::endl;
    // std::cout << "before_current_index: " << current_index << std::endl;

    int init_num;   // 初期のハッシュ関数の探索場所
    int new_clock_pos;
    // if (Hash(str, current_index, length) % row_length < clock_pos % row_length) {
    //     new_clock_pos = (clock_pos - row_length + len) % len;
    // } else {
        new_clock_pos = clock_pos;
    // }
    // std::cout << "---" << std::endl;
#endif // GET_USING_FREQUENCY_PREIOD

    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    start_time = std::chrono::high_resolution_clock::now();

    for(int i = 0;i < hash_number;++i){
#ifdef CHECK_COLLISION_HASH
        position = Hash(str, i, length) % row_length + i * row_length;

        if(collision_hash_.at(i).at(position).size() > 1) {
            collision_element_access_count_++;
        }
#endif // CHECK_COLLISION_HASH

#ifdef GET_USING_FREQUENCY_PREIOD
        before_min_num = min_num;
#endif // GET_USING_FREQUENCY_PREIOD
        position = Hash(str, i, length) % row_length + i * row_length;

        min_num = min(counter[position].Total(), min_num);

#ifdef GET_USING_FREQUENCY_PREIOD
        // current_index = (new_clock_pos / row_length - i + hash_number) % hash_number;
        // std::cout << "current_index: " << current_index << std::endl;

        // position = Hash(str, current_index, length) % row_length + current_index * row_length;

        if (counter[position].Total() <= before_min_num) {
            // min_hash_number = current_index;
            min_hash_number = i;
        }
        // std::cout << "min_num: " << min_num << std::endl;
        // std::cout << "min_hash_number: " << min_hash_number << std::endl;
#endif // GET_USING_FREQUENCY_PREIOD

    }

#ifdef GET_USING_FREQUENCY_PREIOD
    distance = (clock_pos / row_length - min_hash_number + hash_number) % hash_number;
    // std::cout << "----------------------: " << min_hash_number << std::endl;
    hit_counts[distance]++;
#endif // GET_USING_FREQUENCY_PREIOD

    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    query_time += duration;

    return min_num;
}

static int Count_Hash[2] = {-1, 1};
void Recent_Counter::CO_Init(const unsigned char *str, int length, unsigned long long num){
    unsigned int position;
    Clock_Go((double)num * step);
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

void Recent_Counter::Clock_Go(double num){
    // std :: cout << "num:" << num << std::endl;    
    for(;last_time < (unsigned long long int)num;++last_time){
        counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        clock_pos = (clock_pos + 1) % len;
        if(clock_pos == 0){
            cycle_num = (cycle_num + 1) % field_num;
        }
    }
}


void Recent_Counter::DumpHitCount() {
    for (size_t i = 0; i < hit_counts.size(); ++i) {
        std::cout << i << ":" << hit_counts[i] << std::endl;
    }
}
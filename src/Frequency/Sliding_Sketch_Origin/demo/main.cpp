#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <chrono> // 時間計測用ライブラリ

#include "../clock.h"
#include "../data.h"
#include "../../../csv.h"
#include "../../../ssv.h"

using namespace std;
unordered_map<Data, int, My_Hash> mp;

// #define USE_SAMPLE_DATA

//argv[1]:cycle
//argv[2]:hash_number
//argv[3]:Memory
//argv[4]:file
//argv[5]:input_num_max
//argv[6]:out_file
//argv[7]:out_model
//argv[8]:label_name
//argv[9]:field

void Read_File(int argc, char* argv[]){
    //int cycle = 50000;
    // int cycle = 50000;
    int cycle = std::atoi(argv[4]);
    // int cycle = 40000;
    int hash_number = 10;
    //double mymemory  = stoi(argv[2]);
    double mymemory  = std::atof(argv[2]);
    // double mymemory  = 3;
    // int input_num_max = 5000000;
    // int input_num_max = 50000;
    int input_num_max = std::atoi(argv[3]);
    int field_num = 2;
    int row_length = (mymemory * 1024 * 1024) / hash_number / (4 * field_num);
    int amari = (hash_number * row_length) % cycle;
    std::cout << "amari:" << amari << std::endl;

    int repeat_num = (argc > 5) ? std::atoi(argv[5]) : 1;

    int overestimation_count = 0;
    int underestimation_count = 0;
    
    //row_length = hash_number * row_length / temp_step;
    
    // int row_length = 1;

#ifdef USE_SAMPLE_DATA
    FILE* file = fopen("../../../../data/formatted00.dat","rb");
#else
    FILE* file = fopen("../../../../data/sx-stackoverflow.txt","rb");
#endif  // USE_SAMPLE_DATA
    Data packet;

    //std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/sx-stackoverflow.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial2.txt");
    // tsvに合わせたやる必要がある？
    //std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/webdocs.dat");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial3.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial4.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv(argv[1]);
    // std::cout << "input size" << input.size() << std::endl;
    std::vector<int> input2 = Ssv2::ReadSsv(argv[1]);
    // std::cout << "input size" << input2.size() << std::endl;

    std::vector<int> insertion_times;
    std::vector<int> query_times;

    for(int j = 0; j < repeat_num; ++j){

    // Recent_Counter CM_Counter(cycle, hash_number * row_length - amari, row_length - amari / hash_number, hash_number, field_num);
    Recent_Counter CM_Counter(cycle, hash_number * row_length, row_length, hash_number, field_num);
    // Recent_Counter CU_Counter(cycle, hash_number * row_length, row_length, hash_number, field_num);
    // Recent_Counter CO_Counter(2*cycle/3, hash_number * row_length, row_length, hash_number, field_num);

    Data *dat = new Data[cycle + 1];

    unsigned long long int num = 0;
    double CM_dif = 0, CU_dif = 0, CO_dif = 0;
    double CM_ae = 0,  CU_ae = 0,  CO_ae = 0;
    double CM_re = 0,  CU_re = 0,  CO_re = 0;
        
    cout <<"Sliding Sketch,Arrivals,ARE"<<endl;

    // 時間計測開始
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

#ifdef USE_SAMPLE_DATA
    while(fread(packet.str, DATA_LEN, 1, file) > 0) {
#else
    // for (int i = 0; i < input.size(); i++) {
    for (int i = 0; i < input2.size(); i++) {
        // std::memcpy(packet.str, &input[i][0], DATA_LEN);
        std::memcpy(packet.str, &input2[i], DATA_LEN);
#endif  // USE_SAMPLE_DATA

        // for(int j = 0; j < DATA_LEN; ++j) {
        //   std::cout << std::hex << static_cast<int>(packet.str[j]) << " ";
        // }
        //cout << endl;

        if(num > input_num_max){
            break;
        }

        unsigned int pos = num % cycle;
        if(num >= cycle){
            mp[dat[pos]] -= 1;
        }


        dat[pos] = packet;

        CM_Counter.CM_Init(packet.str, DATA_LEN, num);
        // CU_Counter.CU_Init(packet.str, DATA_LEN, num);
        // CO_Counter.CO_Init(packet.str, DATA_LEN, num);

        if(mp.find(packet) == mp.end())
            mp[packet] = 1;
        else
            mp[packet] += 1;

        int CM_guess = CM_Counter.Query(packet.str, DATA_LEN);
        // int CU_guess = CU_Counter.Query(packet.str, DATA_LEN);
        // int CO_guess = CO_Counter.CO_Query(packet.str, DATA_LEN);

        double real = mp[packet];

        int CM_sub = CM_guess - real;
        // int CU_sub = CU_guess - real;
        // int CO_sub = CO_guess - real;

        CM_dif += abs(CM_sub);
        // CU_dif += abs(CU_sub);
        // CO_dif += abs(CO_sub);

        CM_re = CM_re + (double)abs(CM_sub) / real;
        // CU_re = CU_re + (double)abs(CU_sub) / real;
        // CO_re = CO_re + (double)abs(CO_sub) / real;

        CM_ae += abs(CM_sub);
        // CU_ae += abs(CU_sub);
        // CO_ae += abs(CO_sub);

        if (CM_guess > real) {
            // cout << " <===== Overestimation" << endl;
            overestimation_count++;
        } else if (CM_guess < real) {
            // cout << " <===== Underestimation" << endl;
            underestimation_count++;
        } else {
            // cout << endl;
        }


        // if(num%cycle ==0){
        //  cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        // // // cout << "time:" << num << " input_num: " << input[i][0] << "," << " guess: " << CM_guess << "," << " real: "<< real << endl;
        // // // // cout << "Sl-CU" << "," << num << "," << CU_re / num << endl;
        // // // // cout << "Sl-Count" << "," << num << "," << CO_re / num << endl;
        // }

        if(num%input_num_max ==0){
            cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        }

        num++;

        
    }

    // 時間計測終了
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    cout << "ARE" << "," << num << "," << CM_re / num << endl;

    // 実行時間を表示
    cout << "Execution time for the loop: " << duration.count() << " ms" << endl;
    cout << "Insertion Time," << CM_Counter.insertion_time.count() << endl;
    cout << "Query Time," << CM_Counter.query_time.count() << endl;
    insertion_times.push_back(CM_Counter.insertion_time.count());
    query_times.push_back(CM_Counter.query_time.count());

#ifdef CHECK_COLLISION_HASH
    cout << "collision count" << CM_Counter.collision_count_ << endl;
    cout << "collision access count" << CM_Counter.collision_element_access_count_ << endl;
#endif // CHECK_COLLISION_HASH
    // CM_Counter.DumpHitCount();
    }

    // パラメータダンプ
    cout << "DATA_LEN:" << DATA_LEN << endl;
    cout << "input_num_max:" << input_num_max << endl;
    cout << "Cycle:" << cycle << endl;
    cout << "Memory:" << mymemory << endl;
    cout << "Hash:" << hash_number << endl;
    cout << "Row Length:" << row_length << endl;

    cout << "Insertion times: ";
    for (const auto& time : insertion_times) {
        cout << time << ",";
    }
    cout << endl;

    cout << "Query times: ";
    for (const auto& time : query_times) {
        cout << time << ",";
    }
    cout << endl;

    cout << "Overestimation:" << overestimation_count << endl;
    cout << "Underestimation:" << underestimation_count << endl;
    cout << "Match:" << input_num_max - overestimation_count - underestimation_count << endl;
}

int main(int argc, char* argv[]){
    Read_File(argc, argv);

#ifdef HASH_COLLISION
    cout << "HASH_COLLISION" << endl;
#endif  // HASH_COLLISION
}

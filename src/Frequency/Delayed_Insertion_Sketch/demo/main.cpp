#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <string.h>
#include "../clock.h"
#include "../data.h"
#include "../../../csv.h"
#include "../../../ssv.h"

using namespace std;
unordered_map<Data, int, My_Hash> mp;


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
    int cycle = std::atoi(argv[4]);
    // 所持しているハッシュ関数
    int hash_number = 10;
    double mymemory  = std::atof(argv[2]);
    int input_num_max = std::atoi(argv[3]);
    // バケットの数(today or yesterday)
    int field_num = 2;
    // 衝突の関係する
    int row_length = (mymemory * 1024 * 1024) / hash_number / (4 * field_num);
    // row_lenghtをcycleの倍数にする
    int amari = (hash_number * row_length) % cycle;
    std::cout << "amari:" << amari << std::endl;
    
    int element_count_step = std::atoi(argv[5]);
    row_length = ((mymemory * 1024 * 1024) - (DATA_LEN * element_count_step + 4 * element_count_step)) /hash_number / (4 * field_num);
    // if (hash_number * row_length < cycle) {
    //     std::cout << "hash_number: " << hash_number << std::endl;
    //     std::cout << "row_length: " << row_length << std::endl;
    //     std::cout << "Should hash_number * row_length >= cycle" << std::endl;
    //     std::cout << "You should adjust memory param" << std::endl;

    //     return;
    // }
    // Recent_Counter CM_Counter(cycle, hash_number * row_length - amari, row_length - amari / hash_number, hash_number, field_num, element_count_step);
    Recent_Counter CM_Counter(cycle, hash_number * row_length, row_length, hash_number, field_num, element_count_step);

    Data *dat = new Data[cycle + 1];

    unsigned long long int num = 0;
    double CM_dif = 0, CU_dif = 0, CO_dif = 0;
    double CM_ae = 0,  CU_ae = 0,  CO_ae = 0;
    double CM_re = 0,  CU_re = 0,  CO_re = 0;

    FILE* file = fopen("../../../../data/formatted00.dat","rb");
    Data packet;

    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial2.txt");
    //std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial3.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv(argv[1]);
    std::vector<int> input2 = Ssv2::ReadSsv(argv[1]);

    cout <<"Sliding Sketch,Arrivals,ARE"<<endl;
    // cout << "num,diff,guess,real " << endl;

    int overestimation_count = 0;
    int underestimation_count = 0;

    // while(fread(packet.str, DATA_LEN, 1, file) > 0)
    // for (int i = 0; i < input.size(); i++)
    // {
    //     std::memcpy(packet.str, &input[i][0], DATA_LEN);
    for (int i = 0; i < input2.size(); i++) {
        std::memcpy(packet.str, &input2[i], DATA_LEN);
        // cout << "INPUT: " << packet.str << endl;
        

        if(num > input_num_max){
            break;
        }

        unsigned int pos = num % cycle;
        if(num >= cycle){
            mp[dat[pos]] -= 1;
        }


        dat[pos] = packet;
        // バイナリデータ？
        // cout << packet.str << endl;

        CM_Counter.DelayedInsertion_CM_Init(packet.str, DATA_LEN, num);

        if(mp.find(packet) == mp.end())
            mp[packet] = 1;
        else
            mp[packet] += 1;

        int CM_guess = CM_Counter.DelayedInsertion_CM_Query(packet.str, DATA_LEN);

        double real = mp[packet];

        int CM_sub = CM_guess - real;
        double diff = (double)CM_guess - real;

        CM_dif += abs(CM_sub);

        CM_re = CM_re + (double)abs(CM_sub) / real;

        CM_ae += abs(CM_sub);

        // if(num%cycle ==0){
        //     cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        // }
        // cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        // cout << "input_num: " << input[i][0] << "," << " guess: " << CM_guess << "," << " real: "<< real;
        if (CM_guess > real) {
            // cout << " <===== Overestimation" << endl;
            overestimation_count++;
        } else if (CM_guess < real) {
            // cout << " <===== Underestimation" << endl;
            underestimation_count++;
        } else {
            // cout << endl;
        }

        if(num%input_num_max ==0){
            cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        }

        // }

        // 終わり50個前から出力して、over estimationかunder estimationかを確認する
        // todo: clock_pos1かclock_pos2のどちらの管理区域か出力する必要がありそう
        // if (input_num_max - 50 < num) {
            //  cout << num << "," << diff << "," << CM_guess << "," << real << endl;
        // }


        num++;

    }

    cout << "ARE" << "," << num << "," << CM_re / num << endl;

    cout << "Execution insertion time for the loop," << CM_Counter.insertion_time.count() << endl;
    cout << "Execution query time for the loop," << CM_Counter.query_time.count() << endl;
    cout << "return_write_time," << CM_Counter.return_write_time.count() << endl;
    cout << "detect_hash_collision_time," << CM_Counter.detect_hash_collision_time.count() << endl;
    cout << "insertion_correct_sketch_time," << CM_Counter.insertion_correct_sketch_time.count() << endl;
    cout << "GetTargetKey_time," << CM_Counter.GetTargetKey_time.count() << endl;
    cout << "initilize_element_count_time," << CM_Counter.initilize_element_count_time.count() << endl;
    cout << "increment_time," << CM_Counter.increment_time.count() << endl;
    cout << "time1," << CM_Counter.time1.count() << endl;
    cout << "time2," << CM_Counter.time2.count() << endl;
    cout << "time3," << CM_Counter.time3.count() << endl;
    cout << "time4," << CM_Counter.time4.count() << endl;

    // 現在の時刻を取得
    std::time_t now = std::time(nullptr);

    // 現在の時刻をローカルタイムに変換
    std::tm* localTime = std::localtime(&now);

    // フォーマットされた日付を出力
    std::cout << "date: "
            << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") // 年-月-日 時:分:秒
            << std::endl;

    // パラメータダンプ
    cout << "DATA_LEN:" << DATA_LEN << endl;
    cout << "input_num_max:" << input_num_max << endl;
    cout << "Cycle:" << cycle << endl;
    cout << "Memory:" << mymemory << endl;
    cout << "Hash:" << hash_number << endl;
    cout << "Row Length:" << row_length << endl;
    cout << "element_count_step:" << element_count_step << endl;
    cout << "Overestimation:" << overestimation_count << endl;
    cout << "Underestimation:" << underestimation_count << endl;
    cout << "Match:" << input_num_max - overestimation_count - underestimation_count << endl;

    CM_Counter.DumpHashCount();
}

int main(int argc, char* argv[]){
    Read_File(argc, argv);

#ifdef HASH_COLLISION
    cout << "HASH_COLLISION" << endl;
#endif  // HASH_COLLISIONq

#ifdef NOT_USE_CORRECTION_SKETCH
    cout << "NOT_USE_CORRECTION_SKETCH" << endl;
#endif  // NOT_USE_CORRECTION_SKETC
}

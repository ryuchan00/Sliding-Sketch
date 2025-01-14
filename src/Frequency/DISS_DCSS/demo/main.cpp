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
     int cycle = 50000;
    // 所持しているハッシュ関数
    int hash_number = 10;
    double mymemory  = std::atof(argv[2]);
    int input_num_max = std::atoi(argv[3]);
    // バケットの数(today or yesterday)
    int field_num = 2;
    // 衝突の関係する
    int row_length = (mymemory * 1024 * 1024) / hash_number / (4 * field_num);
    // 補正スケッチの更新間隔    
    int element_count_step = std::atoi(argv[4]);;

    Recent_Counter CM_Counter(cycle, hash_number * row_length, row_length, hash_number, field_num, element_count_step);

    Data *dat = new Data[cycle + 1];

    unsigned long long int num = 0;
    double CM_dif = 0;
    double CM_ae = 0;
    double CM_re = 0;

    Data packet;

    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial2.txt");
    // std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial3.txt");
    std::vector<std::vector<int>> input = Csv::ReadCsv(argv[1]);
    // std::vector<int> input2 = Ssv2::ReadSsv(argv[1]);

    cout <<"Sliding Sketch,Arrivals,ARE"<<endl;

    int overestimation_count = 0;
    int underestimation_count = 0;

    // while(fread(packet.str, DATA_LEN, 1, file) > 0)
    // sx-stackoverflow.txtのデータを読み込む
    for (int i = 0; i < input.size(); i++)
    {
        std::memcpy(packet.str, &input[i][0], DATA_LEN);
    // Webdocs.datのデータを読み込む
    // for (int i = 0; i < input2.size(); i++) {
    //     std::memcpy(packet.str, &input2[i], DATA_LEN);

        if(num > input_num_max){
            break;
        }

        unsigned int pos = num % cycle;
        if(num >= cycle){
            mp[dat[pos]] -= 1;
        }


        dat[pos] = packet;

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

        if(num%cycle ==0){
            cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        }

        if (CM_guess > real) {
            // cout << " <===== Overestimation" << endl;
            overestimation_count++;
        } else if (CM_guess < real) {
            // cout << " <===== Underestimation" << endl;
            underestimation_count++;
        } else {
            // cout << endl;
        }

        num++;
    }

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

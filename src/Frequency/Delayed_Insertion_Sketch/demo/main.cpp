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
    //int cycle = 5;
     int cycle = 50000;
    // 所持しているハッシュ関数
    int hash_number = 10;
    // int hash_number = 2;    // テスト用
    // double mymemory  = 1;
    // double mymemory  = 1;
    double mymemory  = stoi(argv[1]);;
    int input_num_max = 50000;
    // int input_num_max = 500000;
    // int input_num_max = 500;
    // バケットの数(today or yesterday)
    int field_num = 2;
    // 衝突の関係する
    int row_length = (mymemory * 1024 * 1024) / hash_number / (4 * field_num);
    // int row_length = 1;
    
    int element_count_step = 5000;
    // int element_count_step = 500;
    // int row_length = 4;    // テスト用
    // hash_number * row_lengthはスケッチ全体のサイズ
    // std::cout << "row_length:" << row_length << std::endl;
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
    std::vector<std::vector<int>> input = Csv::ReadCsv("../../../../data/artificial4.txt");

    cout <<"Sliding Sketch,Arrivals,ARE"<<endl;
    // cout << "num,diff,guess,real " << endl;

    // while(fread(packet.str, DATA_LEN, 1, file) > 0)
    for (int i = 0; i < input.size(); i++)
    {
        // cout << "INPUT: " << packet.str << endl;
        std::memcpy(packet.str, &input[i][0], DATA_LEN);

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
        // cout << "Sl-CM" << "," << num << "," << CM_re / num << endl;
        cout << input[i][0] << ":" << CM_guess << "," << real << endl;
        // }

        // 終わり50個前から出力して、over estimationかunder estimationかを確認する
        // todo: clock_pos1かclock_pos2のどちらの管理区域か出力する必要がありそう
        // if (input_num_max - 50 < num) {
            //  cout << num << "," << diff << "," << CM_guess << "," << real << endl;
        // }


        num++;

    }
    cout << "DATA_LEN:" << DATA_LEN << endl;
}

int main(int argc, char* argv[]){
    Read_File(argc, argv);
}

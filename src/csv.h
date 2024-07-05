#pragma once

#include <fstream>
#include <iostream>
#include <sstream>  //for istringstream
#include <string>
#include <vector>

class Csv {
   public:
    /// <summary>
    /// CSVファイルをデリミタでスプリット
    /// </summary>
    /// <param name="input">インプット文字列</param>
    /// <param name="delimiter">デリミタ</param>
    /// <returns>CSVの各項目をvectorにして返却</returns>
    static std::vector<std::string> Split(std::string& input, char delimiter) {
        std::istringstream stream(input);
        std::string field;
        std::vector<std::string> result;

        while (getline(stream, field, delimiter)) {
            result.push_back(field);
        }

        return result;
    }

    /// <summary>
    /// CSVファイルを読み込む
    /// </summary>
    /// <param name="filepath">読み込みファイルパス</param>
    /// <returns>ステージオブジェクトの番号の配列</returns>
    static std::vector<std::vector<int>> ReadCsv(std::string filepath) {
        // ファイルストリームを宣言と同時にファイルを開く
        std::ifstream stream(filepath, std::ios::out);

        std::vector<std::vector<int>> output;

        if (!stream) {
            return output;
        }

        // ファイルから読み込む
        std::string line;

        while (getline(stream, line)) {  // １行ずつ読み出す
            std::vector<std::string> line_vec = Split(line, ' ');

            std::vector<int> line_objects;

            for (int i = 0; i < static_cast<int>(line_vec.size()); i++) {
                if (line_vec[i] == "") {
                    continue;
                }

                line_objects.push_back(std::stoi(line_vec[i]));
            }
            output.push_back(line_objects);
        }

        // ファイルを閉じる
        stream.close();

        return output;
    }
};
#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <memory>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

#define DataFramePtr shared_ptr<DataFrame>


const string WHITESPACE = " \n\r\t\f\v";

string trim(const string &s) {
    string str;
    size_t start = s.find_first_not_of(WHITESPACE);
    str =  (start == string::npos) ? "" : s.substr(start);

    size_t end = str.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : str.substr(0, end + 1);
}

void split(const string &line, vector<string>&vec, string delimiters = ","){
    string cell;
    for (int i = 0; i < line.size(); i++)
    {
        if (delimiters.find(line[i]) != std::string::npos){
            if(!cell.empty()) vec.push_back(cell);
            cell = "";
        }
        else
        {
            cell += line[i];
        }
    }
    vec.push_back(cell);
}
 

struct DataFrame
{
    vector<vector<string>> data;
    vector<map<string, int>> unique_values;
    vector<map<string, map<string, int>>> label_map;

    DataFrame() {}

    DataFrame(DataFrame &df, int index, string value)
    {
        for (int i = 0; i < df.nRows(); i++)
        {
            if (df.data[i][index] == value)
                add_row(df.data[i]);
        }
    }

    void clear_all()
    {
        data.clear();
        unique_values.clear();
        label_map.clear();
    }

    void train_test_split(DataFrame &train, DataFrame &test, double train_size = 0.8)
    {
        vector<vector<string>> temp_data = data;
        test.clear_all();
        train.clear_all();
        unsigned seed = chrono::system_clock::now()
                            .time_since_epoch()
                            .count();
        shuffle(data.begin(), data.end(), default_random_engine(seed));

        int train_rows = train_size * nRows();
        for (int i = 0; i < train_rows; i++)
            train.add_row(data[i]);
        for (int i = train_rows; i < nRows(); i++)
            test.add_row(data[i]);
    }

    void read_from_file(string filename, string delimiter = ",")
    {
        // clear
        data.clear();
        unique_values.clear();
        label_map.clear();

        ifstream file(filename);
        string line;
        while (getline(file, line))
        {
            if (line.size() == 0) continue;
            vector<string> row;
            split(line, row, delimiter);
            add_row(row);
        }

        file.close();
    }

    void add_row(vector<string> row)
    {
        data.push_back(row);

        // if the first row
        if (data.size() == 1)
        {
            unique_values = vector<map<string, int>>(row.size());
            label_map = vector<map<string, map<string, int>>>(row.size());
        }

        for (int i = 0; i < row.size(); i++)
        {
            unique_values[i][row[i]]++;

            if (i != row.size() - 1)
            {
                label_map[i][row[i]][row[row.size() - 1]]++;
            }
        }
    }

    void print_unique(int index)
    {
        for (auto it = unique_values[index].begin(); it != unique_values[index].end(); it++)
        {
            cout << it->first << " " << it->second << "----" << endl;

            for (auto it2 = label_map[index][it->first].begin(); it2 != label_map[index][it->first].end(); it2++)
            {
                cout << it2->first << " " << it2->second << endl;
            }
        }

        cout << endl;
    }

    int nCols()
    {
        return data[0].size();
    }

    int nRows()
    {
        return data.size();
    }

    double entropy(map<string, int> &mp)
    {
        double entropy = 0;
        int total = 0;
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            total += it->second;
        }
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            double p = (double)it->second / total;
            entropy += p * log2(p);
        }
        return -entropy;
    }

    double information_gain(int index)
    {
        // calculate entropy of the whole dataset
        double total_entropy = entropy(unique_values[nCols() - 1]);

        // calculate remainder
        double remainder = 0;
        for (auto it = unique_values[index].begin(); it != unique_values[index].end(); it++)
        {
            remainder += (double)it->second / nRows() * entropy(label_map[index][it->first]);
        }

        // cout<<total_entropy<<" "<<remainder<<endl;

        return total_entropy - remainder;
    }

    map<string, int> get_unique_values(int index)
    {
        return unique_values[index];
    }

    map<string, map<string, int>> get_label_map_for_index(int index)
    {
        return label_map[index];
    }
};


#endif
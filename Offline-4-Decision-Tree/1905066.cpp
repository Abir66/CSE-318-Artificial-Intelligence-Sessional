#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "dataframe.h"
#include "descision-tree.h"

using namespace std;

int main()
{

    int cnt = 20;

    double best_accuracy = 0;
    double total_accuracy = 0;
    double std_deviation = 0;
    DecisionTreePtr best_tree = nullptr;
    
    for (int i = 0; i < cnt; i++)
    {

        DataFrame df, train, test;

        df.read_from_file("car-evaluation-dataset/car.data");

        df.train_test_split(train, test, 0.8);

        DescisionTree dt(train, 1);
        //DescisionTree dt("tree.txt");

        int correct = 0;
        for (int i = 0; i < test.nRows(); i++)
        {
            string prediction = dt.predict(test.data[i]);
            if (prediction == test.data[i][test.nCols() - 1])
                correct++;
        }

        double accuracy = (double)correct / test.nRows();
        total_accuracy += accuracy;
        std_deviation += accuracy * accuracy;
        cout<<accuracy<<endl;

        if (accuracy > best_accuracy)
        {
            best_accuracy = accuracy;
            best_tree = make_shared<DescisionTree>(dt);
        }
    }

    double avg_accuracy = total_accuracy / cnt;
    std_deviation = sqrt(std_deviation / cnt - avg_accuracy * avg_accuracy);
    //cout<< avg_accuracy << "," << best_accuracy << "," << std_deviation << endl;
    cout<< "avg accuracy: " << avg_accuracy << endl;
    cout<< "best accuracy: " << best_accuracy << endl;
    cout<< "std deviation: " << std_deviation << endl;


    //best_tree->export_tree("tree.txt");

    return 0;
}
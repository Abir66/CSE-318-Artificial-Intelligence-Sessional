#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "dataframe.h"
#include "descision-tree.h"

using namespace std;



int main()
{

    DataFrame df, train, test;

    df.read_from_file("car-evaluation-dataset/car.data");
    df.train_test_split(train, test, 0.8);

    df.clear_all();

    DescisionTree dt(train);

    // test accuracy
    int correct = 0;
    for (int i = 0; i < test.nRows(); i++)
    {   
        string prediction = dt.predict(test.data[i]);
        if (prediction == test.data[i][test.nCols() - 1])
            correct++;
    }

    cout << "Accuracy: " << (double)correct / test.nRows() << endl;


    return 0;
}
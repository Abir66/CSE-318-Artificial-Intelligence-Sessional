#ifndef DESCISIONTREE_H
#define DESCISIONTREE_H


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <memory>
#include "dataframe.h"

using namespace std;


#define NodePtr shared_ptr<Node>

// descison tree implementation
struct Node
{
    int attribute_Index = -1;
    string attribute_name;
    string output;
    bool isLeaf;
    vector<NodePtr> children;
    NodePtr parent = nullptr;
    int popularity_count = 0;
    string popularity_value;
};

struct DescisionTree
{
    NodePtr root;
    // DataFramePtr df;
    vector<int> attributes;
    DataFrame originalData;

    DescisionTree(DataFrame &df)
    {
        root = make_shared<Node>();
        originalData = df;
        int attribute_size = df.nCols() - 1;
        attributes = vector<int>(attribute_size);
        for (int i = 0; i < attribute_size; i++)
        {
            attributes[i] = i;
        }
        buildTree(root, df, attributes);
    }

    string predict(string row, string delimiter = ",")
    {
        vector<string> row_vector;
        string cell;
        for (int i = 0; i < row.size(); i++)
        {
            if (row[i] == delimiter[0])
            {
                row_vector.push_back(cell);
                cell = "";
            }
            else
            {
                cell += row[i];
            }
        }
        row_vector.push_back(cell);

        return predict(row_vector);
    }

    string predict(vector<string> row)
    {
        NodePtr current = root;
        bool found = false;
        while (true)
        {
            if(current->isLeaf)
                break;
            string value = row[current->attribute_Index];

            for (auto child : current->children)
            {   
                if (child->attribute_name == value)
                {
                    current = child;
                    found = true;
                    break;
                }
            }

        }
        return current->output;
    }

    void buildTree(NodePtr root, DataFrame &df, vector<int> &attributes, int level = 0)
    {   

        //--------------------------if examples is empty--------------------------
        if (df.nRows() == 0)
        {
            root->isLeaf = true;
            root->output = root->parent->popularity_value;
            return;
        }

        // if all the rows have the same output
        if (df.get_unique_values(df.nCols() - 1).size() == 1)
        {
            root->isLeaf = true;
            root->output = df.data[0][df.nCols() - 1];
            return;
        }

        //--------------------------set popularity--------------------------
        map<string, int> popularity = df.get_unique_values(df.nCols() - 1);
        int max_popularity = 0;
        string max_popularity_value;
        for (auto it = popularity.begin(); it != popularity.end(); it++)
        {
            if (it->second > max_popularity)
            {
                max_popularity = it->second;
                max_popularity_value = it->first;
            }
        }

        root->popularity_count = max_popularity;
        root->popularity_value = max_popularity_value;

        //-------------------------- if no attributes left--------------------------
        if (attributes.empty())
        {
            root->isLeaf = true;
            root->output = max_popularity_value;
            //root->attribute_name = "leaf";
            return;
        }

        int best_attribute_index = 0;
        double best_gain = 0;
        for (int i = 0; i < attributes.size(); i++)
        {
            double gain = df.information_gain(attributes[i]);
            if (gain > best_gain)
            {
                best_gain = gain;
                best_attribute_index = i;
            }
        }


        auto unique_values = originalData.get_unique_values(attributes[best_attribute_index]);
        vector<int> new_attributes(attributes);
        new_attributes.erase(new_attributes.begin() + best_attribute_index);

        root->attribute_Index = attributes[best_attribute_index];
        root->isLeaf = false;

        for (auto it = unique_values.begin(); it != unique_values.end(); it++)
        {

            NodePtr node = make_shared<Node>();
            node->parent = root;
            node->isLeaf = false;
            //node->attribute_Index = attributes[best_attribute_index];
            node->attribute_name = it->first;
            DataFrame new_df(df, attributes[best_attribute_index], it->first);
            buildTree(node, new_df, new_attributes, level + 1);
            root->children.push_back(node);
            
        }

    }


    void printTree(NodePtr root, int level = 0)
    {
        for (int i = 0; i < level; i++)
        {
            cout << "----";
        }
        if (root->isLeaf)
        {
            cout <<root->attribute_Index <<"-"<<root->attribute_name<<"  ---->  "<<root->output << endl;
            return;
        }

        cout <<root->attribute_Index <<"-"<<root->attribute_name << endl;

        for (auto child : root->children)
        {
            printTree(child, level + 1);
        }
    }

};

#endif
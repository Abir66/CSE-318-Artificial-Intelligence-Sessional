#ifndef DESCISIONTREE_H
#define DESCISIONTREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <memory>
#include <cstring>
#include "dataframe.h"

using namespace std;

#define NodePtr shared_ptr<Node>
#define DecisionTreePtr shared_ptr<DescisionTree>

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
    int total = 0;
};

class DescisionTree
{

    NodePtr root;
    vector<int> attributes;
    DataFramePtr originalData;
    double threshold = 1;
    int max_level = 100;

public:
    
    int node_count = 0, inner_node_count = 0;

    DescisionTree() {}
    
    DescisionTree(string filename) { import_tree(filename); }

    DescisionTree(DataFrame &df, double threshold = 1, int max_level = 100)
    {
        root = make_shared<Node>();
        originalData = make_shared<DataFrame>(df);
        int attribute_size = df.nCols() - 1;
        attributes = vector<int>(attribute_size);
        for (int i = 0; i < attribute_size; i++)
        {
            attributes[i] = i;
        }
        this->threshold = threshold;
        this->max_level = max_level;
        buildTree(root, df, attributes);
        // don't need the original data anymore
        originalData = nullptr;
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

        while (true)
        {
            if (current->isLeaf)
                break;
            string value = row[current->attribute_Index];
            bool found = false;

            for (auto child : current->children)
            {
                if (child->attribute_name == value)
                {
                    current = child;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                return current->popularity_value;
            }
        }
        return current->output;
    }

    void export_tree(string filename)
    {
        ofstream file;
        file.open(filename);
        printTree(root, 0, file);
        file.close();
    }

    void print_tree(ostream &out = std::cout)
    {
        printTree(root, 0, out);
    }

    void buildTree(NodePtr root, DataFrame &df, vector<int> &attributes, int level = 0)
    {

        node_count++;

        if (level > max_level)
        {
            root->isLeaf = true;
            root->output = root->parent->popularity_value;
            return;
        }

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
        int total = 0;
        string max_popularity_value;
        for (auto it = popularity.begin(); it != popularity.end(); it++)
        {
            if (it->second > max_popularity)
            {
                max_popularity = it->second;
                max_popularity_value = it->first;
            }
            total += it->second;
        }

        root->popularity_count = max_popularity;
        root->popularity_value = max_popularity_value;
        root->total = total;

        

        if(max_popularity * 1.0 / total > threshold)
        {
            root->isLeaf = true;
            root->output = max_popularity_value;
            return;
        }


        //-------------------------- if no attributes left--------------------------
        if (attributes.empty())
        {
            root->isLeaf = true;
            root->output = max_popularity_value;
            // root->attribute_name = "leaf";
            return;
        }

        inner_node_count++;

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

        auto unique_values = originalData->get_unique_values(attributes[best_attribute_index]);
        vector<int> new_attributes(attributes);
        new_attributes.erase(new_attributes.begin() + best_attribute_index);

        root->attribute_Index = attributes[best_attribute_index];
        root->isLeaf = false;

        for (auto it = unique_values.begin(); it != unique_values.end(); it++)
        {

            NodePtr node = make_shared<Node>();
            node->parent = root;
            node->isLeaf = false;
            // node->attribute_Index = attributes[best_attribute_index];
            node->attribute_name = it->first;
            DataFrame new_df(df, attributes[best_attribute_index], it->first);
            buildTree(node, new_df, new_attributes, level + 1);
            root->children.push_back(node);
        }
    }

private:
    void printTree(NodePtr root, int level = 0, ostream &out = std::cout)
    {
        for (int i = 0; i < level; i++)
        {
            out << "\t";
        }
        if (root->isLeaf)
        {
            out << root->attribute_name << "  ---->  " << root->output << endl;
            return;
        }

        if (level == 0)
            out << "root," <<root->total << "," << root->popularity_value << "," << root->popularity_count << "," << root->attribute_Index << endl;

        else
            out << root->attribute_name << "," <<root->total << "," << root->popularity_value << "," << root->popularity_count << "," << root->attribute_Index << endl;

        for (auto child : root->children)
        {
            printTree(child, level + 1, out);
        }
    }

    void import_tree(string filename)
    {
        ifstream file(filename);
        string line;
        NodePtr current = nullptr;
        int prev_level = 0;
        root = nullptr;
        while (getline(file, line))
        {
            if (line.size() == 0)
                continue;
            vector<string> row;

            int level = 0;
            for (int i = 0; i < line.size(); i++)
            {
                if (line[i] == '\t')
                    level++;
                else
                    break;
            }

            split(trim(line), row, ",");

            NodePtr node = make_shared<Node>();

            if (row.size() > 1)
            {
                node->attribute_name = row[0];
                node->total = stoi(row[1]);
                node->popularity_value = row[2];
                node->popularity_count = stoi(row[3]);
                node->attribute_Index = stoi(row[4]);
            }

            // else leaf
            else
            {
                node->isLeaf = true;
                string s = row[0];
                split(s, row, " ");
                node->attribute_name = row[1];
                node->output = row[3];
            }

            if (root == nullptr)
            {
                root = node;
                current = root;
                continue;
            }

            if (level > prev_level)
            {
                current->children.push_back(node);
                node->parent = current;
                current = node;
            }

            else if (level == prev_level)
            {
                current->parent->children.push_back(node);
                node->parent = current->parent;
                current = node;
            }

            else if (level < prev_level)
            {
                int diff = prev_level - level;
                while (diff--)
                {
                    current = current->parent;
                }
                current->parent->children.push_back(node);
                node->parent = current->parent;
                current = node;
            }

            prev_level = level;
        }

        file.close();
    }
};

#endif
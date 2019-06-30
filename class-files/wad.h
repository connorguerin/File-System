//
// Created by Connor on 3/23/2019.
//

#ifndef P3_WAD_H
#define P3_WAD_H
#include <string>
#include <vector>
#include <regex>
#include "tree.h"
using namespace std;

class Wad {
public:
    static Wad* loadWad(const string &path);
    char* getMagic();
    bool isContent(const string &path);
    bool isDirectory(const string &path);
    int getSize(const string &path);
    int getContents(const string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const string &path, vector<string> *directory);
    void createTree();
    vector<string> directoryNames(const string &path);
    TreeNode* findNode(vector<string> fileNames, TreeNode *current);
    ~Wad();
private:
    vector<int> header;
    char* magic;
    int numberDescriptors;
    int dOffset;
    tree* fs;
    vector<vector<int>> descriptors;
    vector <char> data;
    vector<string> descriptorNames;
    string filePath;
};
#endif //P3_WAD_H


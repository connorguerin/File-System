//
// Created by Connor on 3/24/2019.
//

#ifndef P3_TREE_H
#define P3_TREE_H

#include <vector>
#include <string>
using namespace std;
struct TreeNode{
    vector<TreeNode*> children;
    TreeNode* parent;
    int offset;
    int length;
    bool isDir;
    string name;
    ~TreeNode(){

	for (int i = 0; i < children.size(); i++){
	    delete children[i];
	    children[i] = nullptr;
      }
    }
};

class tree {
public:
    tree();
    TreeNode* root;
    ~tree();
};


#endif //P3_TREE_H


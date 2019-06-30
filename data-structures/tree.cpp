//
// Created by Connor on 3/24/2019.
//

#include "tree.h"

tree::tree() {
    root = new TreeNode;
    root->name = "root";
    root->length = 0;
    root->isDir = true;
    root->offset = 0;
    vector<TreeNode*> theKids;
    root->children = theKids;
}
tree::~tree() {
 delete root;
}

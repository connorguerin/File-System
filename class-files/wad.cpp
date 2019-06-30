//
// Created by Connor on 3/23/2019.
//

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "wad.h"
char *Wad::getMagic() {
    return this->magic;
}

Wad::~Wad(){
    if (fs)
	delete fs;
}
Wad *Wad::loadWad(const string &path) {
    ifstream infile(path);
    if (infile) {
        Wad *retWad = new Wad;
        retWad->filePath = path;
        infile.seekg(0, infile.end);
        int length = infile.tellg();
        infile.seekg(0, infile.beg);


        char * mag = new char[4];
        char * descriptors = new char[4];
        char * descriptorOffset = new char[4];

        infile.read(mag, 4);
        infile.read(descriptors, 4);
        infile.read(descriptorOffset, 4);


        int numberOfDescriptors = *((int*) descriptors);
        int dOffset = *((int*) descriptorOffset);
        retWad->magic = mag;
        retWad->dOffset = dOffset;
        retWad->numberDescriptors = numberOfDescriptors;



        char * elementOffset = new char[4];
        char * elementLength = new char[4];
        char * charName = new char[9];
        infile.seekg(dOffset);

        uint32_t eO;
        uint32_t eL;

        int i = 0;
        while (infile.tellg() != -1 ) {
            infile.read(elementOffset, 4);
            infile.read(elementLength, 4);
            infile.read(charName, 8);

            eO = *((uint32_t*) elementOffset);
            eL = *((uint32_t*) elementLength);
            string name = string(charName);

            vector<int> add;
            add.push_back(eO);
            add.push_back(eL);
            retWad->descriptors.push_back(add);
            retWad->descriptorNames.push_back(name);
            //cout << retWad->descriptorNames[i] << " " <<  "\n";
            i++;


        }
        delete[] mag;
        delete[] descriptors;
        delete[] descriptorOffset;
        delete[] elementLength;
        delete[] elementOffset;
        delete[] charName;

        retWad->createTree();
	//infile.close();
        return retWad;
    }
    else
        return nullptr;
}

bool Wad::isContent(const string &path) {
    vector<string> files;
    files = directoryNames(path);
    TreeNode *leaf = findNode(files, this->fs->root);
    //cout << leaf->name << endl;
    if (leaf)
        return !(leaf->isDir);
    else
	return 0;
}

bool Wad::isDirectory(const string &path) {
    vector<string> files;
    files = directoryNames(path);
    TreeNode *leaf = findNode(files, this->fs->root);
    //cout << leaf->name << endl;
    if (leaf)
        return leaf->isDir;
    else
        return 0;
}

int Wad::getSize(const string &path) {
    if (isContent(path)) {
        vector<string> files;
    	files = directoryNames(path);
	TreeNode *leaf = findNode(files, this->fs->root);
	return leaf->length;
	}
    else {
        return -1;
    }

}

int Wad::getContents(const string &path, char *buffer, int length, int offset) {
    if (isContent(path)) {
        ifstream infile(this->filePath);
	infile.seekg(0, infile.beg);
        vector<string> files = directoryNames(path);
        TreeNode *leaf = findNode(files, this->fs->root);
        //cout << leaf->name;
	int dataOffset = leaf->offset;
	int dataLength = leaf->length;
	int o = offset;
	int l = length;
	if (offset > leaf->length){
	   return -1;
	}
	else {
              infile.seekg(dataOffset + offset);

            if ((leaf->length) < (offset + length)) {
		int readLength = (leaf->length) - (offset);
		char *retbuf1 = new char[readLength];
		infile.read(retbuf1, readLength);
		memcpy(buffer, retbuf1, readLength);
		delete[] retbuf1;
		return readLength;
	    }
	    else {
		char *retbuf2 = new char[length];
	        infile.read(retbuf2, length);
		//cout << retbuf2 << endl;
		memcpy(buffer,retbuf2,length);
		delete[] retbuf2;
                return length;
	    }
	}
    }
    else {
        return -1;
    }
}

int Wad::getDirectory(const string &path, vector<string> *directory) {
    if (isDirectory(path)) {
        vector<string> files = directoryNames(path);
        TreeNode *leaf = findNode(files, this->fs->root);
        if (leaf) {
            for (int i = 0; i < leaf->children.size(); i++) {
                directory->push_back(leaf->children[i]->name);
            }
            return leaf->children.size();
        }
        return -1;
    }
    else {
        return -1;
    }
}

void Wad::createTree() {
    tree *retTree = new tree();
    TreeNode* current = retTree->root;
    regex mapMarker("^(E)([0-9])(M)([0-9])$");
    regex nsStart("^(.*)(_START)$");
    regex nsEnd("^(.*)(_END)$");

    for (int i = 0; i < descriptorNames.size() - 1; i++) {
        //cout << i << endl;
        string s = descriptorNames[i];

        if (regex_match(s, nsStart)){
            auto *newNode = new TreeNode;
            newNode->offset = descriptors[i][0];
            newNode->length = descriptors[i][1];
            newNode->isDir = true;
	    s.resize(s.size() - 6);
            newNode->name = s;
            newNode->parent = current;

            current->children.push_back(newNode);
            current = newNode;
        }
        else if (regex_match(s, nsEnd)) {
            //auto *newNode = new TreeNode;
            //newNode->offset = descriptors[i][0];
            //newNode->length = descriptors[i][1];
            //newNode->isDir = true;
            //newNode->name = s;
            //newNode->parent = current->parent;
            current = current->parent;
            //current->children.push_back(newNode);
        }

        else if (regex_match(s,mapMarker)) {
            auto *node = new TreeNode;
            node->offset = descriptors[i][0];
            node->length = descriptors[i][1];
            node->isDir = true;
            node->name = s;
            node->parent = current;

            current->children.push_back(node);
            current = node;

            int k = i;
            for (int j = i + 1; j < i + 11; j++){

                auto *newNode = new TreeNode;
                newNode->offset = descriptors[j][0];
                newNode->length = descriptors[j][1];
                newNode->isDir = false;
                newNode->name = descriptorNames[j];
                newNode->parent = current;

                current->children.push_back(newNode);
                k++;
            }
            i = k;
            current = current->parent;
        }

        else {
            auto *newNode = new TreeNode;
            newNode->offset = descriptors[i][0];
            newNode->length = descriptors[i][1];
            newNode->isDir = false;
            newNode->name = s;
            newNode->parent = current;

            current->children.push_back(newNode);
        }
    }
	this->fs = retTree;
}

vector<string> Wad::directoryNames(const string &path) {
    vector<string> files(0);
    string delimiter = "/";
    string s(path);
    if (s == "/")
	return files;

    s = s.substr(1, s.length() - 1);
    size_t pos = 0;
    string sub;
    while (pos != -1) {
        pos = s.find(delimiter);
        if (pos != -1) {
            sub = s.substr(0, pos);
            files.push_back(sub);
            s.erase(0, pos + delimiter.length());
        }
        else if (pos == -1 && s.length() > 0) {
            files.push_back(s);
        }
    }
    return files;
}

TreeNode *Wad::findNode(vector<string> fileNames, TreeNode *current) {
    if (fileNames.size() == 0) {
	return current;
    }

    else if (fileNames.size() == 1) {
	string tester = current->name;
        for (TreeNode *node : current->children) {
           tester= node->name;
	   if (fileNames[0] == node->name){
                return node;
	   }
        }
    }
    else {
        for (TreeNode *otherNode : current->children) {
            if (fileNames[0] == otherNode->name){
                current = otherNode;
		string test = otherNode->name;
                fileNames.erase(fileNames.begin());
                return findNode(fileNames, current);
            }
        }
    }
}


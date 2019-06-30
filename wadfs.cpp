#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include "wad.h"

Wad *theWad;
static fuse_operations fuse_implementation;

static int do_releasedir(const char *path, struct fuse_file_info *) {
	if (theWad->isDirectory(path))
		return 0;
	else
		return -1;
}

static int do_opendir(const char *path, struct fuse_file_info *fi){
	if (theWad->isDirectory(path))
		return 0;
	else
		return -1;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    vector<string> names;
    theWad->getDirectory(path, &names);
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);


    for (string s : names) {
        filler(buffer, const_cast<char*>(s.c_str()), NULL, 0);
    }

	return 0;
}









static int do_getattr( const char *path, struct stat *st ) {
	if (theWad->isDirectory(path))
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2;
	}
	else
	{
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = theWad->getSize(path);
	}

	return 0;

}

static int do_open(const char *path, struct fuse_file_info *fi) {
	if (theWad->isContent(path))
		return 0;
	else
		return -ENOENT;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	if (theWad->isContent(path)) {
		char* ducker = new char[theWad->getSize(path)];
		int retval = theWad->getContents(path, ducker, size, offset);
		memcpy(buffer, ducker, retval);
		return retval;
	}
	else {
		return -1;
	}
}

static int do_release(const char *path, struct fuse_file_info *) {
	if (theWad->isContent(path))
		return 0;
	else
		return -1;


}
int main(int argc, char *argv[]) {

	fuse_implementation.getattr = do_getattr;
	fuse_implementation.releasedir = do_releasedir;
	fuse_implementation.opendir = do_opendir;
	fuse_implementation.readdir = do_readdir;
	fuse_implementation.open = do_open;
	fuse_implementation.read = do_read;
	fuse_implementation.release = do_release;

	string currentDir = string(get_current_dir_name());
	string wadArg = string(argv[1]);
	if (wadArg.find(currentDir) != string::npos) {
		theWad = Wad::loadWad(argv[1]);
	}
	else {
		string delimiter("/");
		string path = currentDir + delimiter + wadArg;
		theWad = Wad::loadWad(path);
	}
	char* temp[] = {argv[0], argv[2]};
 	return fuse_main(argc - 1, temp, &fuse_implementation, NULL);
}

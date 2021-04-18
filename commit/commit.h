#ifndef COMMIT_H
#define COMMIT_H

#include <map>
#include "../constants/constants.h"

class Commit
{
    string _message;
    time_t _timestamp;
    fs::path _parent;
    string _uid;
    map<fs::path, fs::path> file_blobs;
};

#endif // COMMIT_H
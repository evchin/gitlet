#ifndef COMMIT_H
#define COMMIT_H

#include <map>
#include <ctime>
#include "../cereal/types/string.hpp"
#include "../cereal/types/map.hpp"
#include "../globals/globals.h"

class Commit
{
public:
    // PURPOSE: initial commit
    Commit();
    // PURPOSE: commit with message
    Commit(string message);
    // PURPOSE: return blob path given file path
    fs::path get_value(fs::path key);
    friend ostream& operator<<(ostream& outs, const Commit c);

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(_message, _timestamp, _parent, _uid, _blobs);
    }
private:
    string _message;
    time_t _timestamp;
    fs::path _parent;
    string _uid;
    map<fs::path, fs::path> _blobs;
};

#endif // COMMIT_H
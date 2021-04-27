#ifndef COMMIT_H
#define COMMIT_H

#include <map>
#include <ctime>
#include "../cereal/types/map.hpp"
#include "../globals/globals.h"

class Commit
{
public:
    // PURPOSE: initial commit
    Commit();
    // PURPOSE: clones commit c and reassigns message and timestamp
    Commit(const Commit& c, fs::path parent_path, string message);
    // PURPOSE: return blob path given file path
    fs::path get_value(fs::path key);
    // PURPOSE: set blob path value for a filepath key
    void set_value(fs::path key, fs::path value);
    // PURPOSE: remove key from map
    void remove_key(fs::path key);
    // PURPOSE: set uid
    void set_uid(string uid);
    // PURPOSE: return if file exists in commit
    bool tracks(fs::path file);
    // PURPOSE: return true if file is the exact same as file in commit
    bool matches(fs::path file);
    // PURPOSE: get parent commit's path
    fs::path parent_path();
    // PURPOSE: get message
    string get_message();
    // PURPOSE: get uid
    string get_uid();
    // PURPOSE: checkout commit in CWD (DANGEROUS!)
    void checkout();

    // PURPOSE: print commit
    friend ostream& operator<<(ostream& outs, const Commit c);
    // PURPOSE: for serialization
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(_message, _timestamp, _parent, _uid, _blobs);
    }

private:
    string _message;
    time_t _timestamp;
    fs::path _parent;
    string _uid = "";
    // map of file name to blob path
    map<fs::path, fs::path> _blobs;
};

#endif // COMMIT_H
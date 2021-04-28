#ifndef COMMIT_H
#define COMMIT_H

// #include <map>
// #include "../cereal/types/map.hpp"
#include <ctime>
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
    // PURPOSE: return true if modified files exist in CWD, returns modified files by reference
    bool check_for_modified(string& modified_files);
    // PURPOSE: return true if unstaged modified files exist in CWD, returns modified files by reference
    bool check_for_unstaged_modified(string& modified_files);
    // PURPOSE: return true if untracked files exist in CWD that are not IN commit, returns untracked files by reference
    bool check_for_untracked(string& untracked_files);
    // PURPOSE: get parent commit's path
    fs::path parent_path();
    // PURPOSE: get parent commit
    Commit get_parent();
    // PURPOSE: get message
    string get_message();
    // PURPOSE: get uid
    string get_uid();
    // PURPOSE: get branch
    string get_branch();
    // PURPOSE: checkout commit in CWD (DANGEROUS!)
    void checkout();

    // PURPOSE: print commit
    friend ostream& operator<<(ostream& outs, const Commit c);
    // PURPOSE: return true if equal
    friend bool operator==(const Commit lhs, const Commit rhs);
    // PURPOSE: return false if equal
    friend bool operator!=(const Commit lhs, const Commit rhs);
    // PURPOSE: return true if lhs was committed before rhs
    friend bool operator<(const Commit lhs, const Commit rhs);
    // PURPOSE: return true if lhs was committed after rhs
    friend bool operator>(const Commit lhs, const Commit rhs);
    // PURPOSE: for serialization
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(_message, _timestamp, _parent, _uid, _blobs, _branch);
    }

private:
    // message of commit
    string _message;
    // timestamp commited
    time_t _timestamp;
    // parent commit
    fs::path _parent;
    // sha1 hash
    string _uid = "";
    // commit's branch
    string _branch = "";
    // map of file name to blob path
    map<fs::path, fs::path> _blobs;
};

#endif // COMMIT_H
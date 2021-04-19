#include "commit.h"

Commit::Commit()
{
    _message = "initial commit";
    time_t t = time(0);
    _timestamp = t - t;
    string s = "";
    _uid = get_hash(s);
}

Commit::Commit(const Commit& c, fs::path parent_path, string message)
{
    _message = message;
    _blobs = c._blobs;
    _timestamp = time(0);
    _parent = parent_path;
}

fs::path Commit::get_value(fs::path key)
{
    int num = _blobs.count(key);
    if (num == 0) return "";        // if key doesn't exist, return empty filepath
    return _blobs[key];             // else return blob path
}

void Commit::set_value(fs::path key, fs::path value)
{
    _blobs[key] = value;
}

void Commit::remove_key(fs::path key)
{
    _blobs.erase(key);
}

// PURPOSE: set uid
void Commit::set_uid(string uid)
{
    _uid = uid;
}

// PURPOSE: return if tracks file
bool Commit::tracks(fs::path file)
{
    int num = _blobs.count(file);
    return num != 0;
}

// PURPOSE: get path to parent commit
fs::path Commit::parent_path()
{
    return _parent;
}

ostream& operator<<(ostream& outs, const Commit c)
{
    outs << "===\ncommit " << c._uid << endl;
    outs << "Date: " << ctime(&c._timestamp);
    outs << c._message << endl << endl;
    // for (auto const &pair: c._blobs)
    //     std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    return outs;
}
#include "commit.h"

Commit::Commit()
{
    _message = "initial commit";
    time_t t = time(nullptr);
    _timestamp = t - t;
    string s = "";
    _uid = get_hash(s);
}

Commit::Commit(string message)
{
    _message = "initial commit";
    _timestamp = time(0); // TODO: verify time is correct
    // TODO: uid
}

fs::path Commit::get_value(fs::path key)
{
    int num = _blobs.count(key);
    if (num == 0) return "";        // if key doesn't exist, return empty filepath
    return _blobs[key];             // else return blob path
}

ostream& operator<<(ostream& outs, const Commit c)
{
    outs << "===\ncommit " << c._uid << endl;
    outs << "Date: " << ctime(&c._timestamp);
    outs << c._message << endl;
    return outs;
}
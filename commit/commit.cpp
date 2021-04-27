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

// PREREQ: only use relative paths, NOT absolute ones
bool Commit::matches(fs::path file)
{
    fs::path abs_path = CWD / file;
    // get hash of current file
    string h1 = get_hash(abs_path);
    // get hash of commit's file
    fs::path blob_path = get_value(file);
    string h2 = blob_path.parent_path().filename().string();
    h2 += blob_path.filename().string();
    // return comparison
    return h1 == h2;
}

// PURPOSE: get path to parent commit
fs::path Commit::parent_path()
{
    return _parent;
}

string Commit::get_message()
{
    return _message;
}

string Commit::get_uid()
{
    return _uid;
}

void Commit::checkout()
{
    for (const fs::path& f : fs::recursive_directory_iterator(CWD))
    {
        fs::path relative = f.lexically_relative(CWD);
        fs::path blob_path = get_value(relative);

        // if path contains .gitlet or gitlet.exe, continue
        if (relative.string().find(".gitlet") != string::npos  
            || relative.string().find("gitlet.exe") != string::npos) continue;
        
        // if current file does not exist in commit, remove
        if (blob_path == "") fs::remove(f);
        // else, overwrite  
        else
        {
            string contents;
            retrieve(blob_path, contents);
            ofstream outs(f);
            outs << contents;
        }
    }

    // iterate through blobs to add files not in CWD
    for (auto const& [file, blobpath] : _blobs)
    {
        if (!fs::exists(CWD / file))   
        {
            string contents;
            retrieve(blobpath, contents);
            ofstream outs(CWD / file);
            outs << contents;
        }
    }
}

ostream& operator<<(ostream& outs, const Commit c)
{
    outs << "===\ncommit " << c._uid << endl;
    outs << "Date: " << ctime(&c._timestamp);
    outs << c._message << endl << endl;
    // for (auto const &pair: c._blobs)
    //     std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    return outs;

    // TODO: Merge commits?
    // ===
    // commit 3e8bf1d794ca2e9ef8a4007275acf3751c7170ff
    // Merge: 4975af1 2c1ead1
    // Date: Sat Nov 11 12:30:00 2017 -0800
    // Merged development into master.
    // the two hexadecimal numerals following “Merge:” consist of the first seven digits of the first and second parents’ commit ids, in that order.
}
#include "commit.h"

Commit::Commit()
{
    _message = "initial commit";
    time_t t = time(0);
    _timestamp = t - t;
    string s = "";
    _uid = get_hash(s);
    _branch = MASTER_BRANCH;
}

Commit::Commit(const Commit& c, fs::path parent_path, string message)
{
    _message = message;
    _blobs = c._blobs;
    _timestamp = time(0);
    _parent = parent_path;
    _branch = get_current_branch();
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

// PURPOSE: return true if modified files exist in CWD, returns modified files by reference
bool Commit::check_for_modified(string& modified_files)
{
    // TODO: alpha order and deleted/modified info
    // junk.txt (deleted)
    // wug3.txt (modified)

    bool has_unequal = false;
    modified_files = "";
    for (auto const& [file, blobpath] : _blobs)
    {
        if (!matches(file))
        {
            has_unequal = true;
            modified_files += file.filename().string();
            modified_files += "\n";
        }
    }
    return has_unequal;
}

// PURPOSE: return true if unstaged modified files exist in CWD, returns modified files by reference
bool Commit::check_for_unstaged_modified(string& modified_files)
{
    // TODO: alpha order and deleted/modified info
    // junk.txt (deleted)
    // wug3.txt (modified)

    bool has_unequal = false;
    modified_files = "";
    for (auto const& [file, blobpath] : _blobs)
    {
        if (!matches(file) && !fs::exists(ADD_STAGE_DIR / file) && 
            !fs::exists(RM_STAGE_DIR / file))
        {
            has_unequal = true;
            modified_files += file.filename().string();
            modified_files += "\n";
        }
    }
    return has_unequal;
}

// PURPOSE: return true if untracked files exist in CWD that do not match commit
bool Commit::check_for_untracked(string& untracked_files)
{
    // TODO: alpha order 
    bool has_untracked = false;
    untracked_files = "";
    for (const fs::path& f : fs::recursive_directory_iterator(CWD))
    {
        fs::path relative = f.lexically_relative(CWD);
        // if path does not contain ".gitlet" or is "gitlet.exe"
        if (relative.string().find(".gitlet") == string::npos  
            && relative.string().find("gitlet.exe") == string::npos)
        {
            // if file name is not mapped to
            if (get_value(relative) == "" && 
                !fs::exists(ADD_STAGE_DIR / relative) && 
                !fs::exists(RM_STAGE_DIR / relative))
            {
                has_untracked = true;
                untracked_files += relative.string();
                untracked_files += "\n";
            }
        }
    }
    return has_untracked;
}

// PURPOSE: get path to parent commit
fs::path Commit::parent_path()
{
    return _parent;
}

// PURPOSE: get parent commit
Commit Commit::get_parent()
{
    Commit parent;
    retrieve(_parent, parent);
    return parent;
}

string Commit::get_message()
{
    return _message;
}

string Commit::get_uid()
{
    return _uid;
}

// PURPOSE: get branch
string Commit::get_branch()
{
    return _branch;
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
    for (auto const &pair: c._blobs)
        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    return outs;

    // TODO: Merge commits?
    // ===
    // commit 3e8bf1d794ca2e9ef8a4007275acf3751c7170ff
    // Merge: 4975af1 2c1ead1
    // Date: Sat Nov 11 12:30:00 2017 -0800
    // Merged development into master.
    // the two hexadecimal numerals following “Merge:” consist of the first seven digits of the first and second parents’ commit ids, in that order.
}

bool operator==(const Commit lhs, const Commit rhs)
{
    return lhs._uid == rhs._uid;
}

bool operator!=(const Commit lhs, const Commit rhs)
{
    return lhs._uid != rhs._uid;
}

// PURPOSE: return true if lhs was committed before rhs
bool operator<(const Commit lhs, const Commit rhs)
{
    return lhs._timestamp < rhs._timestamp;
}

// PURPOSE: return true if lhs was committed after rhs
bool operator>(const Commit lhs, const Commit rhs)
{
    return lhs._timestamp > rhs._timestamp;
}
#include "globals.h"

fs::path CWD = fs::current_path();
fs::path REPO_DIR = CWD / ".gitlet";
fs::path STAGE_DIR = REPO_DIR / "stages";
fs::path ADD_STAGE_DIR = STAGE_DIR / "add";
fs::path RM_STAGE_DIR = STAGE_DIR / "rm";
fs::path COMMITS_DIR = REPO_DIR / "commits";
fs::path BLOBS_DIR = REPO_DIR / "blobs";
fs::path BRANCHES_DIR = REPO_DIR / "branches";
fs::path MASTER_PATH = BRANCHES_DIR / "master";

fs::path HEAD_PATH = REPO_DIR / "HEAD";
fs::path CURRENT_BRANCH_PATH = REPO_DIR / "CURRENT_BRANCH";

string MASTER_BRANCH = "master";

string get_hash(fs::path file)
{
    // read in file
    ifstream file_stream(file);
    stringstream buffer;
    buffer << file_stream.rdbuf();
    // return hash
    return sha1(buffer.str());
}

string get_hash(string data)
{
    return sha1(data);
}

// PURPOSE: stores serialized file in a subfolder/file system within folder
// RETURNS: file path
fs::path store_file(fs::path filepath, fs::path folder)
{
    // read in file
    ifstream file_stream(filepath);
    stringstream buffer;
    buffer << file_stream.rdbuf();
    return store(buffer.str(), folder);
}

// PURPOSE: get current branch
string get_current_branch()
{
    string branch;
    retrieve(CURRENT_BRANCH_PATH, branch);
    return branch;
}
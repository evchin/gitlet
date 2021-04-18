#include "globals.h"

fs::path CWD = fs::current_path();
fs::path REPO_DIR = CWD / ".gitlet";
fs::path STAGE_DIR = REPO_DIR / "stages";
fs::path ADD_STAGE_DIR = STAGE_DIR / "add";
fs::path RM_STAGE_DIR = STAGE_DIR / "rm";
fs::path COMMITS_DIR = REPO_DIR / "commits";
fs::path MASTER_DIR = COMMITS_DIR / "master";
fs::path BLOBS_DIR = REPO_DIR / "blobs";
fs::path MASTER_PATH = REPO_DIR / "MASTER";
fs::path HEAD_PATH = REPO_DIR / "HEAD";

string get_hash(fs::path file)
{
    // read in file
    ifstream file_stream(file);
    stringstream buffer;
    buffer << file_stream.rdbuf();
    // return hash
    string out = sha1(buffer.str());
    return out;
}

string get_hash(string data)
{
    return sha1(data);
}
#ifndef GLOBALS_H
#define GLOBALS_H

#include <fstream>
#include "../overloads/overloads.h"
#include "../sha1/sha1.h"
#include "../cereal/archives/binary.hpp"
#include "../cereal/types/string.hpp"
using namespace std;
namespace fs = std::filesystem;

extern fs::path CWD;
extern fs::path REPO_DIR;
extern fs::path STAGE_DIR;
extern fs::path ADD_STAGE_DIR;
extern fs::path RM_STAGE_DIR;
extern fs::path COMMITS_DIR;
extern fs::path BLOBS_DIR;
extern fs::path BRANCHES_DIR;
extern fs::path HEAD_PATH;
extern fs::path CURRENT_BRANCH_PATH;
extern fs::path MASTER_PATH;
extern string MASTER_BRANCH;

string get_hash(fs::path file);
string get_hash(string data);
fs::path store_file(fs::path filepath, fs::path folder);

// PURPOSE: stores serialized object in a subfolder/file system within folder
// DO NOT STORE COMMITS USING THIS METHOD.
template <typename T>
fs::path store(T obj, fs::path folder)
{
    // serialize
    stringstream ss;
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive(obj);
    // get file's hash
    string hash = get_hash(ss.str());
    fs::path subfolder = folder / hash.substr(0, 2);
    fs::path file_path = subfolder / hash.substr(2, hash.length() - 2);
    // create subfolder and file, and write to file
    create_directory(subfolder);
    ofstream file(file_path);
    file << ss.str();
    return file_path;
}

// PURPOSE: stores object directly in file
template <typename T>
void store_in_file(T object, fs::path file)
{
    // read in file
    ofstream os(file, ios::binary);
    cereal::BinaryOutputArchive oarchive(os);
    oarchive(object);
}

template <typename T>
void retrieve(fs::path filepath, T& object)
{
    // read in file
    stringstream ss;
    ifstream file_stream(filepath, ios::binary);
    ss << file_stream.rdbuf();
    // deserialize and store in object
    cereal::BinaryInputArchive iarchive(ss);
    iarchive(object);
}

#endif // GLOBALS_H
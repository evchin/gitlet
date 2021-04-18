#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <filesystem>
#include <string.h>
using namespace std;
namespace fs = std::filesystem;

extern fs::path CWD;
extern fs::path REPO_DIR;
extern fs::path ADD_STAGE_DIR;
extern fs::path RM_STAGE_DIR;
extern fs::path COMMITS_DIR;
extern fs::path BLOBS_DIR;

#endif // CONSTANTS_H
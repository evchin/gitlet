#include "constants.h"

fs::path CWD = fs::current_path();
fs::path REPO_DIR = CWD / ".gitlet";
fs::path ADD_STAGE_DIR = REPO_DIR / "stages" / "add";
fs::path RM_STAGE_DIR = REPO_DIR / "stages" / "rm";
fs::path COMMITS_DIR = REPO_DIR / "commits";
fs::path BLOBS_DIR = REPO_DIR / "blobs";
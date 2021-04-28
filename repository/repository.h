#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <sstream>
#include "../commit/commit.h"

class Repository
{
public:
    Repository();
    void init();
    void add(fs::path file);
    void commit(string message);
    void rm(fs::path file);
    void log();
    void global_log();
    void find(string message);
    void status();
    void checkout(fs::path file);
    void checkout(string commit_id, fs::path file);
    void checkout(string branch);
    void branch(string name);
    void rm_branch(string name);
    void reset(string commit_id);
    void merge(string branch_name);
private:
// TODO: when did u ever use this....
    fs::path _master;    // head of master branch
    fs::path _head;      // head of current branch

    // PURPOSE: get split point commit
    Commit get_split_point(string branch_name);
    // PURPOSE: return if file is equal to file in current commit
    bool in_commit(fs::path file);
    // PURPOSE: get current commit and commit path by reference
    Commit get_current_commit(fs::path& commit_path);
    // PURPOSE: retrieve commit with id, return true if commit exists
    bool retrieve_commit(string uid, Commit& c);
    // PURPOSE: serialize and store commit
    fs::path store_commit(Commit c, fs::path folder);
};

#endif // REPOSITORY_H
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
    fs::path _master;    // head of master branch
    fs::path _head;      // head of current branch
    // PURPOSE: return if file is equal to file in current commit
    bool in_commit(fs::path file);
};

#endif // REPOSITORY_H
#include "repository/repository.h"

// TODO: FIX COMMANDS??

int main(int argc, char *argv[])
{   
    Repository repo;

    if (argc == 1)
    {
        cout << "Please enter an argument." << endl;
    }
    else if (strcmp(argv[1], "init") == 0)
    {
        repo.init();
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3) cout << "Please enter a file to add to the staging area.\n";
        else for (int i = 2; i < argc; i++) repo.add(argv[i]);
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        if (argc != 3) cout << "Please enter a commit message.\n";
        else repo.commit(argv[2]);
    }
    else if (strcmp(argv[1], "rm") == 0)
    {
        if (argc < 3) cout << "Please enter a file to remove.\n";
        else for (int i = 2; i < argc; i++) repo.rm(argv[i]);
    }
    else if (strcmp(argv[1], "log") == 0)
    {
        repo.log();
    }
    else if (strcmp(argv[1], "global-log") == 0)
    {
        repo.global_log();
    }
    else if (strcmp(argv[1], "find") == 0)
    {
        if (argc != 3) cout << "Please enter a commit message to find.\n";
        else repo.find(argv[2]);
    }
    else if (strcmp(argv[1], "branch") == 0)
    {
        if (argc != 3) cout << "Please enter a branch name.\n";
        else repo.branch(argv[2]);
    }
    else if (strcmp(argv[1], "rm-branch") == 0)
    {
        if (argc != 3) cout << "Please enter a branch to remove.\n";
        else repo.rm_branch(argv[2]);
    }
    else if (strcmp(argv[1], "checkout") == 0)
    {
        if (argc == 3) 
        {
            string branch = argv[2];
            repo.checkout(branch);
        }
        else if (argc == 4)
        {
            fs::path path = argv[3];
            repo.checkout(path);
        }
        else if (argc == 5) 
        {
            repo.checkout(argv[2], argv[4]);
        }
        else
        {
            cout << "Invalid argument." << endl;
        }
    }
    else if (strcmp(argv[1], "reset") == 0)
    {
        if (argc != 3) cout << "Please enter a commit id to reset to.\n";
        else repo.reset(argv[2]);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        repo.status();
    }
    else if (strcmp(argv[1], "merge") == 0)
    {
        if (argc != 3) cout << "Please enter a branch to merge.\n";
        else repo.merge(argv[2]);
    }
    else
    {
        cout << "Invalid argument." << endl;
    }
    return 0;
}
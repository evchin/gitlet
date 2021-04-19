#include "repository/repository.h"

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

    else if (strcmp(argv[1], "status") == 0)
    {
        cout << "Running status." << endl;
    }
    
    else if (strcmp(argv[1], "find") == 0)
    {
        cout << "Running status." << endl;
    }
    else if (strcmp(argv[1], "checkout") == 0)
    {
        cout << "Running status." << endl;
    }
    else if (strcmp(argv[1], "branch") == 0)
    {
        cout << "Running status." << endl;
    }
    else if (strcmp(argv[1], "rm_branch") == 0)
    {
        cout << "Running status." << endl;
    }
    else if (strcmp(argv[1], "reset") == 0)
    {
        cout << "Running status." << endl;
    }
    else if (strcmp(argv[1], "merge") == 0)
    {
        cout << "Running status." << endl;
    }
    else
    {
        cout << "Invalid argument." << endl;
    }
    std::exit(0);
    
    // rm(string file){}
    // find(string message){}
    // checkout(string file){}
    // checkout(string branch){}
    // checkout(string commit_id, string file){}
    // branch(string name){}
    // rm_branch(string name){}
    // reset(string commit_id){}
    // merge(string branch_name){}

    return 0;
}
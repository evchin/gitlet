#include "repository/repository.h"

int main(int argc, char *argv[])
{
    Repository repo;
    repo.add("hello.txt");
    
    // switch(argc)
    // {
    //     case 1:
    //         cout << "Please enter an argument.";
    //         exit(0);
    //     case 2:
    //     {
    //         if (strcmp(argv[1], "init") == 0)
    //         {
    //             repo.init();
    //         }
    //         else if (strcmp(argv[1], "log") == 0)
    //         {
    //             cout << "Running log." << endl;
    //         }
    //         else if (strcmp(argv[1], "global-log") == 0)
    //         {
    //             cout << "Running global log." << endl;
    //         }
    //         else if (strcmp(argv[1], "status") == 0)
    //         {
    //             cout << "Running status." << endl;
    //         }
    //         else
    //         {
    //             cout << "Invalid argument." << endl;
    //         }
    //         exit(0);
    //     }
    //     case 3:
    //         if (strcmp(argv[1], "add") == 0)
    //         {

    //         }
    //         else if (strcmp(argv[1], "log") == 0)
    //         {
    //             cout << "Running log." << endl;
    //         }
    //         else if (strcmp(argv[1], "global-log") == 0)
    //         {
    //             cout << "Running global log." << endl;
    //         }
    //         else if (strcmp(argv[1], "status") == 0)
    //         {
    //             cout << "Running status." << endl;
    //         }
    //         else
    //         {
    //             cout << "Invalid argument." << endl;
    //         }
    //         exit(0);
            
    //         // commit(string message){} - if no message, "Please enter a commit message."
    //         // rm(string file){}
    //         // find(string message){}
    //         // checkout(string file){}
    //         // checkout(string branch){}
    //         // branch(string name){}
    //         // rm_branch(string name){}
    //         // reset(string commit_id){}
    //         // merge(string branch_name){}
    //     case 4:
    //         // checkout(string commit_id, string file){}
    //     default:
    //         cout << "Invalid argument." << endl;
    //         exit(0);
    // }

    return 0;
}
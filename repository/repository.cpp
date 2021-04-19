#include "repository.h"

Repository::Repository(){}

void Repository::init()
{
    // if .gitlet folder already exists
    if (fs::exists(REPO_DIR))
    {
        cout << "A Gitlet version-control system already exists in the current directory.\n";
    }
    else
    {
        create_directory(REPO_DIR);
        create_directory(STAGE_DIR);
        create_directory(ADD_STAGE_DIR);
        create_directory(RM_STAGE_DIR);
        create_directory(COMMITS_DIR);
        create_directory(COMMITS_DIR / MASTER_BRANCH);
        create_directory(BLOBS_DIR);

        Commit commit;
        fs::path file_path = store(commit, COMMITS_DIR / MASTER_BRANCH);

        // assign heads
        _master = file_path;
        _head = file_path;
        // store master, head, and current branch
        store_in_file(_master, MASTER_PATH);
        store_in_file(_head, HEAD_PATH);
        store_in_file(MASTER_BRANCH, CURRENT_BRANCH_PATH);
    }
}

void Repository::add(fs::path file)
{
    fs::path abs_path = CWD / file;

    // if file doesn't exist
    if (!fs::exists(abs_path))
    {
        cout << "File does not exist.\n";
    }
    else
    {
        // check if file is equal to file in current commit
        if (in_commit(file))
        {
            // if file is in add or rm stage, remove it
            if (fs::exists(ADD_STAGE_DIR / file)) fs::remove(ADD_STAGE_DIR / file);
            if (fs::exists(RM_STAGE_DIR / file)) fs::remove(RM_STAGE_DIR / file);
        }
        else
        {
            // store contents of file in blob
            fs::path blob_path = store(abs_path, BLOBS_DIR);
            store_in_file(blob_path, ADD_STAGE_DIR / file);
        }
    }
}

void Repository::commit(string message)
{
    // if ADD_STAGE_DIR and RM_STAGE_DIR are empty
    if (fs::is_empty(ADD_STAGE_DIR) && fs::is_empty(RM_STAGE_DIR))
    {
        cout << "No changes added to the commit.\n";
    }
    else
    {
        // clone current commit
        fs::path parent_path;
        Commit parent = get_current_commit(parent_path);
        Commit current(parent, parent_path, message);

        // modify map of blobs
        // if in add, overwrite the blob in map and remove file from stage
        for (const fs::path& file : fs::recursive_directory_iterator(ADD_STAGE_DIR))
        {
            fs::path blob_path;
            retrieve(file, blob_path);
            current.set_value(file.filename(), blob_path);
            fs::remove(file);
        }
        // if in rm, delete blob from map and remove file from stage
        for (const fs::path& file : fs::recursive_directory_iterator(RM_STAGE_DIR))
        {
            current.remove_key(file.filename());
            fs::remove(file);
        }

        // update uid of current commit
        stringstream ss;
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(current);
        string uid = get_hash(ss.str());
        current.set_uid(uid);

        // store serialized commit into commits folder under current branch
        string branch_name;
        retrieve(CURRENT_BRANCH_PATH, branch_name);
        fs::path commit_path = store(current, COMMITS_DIR / branch_name);

        // advance heads (head and master)
        if (branch_name == MASTER_BRANCH) store_in_file(commit_path, MASTER_PATH);
        store_in_file(commit_path, HEAD_PATH);
    }
}

void Repository::rm(fs::path file)
{
    // get current commit
    fs::path commit_path;
    Commit current = get_current_commit(commit_path);
    cout << "CURRENT COMMIT: " << current;

    // if file is in add stage
    if (fs::exists(ADD_STAGE_DIR / file)) 
    {
        // unstage it
        fs::remove(ADD_STAGE_DIR / file);
    }
    // if tracked in current commit
    else if (current.tracks(file))
    {
        // stage file for removal (contents don't matter)
        string buffer;
        store_in_file(buffer, RM_STAGE_DIR / file);
        // remove file from cwd
        fs::remove(CWD / file);
    }
    // if the file is neither staged nor tracked by the head commit
    else
    {
        cout << "No reason to remove the file.\n";
    }
}

void Repository::log()
{
    // Starting at the current head commit, 
        // display information about each commit backwards along the commit tree until the initial commit, 
        // following the first parent commit links, ignoring any second parents found in merge commits.
    // ===
    // commit a0da1ea5a15ab613bf9961fd86f010cf74c7ee48
    // Date: Thu Nov 9 20:00:05 2017 -0800
    // A commit message.

    // ===
    // commit 3e8bf1d794ca2e9ef8a4007275acf3751c7170ff
    // Date: Thu Nov 9 17:01:33 2017 -0800
    // Another commit message.

    // Merge commits?
    // ===
    // commit 3e8bf1d794ca2e9ef8a4007275acf3751c7170ff
    // Merge: 4975af1 2c1ead1
    // Date: Sat Nov 11 12:30:00 2017 -0800
    // Merged development into master.
    
    // the two hexadecimal numerals following “Merge:” consist of the first seven digits of the first and second parents’ commit ids, in that order. 
    // The first parent is the branch you were on when you did the merge; the second is that of the merged-in branch. 
}

void Repository::global_log()
{
    // Like log, except displays information about all commits ever made. 
    // The order of the commits does not matter. 
}

void Repository::find(string message)
{
    // If no such commit exists, prints the error message Found no commit with that message.
    // Prints out the ids of all commits that have the given commit message, one per line. 
    // The commit message is a single operand; to indicate a multiword message, put the operand in quotation marks, as for the commit command below. 
    // Hint: the hint for this command is the same as the one for global-log.
}
void Repository::status()
{
    // === Branches ===
    // *master
    // other-branch
    
    // === Staged Files ===
    // wug.txt
    // wug2.txt
    
    // === Removed Files ===
    // goodbye.txt
    
    // LATER ON
    // === Modifications Not Staged For Commit ===
    // junk.txt (deleted)
    // wug3.txt (modified)
    // SEE SPEC
    
    // === Untracked Files ===
    // random.stuff

    // Entries should be listed in lexicographic order, using the Java string-comparison order (the asterisk doesn’t count)
}

void Repository::checkout(fs::path file)
{
    // Takes the version of the file as it exists in the head commit and puts it in the working directory, 
    // overwriting the version of the file that’s already there if there is one. 
    // The new version of the file is not staged.
    // If the file does not exist in the previous commit, abort, printing the error message File does not exist in that commit. Do not change the CWD.
}

void Repository::checkout(string commit_id, fs::path file)
{
    // Takes the version of the file as it exists in the commit with the given id, 
    // and puts it in the working directory, overwriting the version of the file that’s already there if there is one. 
    // The new version of the file is not staged.
    // If no commit with the given id exists, print "No commit with that id exists." 
    // Otherwise, if the file does not exist in the given commit, print File does not exist in that commit. Do not change the CWD.
}

void Repository::checkout(string branch)
{
    // If no branch with that name exists, print "No such branch exists." 
    // If that branch is the current branch, print "No need to checkout the current branch." 
    // If a working file is untracked in the current branch 
        // and would be overwritten by the checkout, print "There is an untracked file in the way; delete it, or add and commit it first." and exit; 
        // perform this check before doing anything else. Do not change the CWD.
    
    // Takes all files in the commit at the head of the given branch, 
    // and puts them in the working directory, overwriting the versions of the files that are already there if they exist. 
    // Also, at the end of this command, the given branch will now be considered the current branch (HEAD). 
    // Any files that are tracked in the current branch but are not present in the checked-out branch are deleted. 
    // The staging area is cleared, unless the checked-out branch is the current branch (see Failure cases below).
}

void Repository::branch(string name)
{
    // If a branch with the given name already exists, print the error message A branch with that name already exists.
    // Creates a new branch with the given name, and points it at the current head commit. 
    // A branch is nothing more than a name for a reference (a SHA-1 identifier) to a commit node. 
    // This command does NOT immediately switch to the newly created branch (just as in real Git). 
    // Before you ever call branch, your code should be running with a default branch called “master”.
}

void Repository::rm_branch(string name)
{
    //  If a branch with the given name does not exist, aborts. Print the error message A branch with that name does not exist. 
    // If you try to remove the branch you’re currently on, aborts, printing the error message Cannot remove the current branch.
    
    // Deletes the branch with the given name. 
    // This only means to delete the pointer associated with the branch; 
    // it does not mean to delete all commits that were created under the branch, or anything like that.
}

void Repository::reset(string commit_id)
{
    // If no commit with the given id exists, print No commit with that id exists. 
    // If a working file is untracked in the current branch and would be overwritten by the reset, 
        // print `There is an untracked file in the way; delete it, or add and commit it first.`
        // and exit; perform this check before doing anything else.
    // Checks out all the files tracked by the given commit. 
    // Removes tracked files that are not present in that commit. 
    // Also moves the current branch’s head to that commit node. 
    // See the intro for an example of what happens to the head pointer after using reset. 
    // The [commit id] may be abbreviated as for checkout. 
    // The staging area is cleared. 
    // The command is essentially checkout of an arbitrary commit that also changes the current branch head.
}

void Repository::merge(string branch_name)
{
    // see spec
}

// PURPOSE: return if file is equal to file in current commit
bool Repository::in_commit(fs::path file)
{
    fs::path abs_path = CWD / file;
    // get hash of current file
    string h1 = get_hash(abs_path);
    // get hash of commit's file
    Commit commit;
    fs::path commit_file;
    retrieve(HEAD_PATH, commit_file);
    retrieve(commit_file, commit);
    fs::path blob_path = commit.get_value(file);
    string h2 = blob_path.parent_path().filename().string();
    h2 += blob_path.filename().string();
    // return comparison
    return h1 == h2;
}

// PURPOSE: get current commit and commit path by reference
Commit Repository::get_current_commit(fs::path& commit_path)
{
    Commit current;
    retrieve(HEAD_PATH, commit_path);
    retrieve(commit_path, current);
    return current;
}
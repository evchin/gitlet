#include "repository.h"

// TODO: remember to close all streams...
// TODO: Test other than txt files
// TODO: Test with folders too
// TODO: made modifications - checkout would be allowed to happen under gitlet if files in staging area. didn't quite understand why, as git does not permit this. so i ignored this qualification.
// TODO: REmove redundant CURRENT_BRANCH and HEAD FILE

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
        create_directory(BRANCHES_DIR);

        // create initial commit
        Commit commit;
        fs::path file_path = store_commit(commit, COMMITS_DIR / MASTER_BRANCH);

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

    // if file is staged for removal 
    if (fs::exists(RM_STAGE_DIR / file))
    {
        // add file back to CWD
        fs::path blob_path;
        retrieve(RM_STAGE_DIR / file, blob_path);
        string contents;
        retrieve(blob_path, contents);
        ofstream out(abs_path);
        out << contents;
        out.close(); 
        // unstage file
        fs::remove(RM_STAGE_DIR / file);
    }
    // if file doesn't exist
    else if (!fs::exists(abs_path))
    {
        cout << "File does not exist.\n";
    }
    else
    {
        // check if file is equal to file in current commit
        if (in_commit(file))
        {
            if (fs::exists(ADD_STAGE_DIR / file)) 
                fs::remove(ADD_STAGE_DIR / file);
        }
        else
        {
            // store contents of file in blob
            fs::path blob_path = store_file(abs_path, BLOBS_DIR);
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

        // store serialized commit into commits folder under current branch
        string branch_name;
        retrieve(CURRENT_BRANCH_PATH, branch_name);
        fs::path commit_path = store_commit(current, COMMITS_DIR / branch_name);

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

    // if file is in add stage
    if (fs::exists(ADD_STAGE_DIR / file)) 
    {
        // unstage it
        fs::remove(ADD_STAGE_DIR / file);
    }
    // if tracked in current commit
    else if (current.tracks(file))
    {
        // store contents of file in blob
        fs::path blob_path = store_file(CWD / file, BLOBS_DIR);
        // stage for removal
        store_in_file(blob_path, RM_STAGE_DIR / file);
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
    // get current commit
    fs::path commit_path;
    Commit head = get_current_commit(commit_path);
    cout << head;

    // display each commit until the initial commit 
    while (fs::exists(head.parent_path()))
    {
        retrieve(head.parent_path(), head);
        cout << head;
    } 
}

void Repository::global_log()
{
    for (const fs::path& f : fs::recursive_directory_iterator(COMMITS_DIR))
    {
        if (!fs::is_directory(f))
        {
            Commit c;
            retrieve(f, c);
            cout << c;
        }
    }
}

void Repository::find(string message)
{
    int num = 0;
    for (const fs::path& f : fs::recursive_directory_iterator(COMMITS_DIR))
    {
        if (!fs::is_directory(f))
        {
            Commit c;
            retrieve(f, c);
            if (c.get_message() == message) 
            {
                cout << c.get_uid() << endl;
                num++;
            }
        }
    }
    if (num == 0)
    {
        cout << "Found no commit with that message." << endl;
    }
}

void Repository::branch(string name)
{
    if (fs::exists(BRANCHES_DIR / name))
    {
        cout << "A branch with that name already exists.\n";
    }
    else
    {
        // create new branch file with pointer to head commit
        fs::path head;
        retrieve(HEAD_PATH, head);
        store_in_file(head, BRANCHES_DIR / name);

        // create new branch folder under commits
        create_directory(COMMITS_DIR / name);
    }
}

void Repository::rm_branch(string name)
{
    if (!fs::exists(BRANCHES_DIR / name))
    {
        cout << "A branch with that name does not exist.\n";
    }
    else if (get_current_branch() == name)
    {
        cout << "Cannot remove the current branch.\n";
    }
    else
    {
        fs::remove(BRANCHES_DIR / name);
    }
}

void Repository::checkout(fs::path file)
{
    fs::path abs_path = CWD / file;
    fs::path commit_path;
    Commit current = get_current_commit(commit_path);

    if (!current.tracks(file))
    {
        cout << "File does not exist in that commit.\n";
    }
    else
    {
        fs::path blob_path = current.get_value(file);
        string contents;
        retrieve(blob_path, contents);
        ofstream out(abs_path);
        out << contents;
        out.close(); 
    }    
}

void Repository::checkout(string commit_id, fs::path file)
{
    ofstream outs(CWD / file);
    Commit commit;

    // if commit does not exist
    if (!retrieve_commit(commit_id, commit)) cout << "No commit with that id exists." << endl;
    else
    {
        fs::path blob_path = commit.get_value(file);

        // if file does not exist in commit
        if (blob_path == "") cout << "File does not exist in that commit." << endl;
        else
        {
            // overwrite file in CWD
            string contents;
            retrieve(blob_path, contents);
            outs << contents;
        }
    }

    outs.close();
}

void Repository::checkout(string branch)
{
    // failure checks
    if (!fs::exists(BRANCHES_DIR / branch)) 
        cout << "No such branch exists." << endl;
    else if (get_current_branch() == branch) 
        cout << "No need to checkout the current branch." << endl;
    else if (!fs::is_empty(ADD_STAGE_DIR) || !fs::is_empty(RM_STAGE_DIR)) 
        cout << "There is a staged file in the way; unstage and delete it, or commit it first." << endl;
    else
    {
        // check for untracked files not in current commit
        fs::path current_path;
        Commit current = get_current_commit(current_path);

        string untracked_files = "";
        bool has_untracked = current.check_for_untracked(untracked_files);
        if (has_untracked)
        {
            cout << "There is an untracked file in the way; delete it, or add and commit it first.\n\n";
            cout << "Untracked files:\n" << untracked_files << endl;
            return;
        }
        string modified_files = "";
        bool has_unequal = current.check_for_modified(modified_files);
        if (has_unequal)
        {
            cout << "There is a modified file in the way; revert it to its original state, or add and commit it first.\n\n";
            cout << "Modified files:\n" << modified_files << endl;
            return;
        }

        // get given commit
        fs::path head_commit_path;
        Commit commit;
        retrieve(BRANCHES_DIR / branch, head_commit_path);
        retrieve(head_commit_path, commit); 

        // call commit's checkout
        commit.checkout();

        // change current branch to given branch 
        store_in_file(branch, CURRENT_BRANCH_PATH);
        // change head to be latest commit of given branch
        store_in_file(head_commit_path, HEAD_PATH);
    }
}

void Repository::reset(string commit_id)
{
    // failure checks
    Commit c;
    fs::path current_commit_path;
    Commit current = get_current_commit(current_commit_path);
    if (!retrieve_commit(commit_id, c)) 
    {
        cout << "No commit with that id exists." << endl;
        return;
    }
    string untracked_files = "";
    bool has_untracked = current.check_for_untracked(untracked_files);
    if (has_untracked)
    {
        cout << "There is an untracked file in the way; delete it, or add and commit it first.\n\n";
        cout << "Untracked files:\n" << untracked_files << endl;
        return;
    }
    string modified_files = "";
    bool has_unequal = current.check_for_modified(modified_files);
    if (has_unequal)
    {
        cout << "There is a modified file in the way; revert it to its original state, or add and commit it first.\n\n";
        cout << "Modified files:\n" << modified_files << endl;
        return;
    }
    if (!fs::is_empty(ADD_STAGE_DIR) || !fs::is_empty(RM_STAGE_DIR)) 
    {
        cout << "There is a staged file in the way; unstage and delete it, or commit it first." << endl;
        return;
    }

    // checkout all files in given commit and remove tracked files that are not present in that commit.
    c.checkout();

    // get branch of commit
    string branch = c.get_branch();
    // change current branch to branch 
    store_in_file(branch, CURRENT_BRANCH_PATH);
    // change head to be latest commit of given branch
    fs::path subfolder = COMMITS_DIR / branch / commit_id.substr(0, 2);
    fs::path head_commit_path = subfolder / commit_id.substr(2, commit_id.length() - 2);
    store_in_file(head_commit_path, HEAD_PATH);
}

void Repository::status()
{
    cout << "=== Branches ===" << endl;
    string current_branch = get_current_branch();
    for(auto& p: fs::directory_iterator(BRANCHES_DIR))
    {
        if (p.path().filename().string() == current_branch) cout << "*";
        cout << p.path().filename().string() << '\n';
    }
    cout << endl;

    cout << "=== Staged Files ===" << endl;
    for(auto& p: fs::directory_iterator(ADD_STAGE_DIR))
        cout << p.path().filename().string() << '\n';
    cout << endl;

    cout << "=== Removed Files ===" << endl;
    for(auto& p: fs::directory_iterator(RM_STAGE_DIR))
        cout << p.path().filename().string() << '\n';
    cout << endl;
    
    cout << "=== Modifications Not Staged For Commit ===" << endl;
    fs::path current_commit_path;
    string modified_files;
    Commit current = get_current_commit(current_commit_path);
    current.check_for_unstaged_modified(modified_files);
    cout << modified_files << endl;

    cout << "=== Untracked Files ===" << endl;
    string untracked_files;
    current.check_for_untracked(untracked_files);
    cout << untracked_files << endl;
}

// Merges files from the given branch into the current branch
void Repository::merge(string branch_name)
{
    // find split point commit
    Commit split = get_split_point(branch_name);
    cout << "SPLIT POINT:" << endl;
    cout << split << endl;

    // If the split point is the same commit as the given branch, then we do nothing; 
        // the merge is complete, and the operation ends with the message Given branch is an ancestor of the current branch. 
    // If the split point is the current branch, then the effect is to 
        // check out the given branch, 
        // and the operation ends after printing the message Current branch fast-forwarded.

    // else

    // Any files that have been modified in the given branch since the split point, 
    // but not modified in the current branch since the split point should be  
        // To clarify, if a file is “modified in the given branch since the split point” 
        // this means the version of the file as it exists in the commit at the front of 
        // the given branch has different content from the version of the file at the split point. Remember: blobs are content addressable!
        
        // changed to their versions in the given branch (checked out from the commit at the front of the given branch). 
        // These files should then all be automatically staged. 
    
}

Commit Repository::get_split_point(string branch_name)
{
    // get current commit
    fs::path current_path;
    Commit current = get_current_commit(current_path);
    // get head commit of given branch
    fs::path branch_path;
    Commit branch;
    retrieve(BRANCHES_DIR / branch_name, branch_path);
    retrieve(branch_path, branch);

    // while current and branch are not the same commit
    while (current != branch)
    {
        // set latest commit to parent 
        if (current < branch) branch = branch.get_parent();
        else current = current.get_parent();
    }

    return current;
}

// PURPOSE: return if file is equal to file in current commit
bool Repository::in_commit(fs::path file)
{
    fs::path abs_path = CWD / file;
    fs::path commit_path;
    Commit commit = get_current_commit(commit_path);
    return commit.matches(file);
}

// PURPOSE: get current commit and commit path by reference
Commit Repository::get_current_commit(fs::path& commit_path)
{
    Commit current;
    retrieve(HEAD_PATH, commit_path);
    retrieve(commit_path, current);
    return current;
}

// PURPOSE: retrieve commit with id, return true if commit exists
// PREREQ: commit must exist
bool Repository::retrieve_commit(string uid, Commit& c)
{
    string branch = get_current_branch();
    fs::path subfolder = COMMITS_DIR / branch / uid.substr(0, 2);
    fs::path file_path = subfolder / uid.substr(2, uid.length() - 2);
    if (!fs::exists(file_path)) return false;
    retrieve(file_path, c);
    return true;
}

// PURPOSE: serialize and store commit
// PREREQ: uid is updated
fs::path Repository::store_commit(Commit c, fs::path folder)
{
    // update commit uid
    if (c.get_uid() == "")
    {
        stringstream ss;
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(c);
        c.set_uid(get_hash(ss.str()));
    }

    // serialize with uid
    stringstream ss;
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive(c);

    // get directories and files
    fs::path subfolder = folder / c.get_uid().substr(0, 2);
    fs::path file_path = subfolder / c.get_uid().substr(2, c.get_uid().length() - 2);
    
    // create subfolder and file, and write to file
    create_directory(subfolder);
    ofstream file(file_path);
    file << ss.str();
    
    return file_path;
}
#include "repository.h"

// TODO: remember to close all streams...
// TODO: Test other than txt files
// TODO: Test with folders too

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
        bool has_untracked = false;
        string untracked_files = "";
        for (const fs::path& f : fs::recursive_directory_iterator(CWD))
        {
            fs::path relative = f.lexically_relative(CWD);
            // if path does not contain ".gitlet" or is "gitlet.exe"
            if (relative.string().find(".gitlet") == string::npos  
                && relative.string().find("gitlet.exe") == string::npos)
            {
                if (!current.matches(relative))
                {
                    has_untracked = true;
                    untracked_files += relative.string();
                    untracked_files += "\n";
                }
            }
        }

        if (has_untracked)
        {
            cout << "There is an untracked file in the way; delete it, or add and commit it first.\n\n";
            cout << "Untracked files:\n" << untracked_files << endl;
            exit(0);
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
    // get commit
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

// PURPOSE: get current branch
string Repository::get_current_branch()
{
    string branch;
    retrieve(CURRENT_BRANCH_PATH, branch);
    return branch;
}

// PURPOSE: retrieve commit with id, return true if commit exists
// PREREQ: commit must exist
bool Repository::retrieve_commit(string uid, Commit& c)
{
    cout << "Retrieving commit " << uid << endl;
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
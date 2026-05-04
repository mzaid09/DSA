/*
 * File   : UserManager.cpp
 * Module : A — User Directory (Hash Table with Chaining)
 * Member : Member 1
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] registerUser: now rejects usernames that contain spaces.
 * [UPDATED] registerUser: now rejects empty username or password input.
 * [UPDATED] deleteUser  : now asks "Are you sure? (y/n)" before deleting.
 * [UPDATED] displayAllUsers: prints clear message when no users exist.
 * [UPDATED] searchUser  : now also prints friend count and post count.
 * [UPDATED] userManagementMenu: invalid menu choice now clears bad input.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "UserManager.h"
#include "Analytics.h"    // insertUserAVL()
#include "Integration.h"  // cleanupDeletedUser()

/* ── Hash table: 100 buckets, all start empty ── */
User* hashTable[TABLE_SIZE] = { nullptr };

/* ── Shared post ID counter (also used in PostManager.cpp) ── */
int postIDCounter = 1;

/* ══════════════════════════════════════════════════════════════
 * Function : hashFunction
 * Purpose  : Converts a username string into a bucket index 0–99.
 * Input    : userName — the string to hash
 * Output   : integer in range [0, TABLE_SIZE-1]
 ══════════════════════════════════════════════════════════════ */
int hashFunction(string userName) {
    int sum = 0;
    for (int i = 0; i < (int)userName.length(); i++) {
        sum += (int)userName[i];
    }
    return sum % TABLE_SIZE;
}

/* ══════════════════════════════════════════════════════════════
 * Function : hashSearch
 * Purpose  : Searches the hash table for a user by username.
 *            Called by every other module before any operation.
 * Input    : userName — the name to search for
 * Output   : pointer to User node if found, nullptr if not found
 ══════════════════════════════════════════════════════════════ */
User* hashSearch(string userName) {
    int   index   = hashFunction(userName);
    User* current = hashTable[index];

    while (current != nullptr) {
        if (current->userName == userName)
            return current;
        current = current->next;
    }
    return nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : registerUser
 * Purpose  : Prompts for account details, validates input, creates
 *            a User node, inserts into hash table, inserts into AVL.
 * Input    : user-typed username, password, email, bio
 * Output   : new user added to system or error message
 *
 * [UPDATED] Rejects username with spaces — cin breaks on spaces and
 *           the hash function would behave incorrectly.
 * [UPDATED] Rejects empty username or empty password.
 ══════════════════════════════════════════════════════════════ */
void registerUser() {
    string uname, pass, email, bio;

    cout << "\n--- Register New User ---" << endl;
    cout << "Enter username  : "; cin >> uname;

    // [UPDATED] reject empty username
    if (uname.empty()) {
        cout << "Error: Username cannot be empty." << endl;
        return;
    }

    // [UPDATED] reject username that contains spaces
    for (int i = 0; i < (int)uname.length(); i++) {
        if (uname[i] == ' ') {
            cout << "Error: Username cannot contain spaces." << endl;
            return;
        }
    }

    // reject duplicate username
    if (hashSearch(uname) != nullptr) {
        cout << "Error: Username '" << uname << "' already exists." << endl;
        return;
    }

    cout << "Enter password  : "; cin >> pass;

    // [UPDATED] reject empty password
    if (pass.empty()) {
        cout << "Error: Password cannot be empty." << endl;
        return;
    }

    cout << "Enter email     : "; cin >> email;
    cin.ignore();
    cout << "Enter bio       : "; getline(cin, bio);

    // create new user node
    User* newUser       = new User();
    newUser->userName   = uname;
    newUser->password   = pass;
    newUser->email      = email;
    newUser->bio        = bio;
    newUser->lastActive = "2024-12-01";

    // insert at head of correct bucket chain
    int index        = hashFunction(uname);
    newUser->next    = hashTable[index];
    hashTable[index] = newUser;

    // MODULE F: insert into AVL user tree with activity = 0
    insertUserAVL(uname, 0);

    cout << "\n✓ User '" << uname << "' registered successfully!" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : loginUser
 * Purpose  : Searches hash table for username, compares password,
 *            sets global currentUser on success.
 * Input    : username and password from user
 * Output   : sets currentUser or prints error
 ══════════════════════════════════════════════════════════════ */
void loginUser() {
    if (currentUser != nullptr) {
        cout << "Error: Already logged in as '" << currentUser->userName << "'." << endl;
        cout << "Please logout first." << endl;
        return;
    }

    string uname, pass;
    cout << "\n--- Login ---" << endl;
    cout << "Enter username: "; cin >> uname;
    cout << "Enter password: "; cin >> pass;

    // [UPDATED] reject empty inputs
    if (uname.empty() || pass.empty()) {
        cout << "Error: Username and password cannot be empty." << endl;
        return;
    }

    User* found = hashSearch(uname);

    if (found == nullptr) {
        cout << "Error: User '" << uname << "' not found." << endl;
        return;
    }
    if (found->password != pass) {
        cout << "Error: Incorrect password." << endl;
        return;
    }

    currentUser = found;
    cout << "\n✓ Login successful! Welcome, " << currentUser->userName << "." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : logoutUser
 * Purpose  : Clears the global currentUser pointer (ends session).
 * Input    : none
 * Output   : currentUser set to nullptr
 ══════════════════════════════════════════════════════════════ */
void logoutUser() {
    if (currentUser == nullptr) {
        cout << "Error: No user is currently logged in." << endl;
        return;
    }
    cout << "Goodbye, " << currentUser->userName << "!" << endl;
    currentUser = nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : deleteUser
 * Purpose  : Removes a user from the hash table and triggers
 *            full Module H cleanup across all modules.
 * Input    : username to delete (prompted from user)
 * Output   : user removed from all modules or error message
 *
 * [UPDATED] Now asks "Are you sure? (y/n)" before deleting
 *           to prevent accidental permanent deletion.
 ══════════════════════════════════════════════════════════════ */
void deleteUser() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string uname;
    cout << "\n--- Delete Account ---" << endl;
    cout << "Enter username to delete: "; cin >> uname;

    // check user exists first
    User* target = hashSearch(uname);
    if (target == nullptr) {
        cout << "Error: User '" << uname << "' not found." << endl;
        return;
    }

    // [UPDATED] confirmation prompt before permanent deletion
    char confirm;
    cout << "Are you sure you want to delete '" << uname << "'? (y/n): ";
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        cout << "Deletion cancelled." << endl;
        return;
    }

    // find and unlink from hash chain
    int   index = hashFunction(uname);
    User* curr  = hashTable[index];
    User* prev  = nullptr;

    while (curr != nullptr && curr->userName != uname) {
        prev = curr;
        curr = curr->next;
    }

    if (prev == nullptr)
        hashTable[index] = curr->next;   // was head of chain
    else
        prev->next = curr->next;

    // MODULE H: full cleanup across all modules
    cleanupDeletedUser(curr);

    // if the deleted user was the one logged in, log them out
    if (currentUser != nullptr && currentUser->userName == uname) {
        currentUser = nullptr;
        cout << "You deleted your own account. Logged out." << endl;
    }

    delete curr;
    cout << "\n✓ Account '" << uname << "' deleted. All data cleaned up." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : searchUser
 * Purpose  : Searches for a user and displays their full profile
 *            including friend count and post count.
 * Input    : username to search (prompted from user)
 * Output   : user profile info or "not found" message
 *
 * [UPDATED] Now prints friend count and post count in addition
 *           to basic info (previous version only showed name/email/bio).
 ══════════════════════════════════════════════════════════════ */
void searchUser() {
    string name;
    cout << "\n--- Search User ---" << endl;
    cout << "Enter username to search: "; cin >> name;

    // [UPDATED] reject empty input
    if (name.empty()) {
        cout << "Error: Username cannot be empty." << endl;
        return;
    }

    User* u = hashSearch(name);
    if (u == nullptr) {
        cout << "User '" << name << "' not found." << endl;
        return;
    }

    // count friends
    int friendCount = 0;
    Edge* edge = u->friendList;
    while (edge != nullptr) {
        friendCount++;
        edge = edge->next;
    }

    // count posts
    int postCount = 0;
    Post* post = u->postHead;
    while (post != nullptr) {
        postCount++;
        post = post->next;
    }

    cout << "\n========= User Profile =========" << endl;
    cout << "Name       : " << u->userName   << endl;
    cout << "Email      : " << u->email      << endl;
    cout << "Bio        : " << u->bio        << endl;
    cout << "Last seen  : " << u->lastActive << endl;
    // [UPDATED] these two lines are new
    cout << "Friends    : " << friendCount   << endl;
    cout << "Posts      : " << postCount     << endl;
    cout << "=================================" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : displayAllUsers
 * Purpose  : Scans all 100 buckets and prints every registered user.
 *            Shows bucket index and chain to illustrate hashing.
 * Input    : none
 * Output   : list of all users grouped by hash bucket
 *
 * [UPDATED] Prints a clear message when no users exist at all,
 *           instead of silently returning an empty output.
 ══════════════════════════════════════════════════════════════ */
void displayAllUsers() {
    cout << "\n--- All Registered Users ---" << endl;
    int totalUsers = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        User* curr = hashTable[i];
        if (curr != nullptr) {
            cout << "Bucket[" << i << "]: ";
            while (curr != nullptr) {
                cout << curr->userName;
                if (curr->next != nullptr) cout << " -> ";
                curr = curr->next;
                totalUsers++;
            }
            cout << endl;
        }
    }

    // [UPDATED] clear message when system has no users yet
    if (totalUsers == 0)
        cout << "No users registered yet. Use Register to create an account." << endl;
    else
        cout << "\nTotal users: " << totalUsers << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : userManagementMenu
 * Purpose  : Displays the User Management submenu in a loop.
 *            Handles all user input and calls appropriate functions.
 * Input    : user menu choice (integer)
 * Output   : calls the matching function or prints error
 *
 * [UPDATED] Invalid non-integer input is now handled — cin.clear()
 *           and cin.ignore() prevent infinite loops on bad input.
 * [UPDATED] searchUser() is now its own function (not inline here).
 ══════════════════════════════════════════════════════════════ */
void userManagementMenu() {
    int choice;
    do {
        cout << "\n========= USER MANAGEMENT =========" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "-----------------------------------" << endl;
        cout << "1. Register User" << endl;
        cout << "2. Login"         << endl;
        cout << "3. Logout"        << endl;
        cout << "4. Delete User"   << endl;
        cout << "5. Search User"   << endl;
        cout << "6. Display All Users" << endl;
        cout << "7. Back"          << endl;
        cout << "-----------------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input gracefully
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 7." << endl;
            continue;
        }

        switch (choice) {
            case 1: registerUser();    break;
            case 2: loginUser();       break;
            case 3: logoutUser();      break;
            case 4: deleteUser();      break;
            case 5: searchUser();      break;
            case 6: displayAllUsers(); break;
            case 7: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1–7." << endl;
        }
    } while (choice != 7);
}

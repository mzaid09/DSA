/*
 * File   : SocialNetwork.cpp
 * Module : B — Social Network (Undirected Graph — Adjacency List)
 * Member : Member 1
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] addFriend   : now checks if user is trying to add themselves.
 * [UPDATED] removeFriend: now prints "users are not friends" if no edge exists.
 * [UPDATED] BFS         : now prints "user has no connections" when friend
 *                         list is empty instead of silently ending.
 * [UPDATED] DFS         : same as BFS — handles zero-friends case clearly.
 * [UPDATED] removeAllEdgesOf: safely handles nullptr friend list (no crash).
 * [UPDATED] socialNetworkMenu: handles non-integer input gracefully.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "SocialNetwork.h"
#include "UserManager.h"   // hashSearch()

/* ══════════════════════════════════════════════════════════════
 * Function : addEdge (internal helper — not in header)
 * Purpose  : Adds a one-directional edge from fromUser → toName.
 *            Inserts at the head of the adjacency list.
 * Input    : fromUser pointer, toName string
 * Output   : new Edge node linked into fromUser->friendList
 ══════════════════════════════════════════════════════════════ */
static void addEdge(User* fromUser, string toName) {
    Edge* newEdge            = new Edge();
    newEdge->friendUserName  = toName;
    newEdge->next            = fromUser->friendList;
    fromUser->friendList     = newEdge;
}

/* ══════════════════════════════════════════════════════════════
 * Function : alreadyFriends (internal helper — not in header)
 * Purpose  : Checks if targetName already exists in user's edge list.
 * Input    : user pointer, targetName string
 * Output   : true if already friends, false otherwise
 ══════════════════════════════════════════════════════════════ */
static bool alreadyFriends(User* user, string targetName) {
    Edge* curr = user->friendList;
    while (curr != nullptr) {
        if (curr->friendUserName == targetName) return true;
        curr = curr->next;
    }
    return false;
}

/* ══════════════════════════════════════════════════════════════
 * Function : addFriend
 * Purpose  : Verifies both users exist, checks for duplicates, then
 *            adds edges in BOTH directions (undirected graph).
 * Input    : user1, user2 — usernames of the two users
 * Output   : friendship added or appropriate error message
 *
 * [UPDATED] Now checks if user1 == user2 (self-friend attempt).
 ══════════════════════════════════════════════════════════════ */
void addFriend(string user1, string user2) {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    // [UPDATED] prevent adding yourself as a friend
    if (user1 == user2) {
        cout << "Error: You cannot add yourself as a friend." << endl;
        return;
    }

    User* u1 = hashSearch(user1);
    User* u2 = hashSearch(user2);

    if (u1 == nullptr) {
        cout << "Error: User '" << user1 << "' not found." << endl;
        return;
    }
    if (u2 == nullptr) {
        cout << "Error: User '" << user2 << "' not found." << endl;
        return;
    }

    if (alreadyFriends(u1, user2)) {
        cout << "Error: " << user1 << " and " << user2 << " are already friends." << endl;
        return;
    }

    addEdge(u1, user2);   // A → B
    addEdge(u2, user1);   // B → A (undirected — both directions)

    cout << "\n✓ " << user1 << " and " << user2 << " are now friends!" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : removeEdge (internal helper — not in header)
 * Purpose  : Removes a one-directional edge for targetName from user's list.
 * Input    : fromUser pointer, targetName string
 * Output   : edge removed from fromUser->friendList
 ══════════════════════════════════════════════════════════════ */
static void removeEdge(User* fromUser, string targetName) {
    Edge* curr = fromUser->friendList;
    Edge* prev = nullptr;

    while (curr != nullptr) {
        if (curr->friendUserName == targetName) {
            if (prev == nullptr)
                fromUser->friendList = curr->next;
            else
                prev->next = curr->next;
            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : removeFriend
 * Purpose  : Removes edges in both directions between user1 and user2.
 * Input    : user1, user2 — usernames to disconnect
 * Output   : friendship removed or error message
 *
 * [UPDATED] Now prints a clear message when the two users are not
 *           actually friends (previously did nothing silently).
 ══════════════════════════════════════════════════════════════ */
void removeFriend(string user1, string user2) {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    User* u1 = hashSearch(user1);
    User* u2 = hashSearch(user2);

    if (u1 == nullptr) {
        cout << "Error: User '" << user1 << "' not found." << endl;
        return;
    }
    if (u2 == nullptr) {
        cout << "Error: User '" << user2 << "' not found." << endl;
        return;
    }

    // [UPDATED] check they are actually friends before trying to remove
    if (!alreadyFriends(u1, user2)) {
        cout << "Error: " << user1 << " and " << user2 << " are not friends." << endl;
        return;
    }

    removeEdge(u1, user2);
    removeEdge(u2, user1);

    cout << "\n✓ Friendship between " << user1 << " and " << user2 << " removed." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : displayFriends
 * Purpose  : Traverses user's Edge* friendList and prints each name.
 * Input    : userName — whose friends to display
 * Output   : list of friend names and total count
 ══════════════════════════════════════════════════════════════ */
void displayFriends(string userName) {
    User* u = hashSearch(userName);
    if (u == nullptr) {
        cout << "Error: User '" << userName << "' not found." << endl;
        return;
    }

    cout << "\nFriends of " << userName << ":" << endl;
    Edge* curr  = u->friendList;
    int   count = 0;

    if (curr == nullptr) {
        cout << "  " << userName << " has no friends yet." << endl;
        return;
    }

    while (curr != nullptr) {
        cout << "  -> " << curr->friendUserName << endl;
        count++;
        curr = curr->next;
    }
    cout << "Total: " << count << " friend(s)" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : BFS
 * Purpose  : Breadth-First Search starting from startUser.
 *            Uses a manual array-based queue (no STL queue).
 *            Visits all direct friends first, then their friends.
 * Input    : startUser — the username to start from
 * Output   : users printed in breadth-first order
 *
 * [UPDATED] Now prints a clear message when the starting user
 *           has no friends (previously just ended silently).
 ══════════════════════════════════════════════════════════════ */
void BFS(string startUser) {
    User* start = hashSearch(startUser);
    if (start == nullptr) {
        cout << "Error: User '" << startUser << "' not found." << endl;
        return;
    }

    // [UPDATED] check for zero friends before starting traversal
    if (start->friendList == nullptr) {
        cout << "User '" << startUser << "' has no connections to traverse." << endl;
        return;
    }

    // manual queue using a string array (no STL)
    string bfsQueue[200];
    string visitedNames[200];
    int    visitedCount = 0;
    int    front = 0, rear = 0;

    // helper lambda to check if already visited
    auto isVisited = [&](string name) -> bool {
        for (int i = 0; i < visitedCount; i++)
            if (visitedNames[i] == name) return true;
        return false;
    };

    // enqueue start user
    bfsQueue[rear++]             = startUser;
    visitedNames[visitedCount++] = startUser;

    cout << "\nBFS Traversal from '" << startUser << "':" << endl;

    while (front < rear) {
        string current = bfsQueue[front++];
        cout << "  Visited: " << current << endl;

        User* currUser = hashSearch(current);
        if (currUser == nullptr) continue;

        Edge* edge = currUser->friendList;
        while (edge != nullptr) {
            if (!isVisited(edge->friendUserName)) {
                bfsQueue[rear++]             = edge->friendUserName;
                visitedNames[visitedCount++] = edge->friendUserName;
            }
            edge = edge->next;
        }
    }
    cout << "BFS complete. " << visitedCount << " user(s) visited." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : DFSHelper (internal recursive helper — not in header)
 * Purpose  : Recursive DFS worker that tracks visited users.
 * Input    : userName, visited array, visitedCount reference
 * Output   : prints each user visited in depth-first order
 ══════════════════════════════════════════════════════════════ */
static void DFSHelper(string userName, string visited[], int& visitedCount) {
    visited[visitedCount++] = userName;
    cout << "  Visited: " << userName << endl;

    User* u = hashSearch(userName);
    if (u == nullptr) return;

    Edge* edge = u->friendList;
    while (edge != nullptr) {
        bool seen = false;
        for (int i = 0; i < visitedCount; i++) {
            if (visited[i] == edge->friendUserName) { seen = true; break; }
        }
        if (!seen)
            DFSHelper(edge->friendUserName, visited, visitedCount);
        edge = edge->next;
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : DFS
 * Purpose  : Depth-First Search from startUser using recursion.
 *            Goes as deep as possible before backtracking.
 * Input    : startUser — the username to start from
 * Output   : users printed in depth-first order
 *
 * [UPDATED] Now prints a clear message when the starting user
 *           has no friends (previously just ended silently).
 ══════════════════════════════════════════════════════════════ */
void DFS(string startUser) {
    User* start = hashSearch(startUser);
    if (start == nullptr) {
        cout << "Error: User '" << startUser << "' not found." << endl;
        return;
    }

    // [UPDATED] check for zero friends before starting traversal
    if (start->friendList == nullptr) {
        cout << "User '" << startUser << "' has no connections to traverse." << endl;
        return;
    }

    string visited[200];
    int    visitedCount = 0;

    cout << "\nDFS Traversal from '" << startUser << "':" << endl;
    DFSHelper(startUser, visited, visitedCount);
    cout << "DFS complete. " << visitedCount << " user(s) visited." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : removeAllEdgesOf
 * Purpose  : Called by Integration (Module H) during account deletion.
 *            Removes this user from every friend's adjacency list,
 *            then deletes this user's own edge list.
 * Input    : userName — the user being deleted
 * Output   : all edges involving this user removed
 *
 * [UPDATED] Now safely handles nullptr friend list (no crash when
 *           the user had no friends at the time of deletion).
 ══════════════════════════════════════════════════════════════ */
void removeAllEdgesOf(string userName) {
    User* u = hashSearch(userName);

    // [UPDATED] safe null check — user may not exist or have no friends
    if (u == nullptr) return;
    if (u->friendList == nullptr) return;

    // for each friend, remove the reverse edge pointing back to this user
    Edge* curr = u->friendList;
    while (curr != nullptr) {
        User* friendUser = hashSearch(curr->friendUserName);
        if (friendUser != nullptr)
            removeEdge(friendUser, userName);
        curr = curr->next;
    }

    // delete this user's own edge list
    curr = u->friendList;
    while (curr != nullptr) {
        Edge* temp = curr;
        curr = curr->next;
        delete temp;
    }
    u->friendList = nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : socialNetworkMenu
 * Purpose  : Displays the Social Network submenu and handles input.
 * Input    : user menu choice (integer)
 * Output   : calls matching function or prints error
 *
 * [UPDATED] Handles non-integer input with cin.clear() + cin.ignore().
 ══════════════════════════════════════════════════════════════ */
void socialNetworkMenu() {
    int choice;
    do {
        cout << "\n======= SOCIAL NETWORK =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "------------------------------" << endl;
        cout << "1. Add Friend"    << endl;
        cout << "2. Remove Friend" << endl;
        cout << "3. View Friends"  << endl;
        cout << "4. BFS Traversal" << endl;
        cout << "5. DFS Traversal" << endl;
        cout << "6. Back"          << endl;
        cout << "------------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 6." << endl;
            continue;
        }

        string u2, name;
        switch (choice) {
            case 1:
                if (!currentUser) { cout << "Error: Please login first." << endl; break; }
                cout << "Friend username: "; cin >> u2;
                addFriend(currentUser->userName, u2);
                break;
            case 2:
                if (!currentUser) { cout << "Error: Please login first." << endl; break; }
                cout << "Remove friend username: "; cin >> u2;
                removeFriend(currentUser->userName, u2);
                break;
            case 3:
                cout << "Enter username to view friends: "; cin >> name;
                displayFriends(name);
                break;
            case 4:
                cout << "Start BFS from user: "; cin >> name;
                BFS(name);
                break;
            case 5:
                cout << "Start DFS from user: "; cin >> name;
                DFS(name);
                break;
            case 6: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1–6." << endl;
        }
    } while (choice != 6);
}

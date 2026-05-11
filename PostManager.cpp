/*
 * File   : PostManager.cpp
 * Module : C — Posts & Feed (Doubly Linked List)
 * Member : Member 2
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] createPost    : now rejects empty post content.
 * [UPDATED] deletePost    : now checks that only the post OWNER can delete it.
 * [UPDATED] likePost      : now prevents a user from liking their OWN post.
 * [UPDATED] likePost      : now prevents DOUBLE liking the same post twice.
 * [UPDATED] navigateFeed  : now handles zero posts in the system gracefully.
 * [UPDATED] navigateFeed  : now prints clear message at first/last post boundary.
 * [UPDATED] postsMenu     : handles non-integer input with cin.clear/ignore.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "PostManager.h"
#include "UserManager.h"
#include "Analytics.h"
#include "Notifications.h"

extern int postIDCounter;

/* ══════════════════════════════════════════════════════════════
 * Function : generatePostID
 * Purpose  : Returns the next unique post ID string (e.g. "P001").
 * Input    : none
 * Output   : string postID
 ══════════════════════════════════════════════════════════════ */
string generatePostID() {
    string id = "P";
    if (postIDCounter < 10)       id += "00" + to_string(postIDCounter);
    else if (postIDCounter < 100) id += "0"  + to_string(postIDCounter);
    else                          id +=         to_string(postIDCounter);
    postIDCounter++;
    return id;
}

/* ══════════════════════════════════════════════════════════════
 * Function : createPost
 * Purpose  : Creates a new Post for the logged-in user and inserts
 *            it at HEAD of their doubly linked list.
 *            Also updates both AVL trees via Module F.
 * Input    : post content typed by user
 * Output   : post inserted or error message
 *
 * [UPDATED] Rejects empty post content.
 ══════════════════════════════════════════════════════════════ */
void createPost() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string content;
    cin.ignore();
    cout << "\n--- Create Post ---" << endl;
    cout << "Enter post content: ";
    getline(cin, content);

    // [UPDATED] reject empty content
    if (content.empty()) {
        cout << "Error: Post content cannot be empty." << endl;
        return;
    }

    Post* newPost    = new Post();
    newPost->postID  = generatePostID();
    newPost->content = content;
    newPost->owner   = currentUser->userName;
    newPost->likes   = 0;
    newPost->prev    = nullptr;

    // insert at HEAD of doubly linked list
    newPost->next = currentUser->postHead;
    if (currentUser->postHead != nullptr)
        currentUser->postHead->prev = newPost;
    currentUser->postHead = newPost;

    insertPostAVL(newPost->postID, 0);       // MODULE F
    incrementUserActivity(currentUser->userName); // MODULE F

    cout << "\n✓ Post created! ID: " << newPost->postID << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : deletePost
 * Purpose  : Removes a post by ID from the logged-in user's list
 *            by relinking prev/next pointers. Also removes from AVL.
 * Input    : post ID typed by user
 * Output   : post removed or error message
 *
 * [UPDATED] Only the post OWNER can delete the post.
 ══════════════════════════════════════════════════════════════ */
void deletePost() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string pid;
    cout << "\n--- Delete Post ---" << endl;
    cout << "Enter Post ID to delete: "; cin >> pid;

    if (pid.empty()) {
        cout << "Error: Post ID cannot be empty." << endl;
        return;
    }

    Post* curr = currentUser->postHead;
    while (curr != nullptr) {
        if (curr->postID == pid) {

            // [UPDATED] ownership check — only owner can delete
            if (curr->owner != currentUser->userName) {
                cout << "Error: You can only delete your own posts." << endl;
                return;
            }

            if (curr->prev != nullptr)
                curr->prev->next = curr->next;
            else
                currentUser->postHead = curr->next;

            if (curr->next != nullptr)
                curr->next->prev = curr->prev;

            deletePostAVL(pid);   // MODULE F
            delete curr;
            cout << "\n✓ Post '" << pid << "' deleted successfully." << endl;
            return;
        }
        curr = curr->next;
    }

    cout << "Error: Post '" << pid << "' not found in your posts." << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : displayMyPosts
 * Purpose  : Prints all posts in the logged-in user's post list.
 * Input    : none (uses currentUser)
 * Output   : all posts listed with ID, likes, and content
 ══════════════════════════════════════════════════════════════ */
void displayMyPosts() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    cout << "\n--- Posts by " << currentUser->userName << " ---" << endl;
    Post* curr = currentUser->postHead;

    if (curr == nullptr) {
        cout << "You have not created any posts yet." << endl;
        return;
    }

    int index = 1;
    while (curr != nullptr) {
        cout << index << ". [" << curr->postID << "] Likes: " << curr->likes << endl;
        cout << "   \"" << curr->content << "\"" << endl;
        cout << "   ─────────────────────────────" << endl;
        curr = curr->next;
        index++;
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : navigateFeed
 * Purpose  : Collects all posts from all users, then lets the
 *            user scroll with N (next) and P (prev).
 * Input    : N / P / B key presses from user
 * Output   : posts displayed one at a time with navigation
 *
 * [UPDATED] Handles zero posts in system gracefully.
 * [UPDATED] Prints boundary message at first/last post.
 ══════════════════════════════════════════════════════════════ */
void navigateFeed() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    extern User* hashTable[];
    Post* feedArray[500];
    int   feedSize = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        User* u = hashTable[i];
        while (u != nullptr) {
            Post* p = u->postHead;
            while (p != nullptr && feedSize < 500) {
                feedArray[feedSize++] = p;
                p = p->next;
            }
            u = u->next;
        }
    }

    // [UPDATED] zero posts — clear message
    if (feedSize == 0) {
        cout << "No posts available in the feed yet." << endl;
        return;
    }

    int  idx = 0;
    char nav  = 'S';

    while (nav != 'B' && nav != 'b') {
        Post* p = feedArray[idx];
        cout << "\n--- Feed [ " << (idx + 1) << " / " << feedSize << " ] ---" << endl;
        cout << "ID    : " << p->postID           << endl;
        cout << "Owner : " << p->owner            << endl;
        cout << "Likes : " << p->likes            << endl;
        cout << "Post  : \"" << p->content << "\"" << endl;
        cout << "[N] Next   [P] Prev   [B] Back : ";
        cin >> nav;

        if (nav == 'N' || nav == 'n') {
            if (idx < feedSize - 1)
                idx++;
            else
                cout << "You are already at the last post." << endl; // [UPDATED]
        } else if (nav == 'P' || nav == 'p') {
            if (idx > 0)
                idx--;
            else
                cout << "You are already at the first post." << endl; // [UPDATED]
        } else if (nav != 'B' && nav != 'b') {
            cout << "Error: Invalid key. Use N, P, or B." << endl;
        }
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : likePost
 * Purpose  : Increments likes on a post. Updates AVL tree (F)
 *            and sends notification to post owner (E).
 * Input    : post ID and owner username from user
 * Output   : likes incremented, notification sent, or error
 *
 * [UPDATED] Prevents liking own post.
 * [UPDATED] Prevents double liking using likedBy[] array in Post struct.
 ══════════════════════════════════════════════════════════════ */
void likePost() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string pid, ownerName;
    cout << "\n--- Like a Post ---" << endl;
    cout << "Post ID    : "; cin >> pid;
    cout << "Post owner : "; cin >> ownerName;

    

    User* owner = hashSearch(ownerName);
    if (owner == nullptr) {
        cout << "Error: User '" << ownerName << "' not found." << endl;
        return;
    }

    Post* curr = owner->postHead;
    while (curr != nullptr) {
        if (curr->postID == pid) {

            for (int i = 0; i < curr->likedByCount; i++) {
                if (curr->likedBy[i] == currentUser->userName) {
                    cout << "Error: You have already liked this post." << endl;
                    return;
                }
            }

            // record liker
            if (curr->likedByCount < 200) {
                curr->likedBy[curr->likedByCount] = currentUser->userName;
                curr->likedByCount++;

                curr->likes++;
                updatePostAVL(pid, curr->likes);

                string notifMsg = currentUser->userName + " liked your post [" + pid + "]";
                enqueueNotification(owner, notifMsg);

                cout << "\n✓ You liked " << ownerName
                     << "'s post! Total likes: " << curr->likes << endl;
            } else {
                cout << "Error: Like limit reached for this post." << endl;
            }
            return;
        }
        curr = curr->next;
    }

    cout << "Error: Post '" << pid << "' not found for user '"
         << ownerName << "'." << endl;
}
/* ══════════════════════════════════════════════════════════════
 * Function : deleteAllPostsOf
 * Purpose  : Called by Module H during account deletion.
 *            Deletes every post node and removes from AVL tree.
 * Input    : user — pointer to user being deleted
 * Output   : all posts freed, AVL entries removed
 ══════════════════════════════════════════════════════════════ */
void deleteAllPostsOf(User* user) {
    if (user == nullptr) return;

    Post* curr = user->postHead;
    while (curr != nullptr) {
        Post* temp = curr;
        curr = curr->next;
        deletePostAVL(temp->postID);
        delete temp;
    }
    user->postHead = nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : postsMenu
 * Purpose  : Displays Posts & Feed submenu and handles input.
 * Input    : integer menu choice
 * Output   : calls matching function or error
 *
 * [UPDATED] Handles non-integer input gracefully.
 ══════════════════════════════════════════════════════════════ */
void postsMenu() {
    int choice;
    do {
        cout << "\n======= POSTS & FEED =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "----------------------------" << endl;
        cout << "1. Create Post"   << endl;
        cout << "2. Delete Post"   << endl;
        cout << "3. View My Posts" << endl;
        cout << "4. Navigate Feed" << endl;
        cout << "5. Like a Post"   << endl;
        cout << "6. Back"          << endl;
        cout << "----------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 6." << endl;
            continue;
        }

        switch (choice) {
            case 1: createPost();     break;
            case 2: deletePost();     break;
            case 3: displayMyPosts(); break;
            case 4: navigateFeed();   break;
            case 5: likePost();       break;
            case 6: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-6." << endl;
        }
    } while (choice != 6);
}

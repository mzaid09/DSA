/*
 * File   : Analytics.cpp
 * Module : F — Analytics & Ranking (AVL Tree x2)
 * Member : Member 3
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] avlGetTopK          : now handles k > total nodes safely.
 * [UPDATED] avlRangeQuery       : now prints "No results found" when range
 *                                 has no matching nodes.
 * [UPDATED] incrementUserActivity: now inserts user if not found in AVL
 *                                  instead of silently failing.
 * [UPDATED] analyticsMenu       : prints "No data yet" when trees are empty.
 * [UPDATED] analyticsMenu       : handles non-integer input gracefully.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "Analytics.h"

/* ── Two AVL tree roots — one for users, one for posts ── */
AVLNode* userAVLRoot = nullptr;
AVLNode* postAVLRoot = nullptr;

/* ══════════════════════════════════════════════════════════════
 * Function : avlHeight
 * Purpose  : Returns the stored height of a node.
 *            Returns 0 if node is nullptr (base case for recursion).
 * Input    : node — AVLNode pointer (can be nullptr)
 * Output   : height integer (0 if nullptr)
 ══════════════════════════════════════════════════════════════ */
int avlHeight(AVLNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlGetBalance
 * Purpose  : Returns the balance factor of a node.
 *            Balance = height(left) - height(right).
 *            Valid range: -1, 0, +1. Outside this → rotate.
 * Input    : node — AVLNode pointer
 * Output   : integer balance factor
 ══════════════════════════════════════════════════════════════ */
int avlGetBalance(AVLNode* node) {
    if (node == nullptr) return 0;
    return avlHeight(node->left) - avlHeight(node->right);
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlRotateRight  (LL Case)
 * Purpose  : Performs right rotation when left subtree is too heavy.
 *            Called when balance factor > +1.
 * Input    : y — the unbalanced node (root of subtree)
 * Output   : x — the new root after rotation
 ══════════════════════════════════════════════════════════════ */
AVLNode* avlRotateRight(AVLNode* y) {
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left  = T2;

    // update heights — y first because it is now lower in the tree
    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;
    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;

    return x;   // x is the new root
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlRotateLeft  (RR Case)
 * Purpose  : Performs left rotation when right subtree is too heavy.
 *            Called when balance factor < -1.
 * Input    : x — the unbalanced node (root of subtree)
 * Output   : y — the new root after rotation
 ══════════════════════════════════════════════════════════════ */
AVLNode* avlRotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;

    y->left  = x;
    x->right = T2;

    // update heights — x first because it is now lower
    x->height = max(avlHeight(x->left), avlHeight(x->right)) + 1;
    y->height = max(avlHeight(y->left), avlHeight(y->right)) + 1;

    return y;   // y is the new root
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlInsert
 * Purpose  : Standard BST insert followed by height update and
 *            balance check. Applies one of four rotations if needed:
 *            LL (right rotate), RR (left rotate),
 *            LR (left-right rotate), RL (right-left rotate).
 * Input    : root — current subtree root
 *            key  — string key to insert
 *            value — integer value for the node
 * Output   : new root of the (sub)tree after insertion and rebalance
 ══════════════════════════════════════════════════════════════ */
AVLNode* avlInsert(AVLNode* root, string key, int value) {
    // Step 1: normal BST insert
    if (root == nullptr) {
        AVLNode* newNode = new AVLNode();
        newNode->key     = key;
        newNode->value   = value;
        return newNode;
    }

    if (key < root->key)
        root->left  = avlInsert(root->left,  key, value);
    else if (key > root->key)
        root->right = avlInsert(root->right, key, value);
    else {
        root->value = value;   // key already exists — update value
        return root;
    }

    // Step 2: update height of current node
    root->height = max(avlHeight(root->left), avlHeight(root->right)) + 1;

    // Step 3: check balance factor
    int balance = avlGetBalance(root);

    // LL Case — right rotation
    if (balance > 1 && key < root->left->key)
        return avlRotateRight(root);

    // RR Case — left rotation
    if (balance < -1 && key > root->right->key)
        return avlRotateLeft(root);

    // LR Case — left rotate left child, then right rotate root
    if (balance > 1 && key > root->left->key) {
        root->left = avlRotateLeft(root->left);
        return avlRotateRight(root);
    }

    // RL Case — right rotate right child, then left rotate root
    if (balance < -1 && key < root->right->key) {
        root->right = avlRotateRight(root->right);
        return avlRotateLeft(root);
    }

    return root;   // already balanced
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlMinNode (internal helper)
 * Purpose  : Returns the node with the smallest key in a subtree.
 *            Used by avlDelete to find the inorder successor.
 * Input    : node — root of subtree to search
 * Output   : pointer to the minimum key node
 ══════════════════════════════════════════════════════════════ */
static AVLNode* avlMinNode(AVLNode* node) {
    AVLNode* curr = node;
    while (curr->left != nullptr)
        curr = curr->left;
    return curr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlDelete
 * Purpose  : Removes a node by key. Handles three cases:
 *            no child, one child, two children (inorder successor).
 *            Rebalances after deletion with the same four rotations.
 * Input    : root — current subtree root
 *            key  — key of node to delete
 * Output   : new root of subtree after deletion and rebalance
 ══════════════════════════════════════════════════════════════ */
AVLNode* avlDelete(AVLNode* root, string key) {
    if (root == nullptr) return root;

    if (key < root->key)
        root->left = avlDelete(root->left, key);
    else if (key > root->key)
        root->right = avlDelete(root->right, key);
    else {
        // node found
        if (root->left == nullptr || root->right == nullptr) {
            AVLNode* temp = (root->left) ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;
            }
            delete temp;
        } else {
            // two children — replace with inorder successor
            AVLNode* temp = avlMinNode(root->right);
            root->key     = temp->key;
            root->value   = temp->value;
            root->right   = avlDelete(root->right, temp->key);
        }
    }

    if (root == nullptr) return root;

    root->height = max(avlHeight(root->left), avlHeight(root->right)) + 1;

    int balance = avlGetBalance(root);

    if (balance > 1  && avlGetBalance(root->left)  >= 0)
        return avlRotateRight(root);

    if (balance > 1  && avlGetBalance(root->left)  <  0) {
        root->left = avlRotateLeft(root->left);
        return avlRotateRight(root);
    }

    if (balance < -1 && avlGetBalance(root->right) <= 0)
        return avlRotateLeft(root);

    if (balance < -1 && avlGetBalance(root->right) >  0) {
        root->right = avlRotateRight(root->right);
        return avlRotateLeft(root);
    }

    return root;
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlSearch
 * Purpose  : Searches the AVL tree for a node by key.
 * Input    : root — subtree root, key — key to find
 * Output   : pointer to node if found, nullptr if not found
 ══════════════════════════════════════════════════════════════ */
AVLNode* avlSearch(AVLNode* root, string key) {
    if (root == nullptr)      return nullptr;
    if (key == root->key)     return root;
    if (key  < root->key)     return avlSearch(root->left,  key);
    return avlSearch(root->right, key);
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlUpdateValue
 * Purpose  : Finds node by key and updates its value in place.
 *            Does not rebalance because key order is unchanged.
 * Input    : root — tree root, key, newValue
 * Output   : node value updated if found
 ══════════════════════════════════════════════════════════════ */
void avlUpdateValue(AVLNode* root, string key, int newValue) {
    AVLNode* node = avlSearch(root, key);
    if (node != nullptr)
        node->value = newValue;
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlInorder
 * Purpose  : Prints all nodes in ascending order by key.
 *            Left → Root → Right traversal.
 * Input    : root — subtree root (recursive)
 * Output   : all nodes printed in sorted order
 ══════════════════════════════════════════════════════════════ */
void avlInorder(AVLNode* root) {
    if (root == nullptr) return;
    avlInorder(root->left);
    cout << "  " << root->key << "  :  " << root->value << endl;
    avlInorder(root->right);
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlCollect (internal helper)
 * Purpose  : Collects all nodes into an array via inorder traversal.
 *            Used by avlGetTopK to sort and extract top results.
 * Input    : root, arr[] to fill, count reference
 * Output   : arr[] populated with all node pointers
 ══════════════════════════════════════════════════════════════ */
static void avlCollect(AVLNode* root, AVLNode* arr[], int& count) {
    if (root == nullptr) return;
    avlCollect(root->left, arr, count);
    arr[count++] = root;
    avlCollect(root->right, arr, count);
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlGetTopK
 * Purpose  : Collects all nodes, sorts by value (descending)
 *            using selection sort, then prints the top K entries.
 * Input    : root — tree root, k — how many top entries to show
 * Output   : top K nodes printed by highest value
 *
 * [UPDATED] Now handles k > total nodes — prints all available
 *           results instead of printing garbage or crashing.
 ══════════════════════════════════════════════════════════════ */
void avlGetTopK(AVLNode* root, int k) {
    AVLNode* arr[500];
    int      count = 0;
    avlCollect(root, arr, count);

    if (count == 0) {
        cout << "No data available yet." << endl;
        return;
    }

    // [UPDATED] cap k at actual node count so we never go out of bounds
    if (k > count) {
        cout << "Note: Only " << count << " entr(ies) available. Showing all." << endl;
        k = count;
    }

    // selection sort by value — descending
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[j]->value > arr[i]->value) {
                AVLNode* temp = arr[i];
                arr[i]        = arr[j];
                arr[j]        = temp;
            }
        }
    }

    cout << "Top " << k << " result(s):" << endl;
    for (int i = 0; i < k; i++) {
        cout << "  " << (i + 1) << ". "
             << arr[i]->key << "  —  score: " << arr[i]->value << endl;
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : avlRangeQuery
 * Purpose  : Prints all nodes whose value falls in [minVal, maxVal].
 *            Uses inorder traversal so output is sorted by key.
 * Input    : root — tree root, minVal, maxVal — inclusive range
 * Output   : matching nodes printed or "no results" message
 *
 * [UPDATED] Now prints "No results found" when the range has
 *           no matching nodes, instead of printing nothing.
 ══════════════════════════════════════════════════════════════ */

// helper to count matches (needed to detect empty result)
static int avlCountInRange(AVLNode* root, int minVal, int maxVal) {
    if (root == nullptr) return 0;
    int count = 0;
    if (root->value >= minVal && root->value <= maxVal) count = 1;
    return count
         + avlCountInRange(root->left,  minVal, maxVal)
         + avlCountInRange(root->right, minVal, maxVal);
}

static void avlPrintRange(AVLNode* root, int minVal, int maxVal) {
    if (root == nullptr) return;
    avlPrintRange(root->left, minVal, maxVal);
    if (root->value >= minVal && root->value <= maxVal)
        cout << "  " << root->key << "  :  " << root->value << endl;
    avlPrintRange(root->right, minVal, maxVal);
}

void avlRangeQuery(AVLNode* root, int minVal, int maxVal) {
    // [UPDATED] check for any results before printing header
    int matchCount = avlCountInRange(root, minVal, maxVal);
    if (matchCount == 0) {
        cout << "No results found in range ["
             << minVal << ", " << maxVal << "]." << endl;
        return;
    }
    avlPrintRange(root, minVal, maxVal);
    cout << matchCount << " result(s) found." << endl;
}

/* ── Module-level wrapper functions called by other modules ── */

/* ══════════════════════════════════════════════════════════════
 * Function : insertUserAVL
 * Purpose  : Inserts a user into the user activity AVL tree.
 *            Called by registerUser() with initial value = 0.
 * Input    : userName, activity score
 * Output   : user inserted into userAVLRoot
 ══════════════════════════════════════════════════════════════ */
void insertUserAVL(string userName, int activity) {
    userAVLRoot = avlInsert(userAVLRoot, userName, activity);
}

/* ══════════════════════════════════════════════════════════════
 * Function : deleteUserAVL
 * Purpose  : Removes a user from the user activity AVL tree.
 *            Called by cleanupDeletedUser() in Integration.cpp.
 * Input    : userName — key to remove
 * Output   : user removed from userAVLRoot
 ══════════════════════════════════════════════════════════════ */
void deleteUserAVL(string userName) {
    userAVLRoot = avlDelete(userAVLRoot, userName);
}

/* ══════════════════════════════════════════════════════════════
 * Function : incrementUserActivity
 * Purpose  : Increments the activity score of a user in the AVL tree.
 *            Called every time a user creates a post.
 * Input    : userName — whose score to increment
 * Output   : node value incremented by 1
 *
 * [UPDATED] Now inserts the user into the AVL tree if they are not
 *           found, instead of silently failing with no effect.
 ══════════════════════════════════════════════════════════════ */
void incrementUserActivity(string userName) {
    AVLNode* node = avlSearch(userAVLRoot, userName);
    if (node != nullptr) {
        node->value++;
    } else {
        // [UPDATED] user not in tree yet — insert with value 1
        userAVLRoot = avlInsert(userAVLRoot, userName, 1);
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : insertPostAVL
 * Purpose  : Inserts a post into the post likes AVL tree.
 *            Called by createPost() with initial likes = 0.
 * Input    : postID, likes count
 * Output   : post inserted into postAVLRoot
 ══════════════════════════════════════════════════════════════ */
void insertPostAVL(string postID, int likes) {
    postAVLRoot = avlInsert(postAVLRoot, postID, likes);
}

/* ══════════════════════════════════════════════════════════════
 * Function : deletePostAVL
 * Purpose  : Removes a post from the post likes AVL tree.
 *            Called by deletePost() and deleteAllPostsOf().
 * Input    : postID — key to remove
 * Output   : post removed from postAVLRoot
 ══════════════════════════════════════════════════════════════ */
void deletePostAVL(string postID) {
    postAVLRoot = avlDelete(postAVLRoot, postID);
}

/* ══════════════════════════════════════════════════════════════
 * Function : updatePostAVL
 * Purpose  : Updates the likes value of a post in the AVL tree.
 *            Called by likePost() after incrementing likes.
 * Input    : postID, newLikes — the updated count
 * Output   : post node value updated in postAVLRoot
 ══════════════════════════════════════════════════════════════ */
void updatePostAVL(string postID, int newLikes) {
    avlUpdateValue(postAVLRoot, postID, newLikes);
}

/* ══════════════════════════════════════════════════════════════
 * Function : analyticsMenu
 * Purpose  : Displays the Analytics & Ranking submenu and handles input.
 * Input    : integer menu choice
 * Output   : calls matching function or error message
 *
 * [UPDATED] Prints "No data available yet" when AVL trees are empty.
 * [UPDATED] Handles non-integer input with cin.clear + cin.ignore.
 ══════════════════════════════════════════════════════════════ */
void analyticsMenu() {
    int choice;
    do {
        cout << "\n======= ANALYTICS & RANKING =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "-----------------------------------" << endl;
        cout << "1. Top Active Users"     << endl;
        cout << "2. Most Liked Posts"     << endl;
        cout << "3. All Users (sorted)"  << endl;
        cout << "4. All Posts (sorted)"  << endl;
        cout << "5. Range Query (posts)" << endl;
        cout << "6. Back"                << endl;
        cout << "-----------------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 6." << endl;
            continue;
        }

        int k, minV, maxV;
        switch (choice) {
            case 1:
                // [UPDATED] check for empty tree before asking for k
                if (userAVLRoot == nullptr) {
                    cout << "No user data available yet." << endl;
                    break;
                }
                cout << "Show top how many users? "; cin >> k;
                avlGetTopK(userAVLRoot, k);
                break;

            case 2:
                // [UPDATED] check for empty tree before asking for k
                if (postAVLRoot == nullptr) {
                    cout << "No post data available yet." << endl;
                    break;
                }
                cout << "Show top how many posts? "; cin >> k;
                avlGetTopK(postAVLRoot, k);
                break;

            case 3:
                cout << "\nAll users sorted by name:" << endl;
                if (userAVLRoot == nullptr)
                    cout << "No user data available yet." << endl;  // [UPDATED]
                else
                    avlInorder(userAVLRoot);
                break;

            case 4:
                cout << "\nAll posts sorted by ID:" << endl;
                if (postAVLRoot == nullptr)
                    cout << "No post data available yet." << endl;  // [UPDATED]
                else
                    avlInorder(postAVLRoot);
                break;

            case 5:
                if (postAVLRoot == nullptr) {
                    cout << "No post data available yet." << endl;  // [UPDATED]
                    break;
                }
                cout << "Min likes: "; cin >> minV;
                cout << "Max likes: "; cin >> maxV;
                if (minV > maxV) {
                    cout << "Error: Min value cannot be greater than Max value." << endl;
                    break;
                }
                cout << "\nPosts with likes in [" << minV << ", " << maxV << "]:" << endl;
                avlRangeQuery(postAVLRoot, minV, maxV);
                break;

            case 6: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-6." << endl;
        }
    } while (choice != 6);
}

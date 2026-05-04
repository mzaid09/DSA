/*
 * File   : Integration.cpp
 * Module : H — Integration (Delete Account Cascade)
 * Member : Member 1
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] cleanupDeletedUser: added null check at start for safety.
 * [UPDATED] cleanupDeletedUser: step numbers now printed clearly to console
 *                               so it is easy to trace during testing.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "Integration.h"
#include "SocialNetwork.h"   // removeAllEdgesOf()
#include "PostManager.h"     // deleteAllPostsOf()
#include "Analytics.h"       // deleteUserAVL()
#include "Messaging.h"       // deleteAllMessagesOf()
#include "Stories.h"         // removeStoriesOf()
#include "Notifications.h"   // clearNotificationsOf()

/* ══════════════════════════════════════════════════════════════
 * Function : cleanupDeletedUser
 * Purpose  : Coordinates the full deletion of a user's data
 *            across ALL modules in the correct safe order.
 *            Called by deleteUser() in UserManager.cpp BEFORE
 *            the User node itself is deleted from the hash table.
 *            Wrong order causes dangling pointers and crashes.
 * Input    : user — pointer to the User node being deleted
 * Output   : all user data removed from all modules
 *
 * [UPDATED] Added nullptr check at start for safety.
 * [UPDATED] Step numbers printed clearly for easier test tracing.
 ══════════════════════════════════════════════════════════════ */
void cleanupDeletedUser(User* user) {
    // [UPDATED] safety check — never proceed with a null pointer
    if (user == nullptr) return;

    cout << "\n[Integration] Starting full account cleanup for: "
         << user->userName << endl;

    // Step 1 — Graph (Module B)
    // Remove this user from every friend's adjacency list.
    // Also deletes this user's own edge list.
    cout << "[Step 1/6] Removing graph edges..." << endl;
    removeAllEdgesOf(user->userName);

    // Step 2 — Posts (Module C)
    // Delete every Post node in user's doubly linked list.
    // deleteAllPostsOf also calls deletePostAVL for each post.
    cout << "[Step 2/6] Deleting posts and post AVL entries..." << endl;
    deleteAllPostsOf(user);

    // Step 3 — AVL User Tree (Module F)
    // Remove user from the user activity ranking tree.
    cout << "[Step 3/6] Removing from user AVL tree..." << endl;
    deleteUserAVL(user->userName);

    // Step 4 — Messages (Module G)
    // Delete all conversation nodes and their message stacks.
    cout << "[Step 4/6] Deleting message stacks and conversations..." << endl;
    deleteAllMessagesOf(user);

    // Step 5 — Stories (Module D)
    // Remove this user's story nodes from the circular list.
    cout << "[Step 5/6] Removing stories from circular list..." << endl;
    removeStoriesOf(user->userName);

    // Step 6 — Notifications (Module E)
    // Clear the user's entire notification queue.
    cout << "[Step 6/6] Clearing notification queue..." << endl;
    clearNotificationsOf(user);

    cout << "[Integration] ✓ Cleanup complete. No dangling pointers." << endl;
}

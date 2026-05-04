/*
 * File   : main.cpp
 * Purpose: Entry point for the Console-Based Social Media Platform.
 *          Defines the global currentUser pointer.
 *          Runs the main menu loop until user exits.
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] main menu: handles non-integer input gracefully.
 * [UPDATED] main menu: shows "Not logged in" vs "Logged in as: X" clearly.
 * ─────────────────────────────────────────────────────────────────────────
 *
 * HOW TO BUILD IN VS CODE TERMINAL:
 * g++ -std=c++17 main.cpp UserManager.cpp SocialNetwork.cpp PostManager.cpp
 *     Stories.cpp Notifications.cpp Analytics.cpp Messaging.cpp Integration.cpp
 *     -o SocialMedia
 *
 * HOW TO RUN:
 *   Windows : SocialMedia.exe
 *   Linux   : ./SocialMedia
 */

#include "globals.h"
#include "UserManager.h"
#include "SocialNetwork.h"
#include "PostManager.h"
#include "Stories.h"
#include "Notifications.h"
#include "Analytics.h"
#include "Messaging.h"
#include "Integration.h"

/* ── Define the global currentUser — declared extern in globals.h ── */
User* currentUser = nullptr;

/* ══════════════════════════════════════════════════════════════
 * Function : showMainMenu
 * Purpose  : Displays the main menu with login status at the top.
 *            Called at the start of every main loop iteration.
 * Input    : none
 * Output   : main menu printed to console
 ══════════════════════════════════════════════════════════════ */
void showMainMenu() {
    cout << "\n=============================================" << endl;
    if (currentUser != nullptr)
        cout << "  Logged in as: " << currentUser->userName  << endl;
    else
        cout << "  Not logged in"                            << endl;
    cout << "=============================================" << endl;
    cout << "     CONSOLE SOCIAL MEDIA PLATFORM           " << endl;
    cout << "=============================================" << endl;
    cout << "  1. User Management     (Hash Table)        " << endl;
    cout << "  2. Social Network      (Graph)             " << endl;
    cout << "  3. Posts & Feed        (Linked Lists)      " << endl;
    cout << "  4. Stories             (Circular List)     " << endl;
    cout << "  5. Notifications       (Queue)             " << endl;
    cout << "  6. Analytics & Ranking (AVL Tree)          " << endl;
    cout << "  7. Messaging System    (Stack)             " << endl;
    cout << "  8. Exit                                    " << endl;
    cout << "=============================================" << endl;
    cout << "  Enter choice: ";
}

/* ══════════════════════════════════════════════════════════════
 * Function : main
 * Purpose  : Program entry point. Runs the main menu loop
 *            until the user selects Exit (8).
 * Input    : none
 * Output   : launches the social media platform
 *
 * [UPDATED] Handles non-integer input with cin.clear + cin.ignore
 *           so the program does not get stuck in an infinite loop.
 ══════════════════════════════════════════════════════════════ */
int main() {
    int choice;

    do {
        showMainMenu();
        cin >> choice;

        // [UPDATED] handle non-integer input gracefully
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 8." << endl;
            continue;
        }

        switch (choice) {
            case 1: userManagementMenu(); break;
            case 2: socialNetworkMenu();  break;
            case 3: postsMenu();          break;
            case 4: storiesMenu();        break;
            case 5: notificationsMenu();  break;
            case 6: analyticsMenu();      break;
            case 7: messagingMenu();      break;
            case 8: cout << "\nExiting... Goodbye!" << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-8." << endl;
        }

    } while (choice != 8);

    return 0;
}

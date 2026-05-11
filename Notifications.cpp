/*
 * File   : Notifications.cpp
 * Module : E — Notifications (Queue — FIFO)
 * Member : Member 2
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] enqueueNotification : now has nullptr check at the very start
 *                                 so it never crashes if targetUser is null.
 * [UPDATED] displayNotifications: now numbers each notification clearly.
 * [UPDATED] dequeueNotification : message improved for empty queue case.
 * [UPDATED] notificationsMenu   : handles non-integer input gracefully.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "Notifications.h"
#include <ctime> 
/* ══════════════════════════════════════════════════════════════
 * Function : enqueueNotification
 * Purpose  : Creates a Notification node and adds it to the REAR
 *            of the target user's notification queue (FIFO).
 *            Called automatically by likePost() and sendMessage().
 * Input    : targetUser — pointer to the user to notify
 *            message    — notification text string
 * Output   : new notification node added at rear of queue
 *
 * [UPDATED] Added nullptr check at the very start — if targetUser
 *           is nullptr for any reason, function returns safely
 *           without crashing the program.
 ══════════════════════════════════════════════════════════════ */
void enqueueNotification(User* targetUser, string message) {
    // [UPDATED] null guard — never crash if targetUser is invalid
    if (targetUser == nullptr) return;
    if (message.empty())       return;

    Notification* newNotif  = new Notification();
    newNotif->message       = message;
    newNotif->targetUser    = targetUser->userName;
    time_t now = time(0);
    struct tm* ltm = localtime(&now);
    char timeBuffer[6];
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", ltm);
    newNotif->timestamp = string(timeBuffer);
    newNotif->next          = nullptr;

    // add to REAR of queue (FIFO — oldest stays at front)
    if (targetUser->notifRear == nullptr) {
        // queue was empty — both front and rear point to the new node
        targetUser->notifFront = newNotif;
        targetUser->notifRear  = newNotif;
    } else {
        targetUser->notifRear->next = newNotif;
        targetUser->notifRear       = newNotif;
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : dequeueNotification
 * Purpose  : Removes and displays the FRONT notification (oldest).
 *            Updates the front pointer to the next node.
 *            If queue becomes empty, resets rear to nullptr too.
 * Input    : none (uses currentUser)
 * Output   : front notification removed and printed or error message
 ══════════════════════════════════════════════════════════════ */
void dequeueNotification() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    if (currentUser->notifFront == nullptr) {
        cout << "No notifications to process." << endl;
        return;
    }

    Notification* temp      = currentUser->notifFront;
    currentUser->notifFront = temp->next;

    // if queue is now empty, also reset rear
    if (currentUser->notifFront == nullptr)
        currentUser->notifRear = nullptr;

    cout << "\n✓ Processed notification:" << endl;
    cout << "   \"" << temp->message << "\"  [" << temp->timestamp << "]" << endl;

    delete temp;
}

/* ══════════════════════════════════════════════════════════════
 * Function : peekNotification
 * Purpose  : Shows the FRONT notification without removing it.
 *            This is a read-only view of the next item to be processed.
 * Input    : none (uses currentUser)
 * Output   : front notification content printed or empty message
 ══════════════════════════════════════════════════════════════ */
void peekNotification() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    if (currentUser->notifFront == nullptr) {
        cout << "No notifications in queue." << endl;
        return;
    }

    cout << "\nNext notification (not removed):" << endl;
    cout << "   \"" << currentUser->notifFront->message
         << "\"  [" << currentUser->notifFront->timestamp << "]" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : displayNotifications
 * Purpose  : Traverses the entire notification queue from front
 *            to rear and prints all notifications with numbers.
 *            Does NOT remove any notifications.
 * Input    : none (uses currentUser)
 * Output   : numbered list of all pending notifications
 *
 * [UPDATED] Now numbers each notification clearly (1, 2, 3...)
 *           so the user can see their total count at a glance.
 ══════════════════════════════════════════════════════════════ */
void displayNotifications() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    cout << "\n--- Notifications for " << currentUser->userName << " ---" << endl;

    Notification* curr = currentUser->notifFront;

    if (curr == nullptr) {
        cout << "You have no notifications." << endl;
        return;
    }

    int index = 1;
    while (curr != nullptr) {
        // [UPDATED] each notification numbered clearly
        cout << "[" << index << "] " << curr->message
             << "  — " << curr->timestamp << endl;
        curr = curr->next;
        index++;
    }

    cout << "Total notifications: " << (index - 1) << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : clearNotificationsOf
 * Purpose  : Called by Module H during account deletion.
 *            Deletes every notification node in the user's queue
 *            and resets both front and rear pointers to nullptr.
 * Input    : user — pointer to the user being deleted
 * Output   : all notification nodes freed, queue emptied
 ══════════════════════════════════════════════════════════════ */
void clearNotificationsOf(User* user) {
    if (user == nullptr) return;

    Notification* curr = user->notifFront;
    while (curr != nullptr) {
        Notification* temp = curr;
        curr = curr->next;
        delete temp;
    }
    user->notifFront = nullptr;
    user->notifRear  = nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : notificationsMenu
 * Purpose  : Displays the Notifications submenu and handles input.
 * Input    : integer menu choice
 * Output   : calls matching function or error message
 *
 * [UPDATED] Handles non-integer input with cin.clear + cin.ignore.
 ══════════════════════════════════════════════════════════════ */
void notificationsMenu() {
    int choice;
    do {
        cout << "\n======= NOTIFICATIONS =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "-----------------------------" << endl;
        cout << "1. View All Notifications"     << endl;
        cout << "2. Process (Dequeue)"          << endl;
        cout << "3. Peek Next Notification"     << endl;
        cout << "4. Back"                       << endl;
        cout << "-----------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 4." << endl;
            continue;
        }

        switch (choice) {
            case 1: displayNotifications(); break;
            case 2: dequeueNotification();  break;
            case 3: peekNotification();     break;
            case 4: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-4." << endl;
        }
    } while (choice != 4);
}

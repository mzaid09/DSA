/*
 * File   : Stories.cpp
 * Module : D — Stories (Circular Linked List)
 * Member : Member 2
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] addStory        : now rejects empty story content.
 * [UPDATED] viewStories     : now prints a clear message when list is empty.
 * [UPDATED] removeStoriesOf : now safely handles the case where the user
 *                             had no stories (nullptr head) without crashing.
 * [UPDATED] storiesMenu     : handles non-integer input gracefully.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "Stories.h"

/* ── Global circular story list — declared extern in globals.h ── */
Story* storyHead  = nullptr;
Story* storyTail  = nullptr;
int    storyCount = 0;

/* ══════════════════════════════════════════════════════════════
 * Function : addStory
 * Purpose  : Creates a new Story node for the logged-in user and
 *            inserts it into the global circular linked list.
 *            After every insert, tail->next always points back to head.
 * Input    : story content typed by user
 * Output   : story added to circular list or error message
 *
 * [UPDATED] Rejects empty story content.
 ══════════════════════════════════════════════════════════════ */
void addStory() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string content;
    cin.ignore();
    cout << "\n--- Add Story ---" << endl;
    cout << "Story content: ";
    getline(cin, content);

    // [UPDATED] reject empty content
    if (content.empty()) {
        cout << "Error: Story content cannot be empty." << endl;
        return;
    }

    Story* newStory        = new Story();
    newStory->userName     = currentUser->userName;
    newStory->storyContent = content;
    newStory->next         = nullptr;

    if (storyHead == nullptr) {
        // first story — circular: points to itself
        storyHead      = newStory;
        storyTail      = newStory;
        newStory->next = storyHead;
    } else {
        // insert after tail and restore circular link
        storyTail->next = newStory;
        storyTail       = newStory;
        storyTail->next = storyHead;   // always restore circular link to head
    }

    storyCount++;
    cout << "\n✓ Story added!" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : viewStories
 * Purpose  : Traverses the circular list exactly storyCount times
 *            and prints each story. Counter prevents infinite loop.
 * Input    : none
 * Output   : all stories printed or "no stories" message
 *
 * [UPDATED] Now prints a clear "No stories available" message
 *           when the list is empty instead of silently returning.
 ══════════════════════════════════════════════════════════════ */
void viewStories() {
    // [UPDATED] explicit check for empty list
    if (storyHead == nullptr || storyCount == 0) {
        cout << "\nNo stories available yet." << endl;
        return;
    }

    cout << "\n--- Viewing Stories ---" << endl;
    Story* curr  = storyHead;
    int    count = storyCount;   // use counter to safely stop the circular loop
    int    index = 1;

    while (count > 0) {
        cout << "Story " << index << " [" << curr->userName << "]: \""
             << curr->storyContent << "\"" << endl;
        curr = curr->next;   // wraps back to head automatically when at tail
        count--;
        index++;
    }

    cout << "\n--- End of stories (" << storyCount << " total) ---" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : removeStoriesOf
 * Purpose  : Called by Module H during account deletion.
 *            Finds and removes all story nodes belonging to userName
 *            while maintaining the circular structure of the list.
 * Input    : userName — the user whose stories must be removed
 * Output   : matching story nodes removed, circular structure preserved
 *
 * [UPDATED] Now safely returns early when storyHead is nullptr
 *           or storyCount is 0, preventing a crash on empty list.
 ══════════════════════════════════════════════════════════════ */
void removeStoriesOf(string userName) {
    // [UPDATED] safe null check — do nothing if list is empty
    if (storyHead == nullptr || storyCount == 0) return;

    bool removedAny = true;

    // keep looping until no more stories by this user remain
    while (removedAny && storyCount > 0) {
        removedAny = false;

        Story* curr = storyHead;
        Story* prev = storyTail;   // in circular list, prev of head is tail

        for (int i = 0; i < storyCount; i++) {
            if (curr->userName == userName) {
                if (storyCount == 1) {
                    // only one node left — empty the list
                    storyHead = nullptr;
                    storyTail = nullptr;
                } else {
                    prev->next = curr->next;
                    if (curr == storyHead) storyHead = curr->next;
                    if (curr == storyTail) storyTail = prev;
                }
                delete curr;
                storyCount--;
                removedAny = true;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

/* ══════════════════════════════════════════════════════════════
 * Function : storiesMenu
 * Purpose  : Displays the Stories submenu and handles input.
 * Input    : integer menu choice
 * Output   : calls matching function or error message
 *
 * [UPDATED] Handles non-integer input with cin.clear + cin.ignore.
 ══════════════════════════════════════════════════════════════ */
void storiesMenu() {
    int choice;
    do {
        cout << "\n======= STORIES =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "-----------------------" << endl;
        cout << "1. Add Story"    << endl;
        cout << "2. View Stories" << endl;
        cout << "3. Back"         << endl;
        cout << "-----------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter 1, 2, or 3." << endl;
            continue;
        }

        switch (choice) {
            case 1: addStory();    break;
            case 2: viewStories(); break;
            case 3: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-3." << endl;
        }
    } while (choice != 3);
}

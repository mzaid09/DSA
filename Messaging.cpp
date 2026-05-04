/*
 * File   : Messaging.cpp
 * Module : G — Messaging System (Stack per Conversation — LIFO)
 * Member : Member 3
 *
 * ── UPDATES FROM PREVIOUS VERSION ───────────────────────────────────────
 * [UPDATED] sendMessage            : now rejects empty message text.
 * [UPDATED] popMessage             : now prints clear message when stack empty.
 * [UPDATED] viewLatestMessage      : handles case where user has no conversations.
 * [UPDATED] displayConversation    : handles empty stack inside a conversation.
 * [UPDATED] findOrCreateConversation: now checks for duplicate conversation before
 *                                    creating a new one (was already doing this,
 *                                    now has explicit comment confirming the check).
 * [UPDATED] messagingMenu          : handles non-integer input gracefully.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include "Messaging.h"
#include "UserManager.h"    // hashSearch()
#include "Notifications.h"  // enqueueNotification()

/* ══════════════════════════════════════════════════════════════
 * Function : findOrCreateConversation
 * Purpose  : Searches the current user's conversation list for a
 *            conversation with 'otherUser'. If not found, creates
 *            a new Conversation node and links it at the head.
 *            Ensures no duplicate conversations are created.
 * Input    : user — the user whose list to search
 *            otherUser — the other person's username
 * Output   : pointer to the existing or newly created Conversation
 ══════════════════════════════════════════════════════════════ */
Conversation* findOrCreateConversation(User* user, string otherUser) {
    // search existing conversations — [UPDATED] this prevents duplicates
    Conversation* curr = user->convList;
    while (curr != nullptr) {
        if (curr->otherUser == otherUser)
            return curr;   // found existing — do NOT create a new one
        curr = curr->next;
    }

    // not found — create new conversation node
    Conversation* newConv  = new Conversation();
    newConv->otherUser     = otherUser;
    newConv->top           = nullptr;
    newConv->next          = user->convList;
    user->convList         = newConv;

    return newConv;
}

/* ══════════════════════════════════════════════════════════════
 * Function : sendMessage
 * Purpose  : Validates sender and receiver, creates a Message node,
 *            and pushes it to the top of the conversation stack (LIFO).
 *            Also pushes a copy to the receiver's side and sends a
 *            notification to the receiver via Module E.
 * Input    : receiver username and message text from user
 * Output   : message pushed to both stacks, notification sent
 *
 * [UPDATED] Now rejects empty message text.
 ══════════════════════════════════════════════════════════════ */
void sendMessage() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    string toUser, text;
    cout << "\n--- Send Message ---" << endl;
    cout << "To user : "; cin >> toUser;

    if (toUser == currentUser->userName) {
        cout << "Error: You cannot message yourself." << endl;
        return;
    }

    // verify receiver exists in hash table (Module A)
    User* receiver = hashSearch(toUser);
    if (receiver == nullptr) {
        cout << "Error: User '" << toUser << "' not found." << endl;
        return;
    }

    cin.ignore();
    cout << "Message : "; getline(cin, text);

    // [UPDATED] reject empty message
    if (text.empty()) {
        cout << "Error: Message cannot be empty." << endl;
        return;
    }

    // create message node
    Message* newMsg   = new Message();
    newMsg->fromUser  = currentUser->userName;
    newMsg->toUser    = toUser;
    newMsg->text      = text;
    newMsg->timestamp = "12:00";   // TODO: replace with ctime

    // push to SENDER's conversation stack
    Conversation* senderConv = findOrCreateConversation(currentUser, toUser);
    newMsg->next             = senderConv->top;
    senderConv->top          = newMsg;

    // push a copy to RECEIVER's conversation stack
    Message* msgCopy    = new Message();
    msgCopy->fromUser   = newMsg->fromUser;
    msgCopy->toUser     = newMsg->toUser;
    msgCopy->text       = newMsg->text;
    msgCopy->timestamp  = newMsg->timestamp;

    Conversation* recvConv = findOrCreateConversation(receiver, currentUser->userName);
    msgCopy->next          = recvConv->top;
    recvConv->top          = msgCopy;

    // MODULE E: notify receiver
    string notifMsg = "New message from " + currentUser->userName;
    enqueueNotification(receiver, notifMsg);

    cout << "\n✓ Message sent to " << toUser << "!" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : viewLatestMessage
 * Purpose  : Peeks at the top of a specific conversation stack.
 *            Shows the most recent message without removing it.
 * Input    : other user's username from current user
 * Output   : top message displayed or appropriate error message
 *
 * [UPDATED] Now handles the case where the user has no conversations
 *           at all, printing a clear message instead of crashing.
 ══════════════════════════════════════════════════════════════ */
void viewLatestMessage() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    // [UPDATED] handle no conversations at all
    if (currentUser->convList == nullptr) {
        cout << "You have no conversations yet." << endl;
        return;
    }

    string otherUser;
    cout << "Conversation with: "; cin >> otherUser;

    Conversation* conv = currentUser->convList;
    while (conv != nullptr) {
        if (conv->otherUser == otherUser) break;
        conv = conv->next;
    }

    if (conv == nullptr) {
        cout << "No conversation found with '" << otherUser << "'." << endl;
        return;
    }

    if (conv->top == nullptr) {
        cout << "No messages in this conversation yet." << endl;
        return;
    }

    // peek — read top without removing
    cout << "\nLatest message:" << endl;
    cout << "  " << conv->top->fromUser << " [" << conv->top->timestamp
         << "]: \"" << conv->top->text << "\"" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : popMessage
 * Purpose  : Removes the top message from a conversation stack.
 *            Frees the memory of the removed Message node.
 * Input    : other user's username from current user
 * Output   : top message removed or error message
 *
 * [UPDATED] Now prints a clear message when the stack is empty
 *           instead of crashing with a nullptr dereference.
 ══════════════════════════════════════════════════════════════ */
void popMessage() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    // [UPDATED] handle no conversations at all
    if (currentUser->convList == nullptr) {
        cout << "You have no conversations yet." << endl;
        return;
    }

    string otherUser;
    cout << "Remove top message from conversation with: "; cin >> otherUser;

    Conversation* conv = currentUser->convList;
    while (conv != nullptr) {
        if (conv->otherUser == otherUser) break;
        conv = conv->next;
    }

    if (conv == nullptr) {
        cout << "No conversation found with '" << otherUser << "'." << endl;
        return;
    }

    // [UPDATED] check for empty stack before popping
    if (conv->top == nullptr) {
        cout << "No messages to remove in this conversation." << endl;
        return;
    }

    // pop — remove top node
    Message* temp = conv->top;
    conv->top     = temp->next;

    cout << "\n✓ Removed: \"" << temp->text << "\"" << endl;
    delete temp;
}

/* ══════════════════════════════════════════════════════════════
 * Function : displayConversation
 * Purpose  : Traverses the conversation stack from top to bottom
 *            (LIFO — newest first) and prints all messages.
 * Input    : other user's username from current user
 * Output   : all messages in conversation printed, newest first
 *
 * [UPDATED] Now prints a clear message when the conversation
 *           exists but the message stack is empty.
 ══════════════════════════════════════════════════════════════ */
void displayConversation() {
    if (currentUser == nullptr) {
        cout << "Error: Please login first." << endl;
        return;
    }

    // [UPDATED] handle no conversations at all
    if (currentUser->convList == nullptr) {
        cout << "You have no conversations yet." << endl;
        return;
    }

    string otherUser;
    cout << "View conversation with: "; cin >> otherUser;

    Conversation* conv = currentUser->convList;
    while (conv != nullptr) {
        if (conv->otherUser == otherUser) break;
        conv = conv->next;
    }

    if (conv == nullptr) {
        cout << "No conversation found with '" << otherUser << "'." << endl;
        return;
    }

    // [UPDATED] conversation exists but stack is empty
    if (conv->top == nullptr) {
        cout << "No messages in this conversation yet." << endl;
        return;
    }

    cout << "\n--- Conversation: " << currentUser->userName
         << " <-> " << otherUser << " ---" << endl;

    Message* curr = conv->top;
    int      index = 1;
    while (curr != nullptr) {
        cout << index << ". " << curr->fromUser
             << " [" << curr->timestamp << "]: \""
             << curr->text << "\"" << endl;
        curr = curr->next;
        index++;
    }
    cout << "--- " << (index - 1) << " message(s) total ---" << endl;
}

/* ══════════════════════════════════════════════════════════════
 * Function : deleteAllMessagesOf
 * Purpose  : Called by Module H during account deletion.
 *            Deletes all conversation nodes and every message
 *            in each conversation's stack. Frees all memory.
 * Input    : user — pointer to the user being deleted
 * Output   : all conversations and message stacks freed
 ══════════════════════════════════════════════════════════════ */
void deleteAllMessagesOf(User* user) {
    if (user == nullptr) return;

    Conversation* conv = user->convList;
    while (conv != nullptr) {
        // delete all messages in this conversation's stack
        Message* msg = conv->top;
        while (msg != nullptr) {
            Message* tempMsg = msg;
            msg = msg->next;
            delete tempMsg;
        }
        Conversation* tempConv = conv;
        conv = conv->next;
        delete tempConv;
    }
    user->convList = nullptr;
}

/* ══════════════════════════════════════════════════════════════
 * Function : messagingMenu
 * Purpose  : Displays the Messaging submenu and handles input.
 * Input    : integer menu choice
 * Output   : calls matching function or error message
 *
 * [UPDATED] Handles non-integer input with cin.clear + cin.ignore.
 ══════════════════════════════════════════════════════════════ */
void messagingMenu() {
    int choice;
    do {
        cout << "\n======= MESSAGING =======" << endl;
        if (currentUser)
            cout << "Logged in as: " << currentUser->userName << endl;
        else
            cout << "Not logged in" << endl;
        cout << "-------------------------" << endl;
        cout << "1. Send Message"          << endl;
        cout << "2. View Latest Message"   << endl;
        cout << "3. Pop Message"           << endl;
        cout << "4. View Full Conversation" << endl;
        cout << "5. Back"                  << endl;
        cout << "-------------------------" << endl;
        cout << "Choice: "; cin >> choice;

        // [UPDATED] handle non-integer input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a number between 1 and 5." << endl;
            continue;
        }

        switch (choice) {
            case 1: sendMessage();         break;
            case 2: viewLatestMessage();   break;
            case 3: popMessage();          break;
            case 4: displayConversation(); break;
            case 5: cout << "Returning to main menu..." << endl; break;
            default: cout << "Error: Invalid choice. Enter 1-5." << endl;
        }
    } while (choice != 5);
}

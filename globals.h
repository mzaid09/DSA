#pragma once
/*
 * File     : globals.h
 * Purpose  : Central header — defines ALL node structs and declares
 *            the global currentUser pointer. Every .cpp includes this.
 *
 * ── UPDATES ─────────────────────────────────────────────────────────────
 * [UPDATED] Post struct: added likedBy[200] array + likedByCount integer
 *           so PostManager can prevent a user from liking the same post twice.
 * ─────────────────────────────────────────────────────────────────────────
 */

#include <iostream>
#include <string>
using namespace std;

/* ── Forward declarations ── */
struct User;
struct Post;
struct Edge;
struct Story;
struct Notification;
struct Message;
struct Conversation;
struct AVLNode;

/* ══════════════════════════════════════════════════════
   MODULE A — User Node (Hash Table)
══════════════════════════════════════════════════════ */
struct User {
    string userName;       // unique key used for hashing
    string password;
    string email;
    string bio;
    string lastActive;

    Post*         postHead;    // head of this user's doubly linked post list
    Edge*         friendList;  // head of this user's adjacency edge list
    Notification* notifFront;  // front of notification queue
    Notification* notifRear;   // rear  of notification queue
    Conversation* convList;    // head of conversation linked list
    User*         next;        // next node in hash table chain (chaining)

    User() {
        postHead   = nullptr;
        friendList = nullptr;
        notifFront = nullptr;
        notifRear  = nullptr;
        convList   = nullptr;
        next       = nullptr;
    }
};

/* ══════════════════════════════════════════════════════
   MODULE B — Edge Node (Graph adjacency list)
══════════════════════════════════════════════════════ */
struct Edge {
    string friendUserName;
    Edge*  next;
    Edge() { next = nullptr; }
};

/* ══════════════════════════════════════════════════════
   MODULE C — Post Node (Doubly Linked List)
   [UPDATED] likedBy[] + likedByCount added to prevent double-liking.
══════════════════════════════════════════════════════ */
struct Post {
    string postID;
    string content;
    string owner;
    int    likes;

    string likedBy[200];   // [UPDATED] stores usernames who already liked
    int    likedByCount;   // [UPDATED] how many users have liked this post

    Post*  next;           // forward  pointer (newer → older)
    Post*  prev;           // backward pointer (older → newer)

    Post() {
        likes        = 0;
        likedByCount = 0;
        next         = nullptr;
        prev         = nullptr;
    }
};

/* ══════════════════════════════════════════════════════
   MODULE D — Story Node (Circular Linked List)
══════════════════════════════════════════════════════ */
struct Story {
    string userName;
    string storyContent;
    Story* next;           // last node's next points back to head
    Story() { next = nullptr; }
};

/* ══════════════════════════════════════════════════════
   MODULE E — Notification Node (Queue — FIFO)
══════════════════════════════════════════════════════ */
struct Notification {
    string message;
    string targetUser;
    string timestamp;
    Notification* next;
    Notification() { next = nullptr; }
};

/* ══════════════════════════════════════════════════════
   MODULE F — AVL Tree Node
══════════════════════════════════════════════════════ */
struct AVLNode {
    string   key;      // userName OR postID
    int      value;    // activity count OR likes count
    AVLNode* left;
    AVLNode* right;
    int      height;
    AVLNode() { value = 0; left = nullptr; right = nullptr; height = 1; }
};

/* ══════════════════════════════════════════════════════
   MODULE G — Message Node (Stack — LIFO)
══════════════════════════════════════════════════════ */
struct Message {
    string   fromUser;
    string   toUser;
    string   text;
    string   timestamp;
    Message* next;         // stack pointer (top → bottom)
    Message() { next = nullptr; }
};

/* ══════════════════════════════════════════════════════
   MODULE G — Conversation Node (linked list of message stacks)
══════════════════════════════════════════════════════ */
struct Conversation {
    string        otherUser;
    Message*      top;         // top of message stack (most recent)
    Conversation* next;
    Conversation() { top = nullptr; next = nullptr; }
};

/* ── Global constants ── */
const int TABLE_SIZE = 100;

/* ── Global currentUser — defined once in main.cpp ── */
extern User* currentUser;

/* ── Global story list — defined in Stories.cpp ── */
extern Story* storyHead;
extern Story* storyTail;
extern int    storyCount;

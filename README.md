# 📱 Console-Based Social Media Platform
### Data Structures — Semester Project | C++

---

## 👥 Team Members

| Member | Role | Modules |
|--------|------|---------|
| Zaid| Foundation & Graph | User Directory (A), Social Network (B), Integration (H), Main Menu |
| Haram | Content & Alerts | Posts & Feed (C), Stories (D), Notifications (E) |
| Ayesha| Analytics & Messaging | AVL Tree Analytics (F), Messaging System (G) |

---

## 📌 Project Overview

A fully integrated **console-based Social Media Platform** built in C++ that demonstrates real-world usage of multiple data structures working together as a single cohesive system.

> This is **not** a collection of isolated data structure implementations.
> Every module connects to and communicates with every other module.

---

## ⚙️ Rules & Constraints

- ❌ No STL containers — `vector`, `list`, `map`, `stack`, `queue` are **NOT** used anywhere
- ✅ All structures implemented using raw **pointers and linked lists**
- ✅ System is **fully integrated** — no module works in isolation
- ✅ Global `currentUser` pointer maintains session across all modules
- ✅ Every sensitive operation checks `currentUser != nullptr` before executing
- ✅ Every function has a **comment block** describing its purpose
- ✅ Proper **indentation** and **meaningful variable names** throughout

---

## 🗂️ File Structure

```
SocialMediaPlatform/
│
├── main.cpp               ← Entry point, main menu loop
├── globals.h              ← ALL structs + global currentUser pointer
│
├── UserManager.h/.cpp     ← Module A : Hash Table
├── SocialNetwork.h/.cpp   ← Module B : Graph
├── PostManager.h/.cpp     ← Module C : Doubly Linked List
├── Stories.h/.cpp         ← Module D : Circular Linked List
├── Notifications.h/.cpp   ← Module E : Queue (FIFO)
├── Analytics.h/.cpp       ← Module F : AVL Tree x2
├── Messaging.h/.cpp       ← Module G : Stack (LIFO)
└── Integration.h/.cpp     ← Module H : Delete Account Cascade
```

---

## 🧱 Data Structures Used

| Module | Feature | Data Structure |
|--------|---------|----------------|
| A | User Directory | Hash Table with Chaining |
| B | Friend Network | Undirected Graph (Adjacency List) |
| C | Posts & Feed | Doubly Linked List |
| D | Stories | Circular Linked List |
| E | Notifications | Queue (FIFO) |
| F | Rankings | AVL Tree × 2 (self-balancing BST) |
| G | Messaging | Stack (LIFO) per Conversation |
| H | Account Deletion | Integration across all modules |

---

## 🔗 Module Descriptions

### Module A — User Directory (Hash Table)
- Stores all user accounts in an array of 100 buckets
- Hash function: sum of ASCII values of username characters `% 100`
- Collisions handled via **linked list chaining**
- Every other module calls `hashSearch()` to validate users
- Features: Register, Login, Logout, Search, Delete, Display All

### Module B — Social Network (Graph)
- Each user node holds an `Edge*` adjacency list of friends
- Friendships are **undirected** — adding A→B also adds B→A
- **BFS** explores friends level by level using a manual queue
- **DFS** explores deeply using recursion
- Features: Add Friend, Remove Friend, View Friends, BFS, DFS

### Module C — Posts & Feed (Doubly Linked List)
- Each user has their own doubly linked post list (`postHead`)
- `next` = scroll forward, `prev` = scroll backward through feed
- New posts inserted at the **head** of the list (newest first)
- Liking a post triggers AVL update (F) and notification (E)
- Features: Create, Delete, View, Navigate Feed, Like

### Module D — Stories (Circular Linked List)
- One global circular list holds all stories from all users
- `tail->next` always points back to `head` (circular property)
- A **counter** prevents infinite loops when viewing
- Features: Add Story, View Stories (looped display)

### Module E — Notifications (Queue — FIFO)
- Each user node holds `notifFront` and `notifRear` pointers
- New notifications added at **rear**, processed from **front**
- Automatically triggered by: post likes, received messages
- Features: Enqueue, Dequeue, Peek, Display All

### Module F — Analytics & Ranking (AVL Tree)
- **Two separate AVL trees:**
  - `userAVLRoot` — ranks users by activity score
  - `postAVLRoot` — ranks posts by likes count
- Self-balances after every insert/delete using rotations (LL, RR, LR, RL)
- Balance factor must stay between -1 and +1
- Features: Insert, Delete, Search, Update, Inorder, Top-K, Range Query

### Module G — Messaging System (Stack — LIFO)
- Each user has a linked list of `Conversation` nodes
- Each conversation holds a **stack** of `Message` nodes
- Most recent message is always on **top** of the stack
- Sender and receiver are both verified via hash table before send
- Features: Send, View Latest, Pop, Display Conversation

### Module H — Integration
- Coordinates full cleanup when an account is deleted
- **Cleanup order (must not be changed):**
  1. Remove all graph edges (Module B)
  2. Delete all posts (Module C)
  3. Remove from AVL post tree (Module F)
  4. Remove from AVL user tree (Module F)
  5. Delete all messages and conversations (Module G)
  6. Remove stories from circular list (Module D)
  7. Clear notification queue (Module E)

---

## 🔄 Integration Flow

```
Register User  →  Hash Table Insert  →  AVL User Insert (value=0)

Login          →  currentUser = &userNode

Create Post    →  DLL Insert at Head  →  AVL Post Insert  →  AVL User Activity++

Like Post      →  post.likes++  →  AVL Update  →  Enqueue Notification

Send Message   →  Verify Both Users  →  Push to Stack  →  Enqueue Notification

Delete Account →  Remove Edges  →  Delete Posts  →  Remove from AVLs
               →  Delete Messages  →  Remove Stories  →  Clear Notifications
```

---

## 🖥️ Main Menu

```
=============================================
  Logged in as: ayesha
=============================================
       CONSOLE SOCIAL MEDIA PLATFORM
=============================================
  1. User Management     (Hash Table)
  2. Social Network      (Graph)
  3. Posts & Feed        (Linked Lists)
  4. Stories             (Circular List)
  5. Notifications       (Queue)
  6. Analytics & Ranking (AVL Tree)
  7. Messaging System    (Stack)
  8. Exit
=============================================
```

---

## 🔐 currentUser System

```cpp
// Defined once in main.cpp
User* currentUser = nullptr;

// Set on successful login
currentUser = foundUserNode;

// Cleared on logout or account deletion
currentUser = nullptr;

// Checked at the top of every sensitive function
if (currentUser == nullptr) {
    cout << "Error: Please login first." << endl;
    return;
}
```

---

## 🚀 How to Run in Visual Studio

### Step 1 — Create Project
- Open Visual Studio
- Select **Create a new project**
- Choose **Empty Project** (C++)
- Name it `SocialMediaPlatform`

### Step 2 — Add Source Files
- Right-click **Source Files** in Solution Explorer
- Select **Add → Existing Item**
- Add all **9 `.cpp` files**:
  - `main.cpp`, `UserManager.cpp`, `SocialNetwork.cpp`
  - `PostManager.cpp`, `Stories.cpp`, `Notifications.cpp`
  - `Analytics.cpp`, `Messaging.cpp`, `Integration.cpp`

### Step 3 — Add Header Files (for reference)
- Right-click **Header Files**
- Select **Add → Existing Item**
- Add all **9 `.h` files**

### Step 4 — Build and Run
- Press `Ctrl + F5` to build and run without debugger
- Or press `F5` to run with debugger

> ⚠️ Make sure all files are in the **same folder** before adding them to the project.

---

## ⏱️ Time Complexity Summary

| Module | Operation | Time Complexity |
|--------|-----------|----------------|
| Hash Table | Search / Insert | O(1) average |
| Hash Table | Worst case (all in one chain) | O(n) |
| Graph | Add / Remove Friend | O(1) |
| Graph | BFS / DFS | O(V + E) |
| Doubly Linked List | Insert at head | O(1) |
| Doubly Linked List | Delete by ID | O(n) |
| Circular List | Add Story | O(1) |
| Queue | Enqueue / Dequeue | O(1) |
| AVL Tree | Insert / Delete / Search | O(log n) |
| AVL Tree | Inorder Traversal | O(n) |
| Stack | Push / Pop / Peek | O(1) |

---

## 📋 Node Structures (Quick Reference)

```cpp
struct User    { string userName, password, email, bio, lastActive;
                 Post* postHead; Edge* friendList;
                 Notification* notifFront, *notifRear;
                 Conversation* convList; User* next; };

struct Edge    { string friendUserName; Edge* next; };

struct Post    { string postID, content, owner; int likes;
                 Post* next; Post* prev; };

struct Story   { string userName, storyContent; Story* next; };

struct Notification { string message, targetUser, timestamp;
                      Notification* next; };

struct AVLNode { string key; int value, height;
                 AVLNode* left; AVLNode* right; };

struct Message { string fromUser, toUser, text, timestamp;
                 Message* next; };

struct Conversation { string otherUser; Message* top;
                      Conversation* next; };
```

---

## 🏫 Academic Information

- **Course:** Data Structures
- **Project Type:** Semester Project
- **Language:** C++
- **IDE:** VS code
- **Type:** Console Application (no graphics)


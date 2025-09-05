// Project 3
#ifndef SQUEUE_H
#define SQUEUE_H
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;
class Grader;   // forward declaration (for grading purposes)
class Tester;   // forward declaration (for testing purposes)
class SQueue;   // forward declaration
class Post;     // forward declaration
#define DEFAULTPOSTID 100000
const int MINPOSTID = 100001;//minimum post ID
const int MAXPOSTID = 999999;//maximum post ID
const int MINLIKES = 0;//lowest priority 
const int MAXLIKES = 500;//highest priority
const int MININTERESTLEVEL = 1;//lowest priority
const int MAXINTERESTLEVEL = 10;//highest priority
const int MINCONLEVEL = 1;//highest priority
const int MAXCONLEVEL = 5;//lowest priority
const int MINTIME = 1;//highest priority
const int MAXTIME = 50;//lowest priority
enum HEAPTYPE {MINHEAP, MAXHEAP};
enum STRUCTURE {SKEW, LEFTIST};

// Priority function pointer type
typedef int (*prifn_t)(const Post&);

class Post{
    public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    friend class SQueue;
    Post(){
        m_postID = DEFAULTPOSTID;m_likes = MINLIKES;
        m_connectLevel = MAXCONLEVEL;m_postTime = MAXTIME;
        m_interestLevel = MININTERESTLEVEL;
        m_right = nullptr;
        m_left = nullptr;
        m_npl = 0;
    }
    Post(int ID, int likes, int connectLevel, int postTime, int interestLevel){
        if (ID < MINPOSTID || ID > MAXPOSTID) m_postID = DEFAULTPOSTID;
        else m_postID = ID;
        if (likes < MINLIKES || likes > MAXLIKES) m_likes = MINLIKES;
        else m_likes = likes;
        if (connectLevel < MINCONLEVEL || connectLevel > MAXCONLEVEL) m_connectLevel = MAXCONLEVEL;
        else m_connectLevel = connectLevel;
        if (postTime < MINTIME || postTime > MAXTIME) m_postTime = MAXTIME;
        else m_postTime = postTime;
        if (interestLevel < MININTERESTLEVEL || interestLevel > MAXINTERESTLEVEL) m_interestLevel = MININTERESTLEVEL;
        else m_interestLevel = interestLevel;
        m_right = nullptr;
        m_left = nullptr;
        m_npl = 0;
    }
    int getPostID() const {return m_postID;}
    int getNumLikes() const {return m_likes;}
    int getConnectLevel() const {return m_connectLevel;}
    int getPostTime() const {return m_postTime;}
    int getInterestLevel() const {return m_interestLevel;}
    void setNPL(int npl) {m_npl = npl;}
    int getNPL() const {return m_npl;}
    // Overloaded insertion operator for Post
    friend ostream& operator<<(ostream& sout, const Post& post);

    private:
    int m_postID;           // every post is identified by a unique ID
    int m_likes;            // 0-500, constant variables defined for this
    // m_connectLevel shows how close the relationship is between the poster and the connection who sees the posts
    // 1 indicates that the poster and the connection are directly related
    // 2 indicates that the poster is related to the connection through another person
    int m_connectLevel;     // 1-5, constant variables defined for this
    // m_postTime is an integer indicating in what order messages are posted
    // it can be between 1 and 50, at any time only 50 posts will be processed for a user
    // therefore this value cannot go beyond 50
    // a value of 1 means this was the first message posted
    // a value of 50 means this was the 50th message posted
    int m_postTime;         // 1-50, constant variables defined for this
    // m_interestLevel indicates how interesting is the post for the connection whi sees it
    // a value of 1 expresses the lowest level of interest
    // a value of 10 expresses the highest level of interest 
    int m_interestLevel;    // 1-10, constant variables defined for this

    Post * m_right;   // right child
    Post * m_left;    // left child
    int m_npl;        // null path length for leftist heap
};

class SQueue{
    public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    
    SQueue(){}
    SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure);
    ~SQueue();
    SQueue(const SQueue& rhs);
    SQueue& operator=(const SQueue& rhs);
    bool insertPost(const Post& post);
    Post getNextPost(); // Returns the highest priority post
    void mergeWithQueue(SQueue& rhs);
    void clear();
    int numPosts() const; // Returns number of posts in queue
    void printPostsQueue() const; // Print the queue using preorder traversal
    prifn_t getPriorityFn() const;
    // Set a new priority function. Must rebuild the heap!!!
    void setPriorityFn(prifn_t priFn, HEAPTYPE heapType);
    HEAPTYPE getHeapType() const;
    STRUCTURE getStructure() const;
    // Set a new data structure (skew/leftist). Must rebuild the heap!!!
    void setStructure(STRUCTURE structure);
    void dump() const; // For debugging purposes

    private:
    Post * m_heap;          // Pointer to root of the heap
    int m_size;             // Current size of the heap
    prifn_t m_priorFunc;    // Function to compute priority
    HEAPTYPE m_heapType;    // either a MINHEAP or a MAXHEAP
    STRUCTURE m_structure;  // skew heap or leftist heap

    void dump(Post *pos) const; // helper function for dump

    /******************************************
     * Private function declarations go here! *
     ******************************************/
    
     Post* mergeNodes(Post* h1, Post* h2);
     void clearHelper(Post* node);
     Post* deepCopy(Post* node);
     void rebuildHeap(); // rebuild using current priority function/structure
 
     // Added private helper functions (allowed modifications)
     static bool comparePosts(prifn_t func, HEAPTYPE heapType, const Post* h1, const Post* h2);
     static void rebuildHelper(Post* node, Post*& newHeap, SQueue* queuePtr);
 
     // Recursive helper for printPostsQueue
     void printPreOrder(Post* node) const;
};

ostream& operator<<(ostream& sout, const Post& post);
#endif

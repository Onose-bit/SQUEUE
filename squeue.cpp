/*Title: squeue.cpp
  Author: Onosetale Okooboh
  Date: 04/14/2025
  Description: This file implements the functions in squeue.h
*/
#include "squeue.h"

// Constructor
SQueue::SQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
  m_priorFunc = priFn;
  m_heapType = heapType;
  m_structure = structure;
  m_heap = nullptr;
  m_size = 0;
}

// --- Destructor ---
SQueue::~SQueue() {
  clear();
}

// Clear helper (post-order deletion) 
void SQueue::clearHelper(Post* node) {
  if (node) {
    clearHelper(node->m_left);
    clearHelper(node->m_right);
    delete node;
  }
}

// Clear the entire heap 
void SQueue::clear() {
  clearHelper(m_heap);
  m_heap = nullptr;
  m_size = 0;
}

// Deep copy helper (recursive)
Post* SQueue::deepCopy(Post* node) {
  if (!node) return nullptr;
  Post* newNode = new Post(node->getPostID(), node->getNumLikes(),
                           node->getConnectLevel(), node->getPostTime(),
                           node->getInterestLevel());
  newNode->m_npl = node->m_npl;
  newNode->m_left = deepCopy(node->m_left);
  newNode->m_right = deepCopy(node->m_right);
  return newNode;
}

// Copy constructor 
SQueue::SQueue(const SQueue& rhs) {
  m_priorFunc = rhs.m_priorFunc;
  m_heapType = rhs.m_heapType;
  m_structure = rhs.m_structure;
  m_size = rhs.m_size;
  m_heap = deepCopy(rhs.m_heap);
}

// Assignment operator 
SQueue& SQueue::operator=(const SQueue& rhs) {
  if (this != &rhs) {
    clear();
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_size = rhs.m_size;
    m_heap = deepCopy(rhs.m_heap);
  }
  return *this;
}

// Static helper: Compare two posts based on the current priority function and heap type 
bool SQueue::comparePosts(prifn_t func, HEAPTYPE heapType, const Post* h1, const Post* h2) {
  int p1 = func(*h1);
  int p2 = func(*h2);
  if (heapType == MINHEAP){
  return (p1 <= p2);  // smaller value = higher priority
  }else{ 
  return (p1 >= p2);  // larger value = higher priority
  }
}
  
// Merge two heaps into one 
Post* SQueue::mergeNodes(Post* h1, Post* h2) {
  if (!h1) return h2;
  if (!h2) return h1;
  
  // Use comparePosts to decide which root should be on top.
  if (!comparePosts(m_priorFunc, m_heapType, h1, h2)) {
    Post* temp = h1;
    h1 = h2;
    h2 = temp;
  }
  
  if (m_structure == SKEW) {
    // Skew heap: swap left and right child after merge.
    Post* temp = h1->m_left;
    h1->m_left = mergeNodes(h1->m_right, h2);
    h1->m_right = temp;
  } else { // LEFTIST
    // Leftist heap: merge right subtrees and then ensure leftist property.
    h1->m_right = mergeNodes(h1->m_right, h2);
    int nplLeft = (h1->m_left ? h1->m_left->m_npl : 0);
    int nplRight = (h1->m_right ? h1->m_right->m_npl : 0);
    if (nplLeft < nplRight) {
      Post* temp = h1->m_left;
      h1->m_left = h1->m_right;
      h1->m_right = temp;
    }
    h1->m_npl = (h1->m_right ? h1->m_right->m_npl + 1 : 0);
  }
  return h1;
}
  
// Merge this queue with another queue
void SQueue::mergeWithQueue(SQueue& rhs) {
  if (this == &rhs){ 
    throw domain_error("Cannot merge queue with itself.");
  }
  // Ensure both queues have the same priority function, heap type, and structure.
  if (m_priorFunc != rhs.m_priorFunc ||
  m_heapType != rhs.m_heapType ||
  m_structure != rhs.m_structure)
  throw domain_error("Incompatible queues cannot be merged.");
  
  m_heap = mergeNodes(m_heap, rhs.m_heap);
  m_size += rhs.m_size;
  
  // Empty the rhs queue.
  rhs.m_heap = nullptr;
  rhs.m_size = 0;
}
  
// Insert a Post into the queue
bool SQueue::insertPost(const Post& post) {
  // Check validity via the priority function; if invalid (0) then do not insert.
  if (m_priorFunc(post) == 0){ 
    return false;
  }
  // Create a new node (copy of post)
  Post* newNode = new Post(post.getPostID(), post.getNumLikes(),
                          post.getConnectLevel(), post.getPostTime(),
                          post.getInterestLevel());
  newNode->m_left = newNode->m_right = nullptr;
  newNode->m_npl = 0;
  
  m_heap = mergeNodes(m_heap, newNode);
  m_size++;
  return true;
}
  
// Return the number of posts in the queue 
int SQueue::numPosts() const {
  return m_size;
}
  
// Get current priority function 
prifn_t SQueue::getPriorityFn() const {
  return m_priorFunc;
}
  
// Remove and return the highest priority Post
Post SQueue::getNextPost() {
  if (!m_heap){ 
  throw out_of_range("Queue is empty");
  }
  // Save the root post
  Post result = *m_heap;
  // Merge the left and right subtrees
  Post* oldRoot = m_heap;
  m_heap = mergeNodes(m_heap->m_left, m_heap->m_right);
  delete oldRoot;
  m_size--;
  return result;
}
  
// Change the priority function and rebuild the heap 
void SQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
  m_priorFunc = priFn;
  m_heapType = heapType;
  // Rebuild the heap with the new priority function.
  rebuildHeap();
}
  
//  Change the structure (skew/leftist) and rebuild the heap 
void SQueue::setStructure(STRUCTURE structure) {
  m_structure = structure;
  // Rebuild the heap with the new structure.
  rebuildHeap();
}
  
//  Get current structure (SKEW or LEFTIST) 
STRUCTURE SQueue::getStructure() const {
  return m_structure;
}
  
// --- Get current heap type ---
HEAPTYPE SQueue::getHeapType() const {
  return m_heapType;
}
  
// Static helper: Rebuild helper 
// Does a preorder traversal, detaches nodes, and merges them into newHeap.
void SQueue::rebuildHelper(Post* node, Post*& newHeap, SQueue* queuePtr) {
  if (!node) return;
  Post* left = node->m_left;
  Post* right = node->m_right;
  node->m_left = node->m_right = nullptr;
  node->m_npl = 0;
  newHeap = queuePtr->mergeNodes(newHeap, node);
  rebuildHelper(left, newHeap, queuePtr);
  rebuildHelper(right, newHeap, queuePtr);
}
  
// Rebuild the heap (used in setPriorityFn and setStructure) 
void SQueue::rebuildHeap() {
  Post* newHeap = nullptr;
  rebuildHelper(m_heap, newHeap, this);
  m_heap = newHeap;
}
  
// Preorder traversal printing helper for printPostsQueue 
void SQueue::printPreOrder(Post* node) const {
  if (!node) return;
  // Print current node: print priority in [ ] and then the Post details
  cout << "[" << m_priorFunc(*node) << "] " << *node << "\n";
  printPreOrder(node->m_left);
  printPreOrder(node->m_right);
}

// Print the posts in the queue using preorder traversal 
void SQueue::printPostsQueue() const {
  cout << "Contents of the queue:" << "\n";
  printPreOrder(m_heap);
}
  
// Dump functions (for debugging) 
void SQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n";
  } else {
    dump(m_heap);
  }
  cout << endl;
}
  
void SQueue::dump(Post *pos) const {
  if (pos != nullptr) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW){ 
      cout << m_priorFunc(*pos) << ":" << pos->m_postID;
    }
    else{ 
      cout << m_priorFunc(*pos) << ":" << pos->m_postID << ":" << pos->m_npl;
      dump(pos->m_right);
      cout << ")";
    }
  }
}
  
ostream& operator<<(ostream& sout, const Post& post) {
  sout << "Post#: " << post.getPostID()
  << ", likes#: " << post.getNumLikes()
  << ", connect level: " << post.getConnectLevel();
  return sout;
}

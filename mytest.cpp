/*Title: mytest.cpp
  Author: Onosetale Okooboh
  Date: 04/14/2025
  Description: This file contains functions that perform individual tests.
  Each test function returns true if the test passes, false otherwise.
*/
#include "squeue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

// ---------------------- Priority Functions ----------------------
int priorityFn1(const Post &post) {
    int priority = post.getNumLikes() + post.getInterestLevel();
    return (priority >= 1 && priority <= 510) ? priority : 0;
}

int priorityFn2(const Post &post) {
    int priority = post.getPostTime() + post.getConnectLevel();
    return (priority >= 2 && priority <= 55) ? priority : 0;
}

enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(){}
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type){
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = std::mt19937(10);// 10 is the fixed seed value
        m_unidist = std::uniform_int_distribution<>(min,max);
    }
    void getShuffle(vector<int> & array){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    int getMin(){return m_min;}
    int getMax(){return m_max;}
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

// ---------------------- Tester Class Definition ----------------------
class Tester {
    public:
    // Helper: Check that extracted priorities are in non-decreasing (min-heap)
    // or non-increasing (max-heap) order.
    bool checkRemovalOrder(const vector<int>& priorities, bool isMinHeap) {
        for (size_t i = 1; i < priorities.size(); i++) {
            if (isMinHeap && priorities[i - 1] > priorities[i]) return false;
            if (!isMinHeap && priorities[i - 1] < priorities[i]) return false;
        }
        return true;
    }
        
    // Helper: Generate a random Post using Random instance.
    Post randomPost(Random &r) {
        return Post(
            r.getRandNum(),
            r.getRandNum() % (MAXLIKES + 1),
            (r.getRandNum() % MAXCONLEVEL) + MINCONLEVEL,
            (r.getRandNum() % MAXTIME) + MINTIME,
            (r.getRandNum() % MAXINTERESTLEVEL) + MININTERESTLEVEL
        );
    }
        
    // Test insertion for min-heap (300 nodes).
    bool testMinHeapInsert() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn2, MINHEAP, LEFTIST);
        const int numNodes = 300;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> priorities;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            priorities.push_back(priorityFn2(p));
        }
        return checkRemovalOrder(priorities, true);
    }
        
    // Test insertion for max-heap (300 nodes).
    bool testMaxHeapInsert() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn1, MAXHEAP, LEFTIST);
        const int numNodes = 300;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> priorities;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            priorities.push_back(priorityFn1(p));
        }
        return checkRemovalOrder(priorities, false);
    }
        
    // Test removal order for min-heap.
    bool testMinHeapRemoval() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn2, MINHEAP, LEFTIST);
        const int numNodes = 300;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> extracted;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            extracted.push_back(priorityFn2(p));
        }
        return checkRemovalOrder(extracted, true);
    }
        
    // Test removal order for max-heap.
    bool testMaxHeapRemoval() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn1, MAXHEAP, LEFTIST);
        const int numNodes = 300;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> extracted;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            extracted.push_back(priorityFn1(p));
        }
        return checkRemovalOrder(extracted, false);
    }
        
    // Test that leftist heap maintains correct NPL values (via removal order proxy).
    bool testLeftistNPLValues() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn2, MINHEAP, LEFTIST);
        const int numNodes = 300;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> priorities;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            priorities.push_back(priorityFn2(p));
        }
        return checkRemovalOrder(priorities, true);
    }
        
    // Test leftist heap property (NPL(left) >= NPL(right)) indirectly.
    bool testLeftistHeapProperty() {
        return testLeftistNPLValues();
    }
        
    // Test rebuilding the heap after changing the priority function.
    bool testPriorityChangeRebuild() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue(priorityFn2, MINHEAP, LEFTIST);  // initial min-heap using priorityFn2
        const int numNodes = 200;
        for (int i = 0; i < numNodes; i++) {
            if (!queue.insertPost(randomPost(randGen))) return false;
        }
        vector<int> original;
        SQueue copyQueue = queue;
        while (copyQueue.numPosts() > 0) {
            Post p = copyQueue.getNextPost();
            original.push_back(priorityFn2(p));
        }
        // Change to max-heap using priorityFn1 and rebuild.
        queue.setPriorityFn(priorityFn1, MAXHEAP);
        vector<int> rebuilt;
        while (queue.numPosts() > 0) {
            Post p = queue.getNextPost();
            rebuilt.push_back(priorityFn1(p));
        }
        return checkRemovalOrder(rebuilt, false);
    }
        
    // Test merging an empty queue with a normal queue.
    bool testEmptyMerge() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue normalQueue(priorityFn2, MINHEAP, LEFTIST);
        const int numNodes = 100;
        for (int i = 0; i < numNodes; i++) {
            normalQueue.insertPost(randomPost(randGen));
        }
        SQueue emptyQueue(priorityFn2, MINHEAP, LEFTIST);
        normalQueue.mergeWithQueue(emptyQueue);
        return (normalQueue.numPosts() == numNodes);
    }
        
    // Test copy constructor for normal case.
    bool testCopyConstructorNormal() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue origQueue(priorityFn1, MAXHEAP, SKEW);
        const int numNodes = 150;
        for (int i = 0; i < numNodes; i++) {
            origQueue.insertPost(randomPost(randGen));
        }
        SQueue copyQueue(origQueue);
        vector<int> origOrder, copyOrder;
        while (origQueue.numPosts() > 0) {
            Post p = origQueue.getNextPost();
            origOrder.push_back(priorityFn1(p));
        }
        while (copyQueue.numPosts() > 0) {
            Post p = copyQueue.getNextPost();
            copyOrder.push_back(priorityFn1(p));
        }
        return (origOrder == copyOrder);
    }
        
    // Test copy constructor for edge case (empty queue).
    bool testCopyConstructorEdge() {
        SQueue emptyQueue(priorityFn2, MINHEAP, SKEW);
        SQueue copyQueue(emptyQueue);
        return (copyQueue.numPosts() == 0);
    }
        
    // Test assignment operator for normal case.
    bool testAssignmentOperatorNormal() {
        Random randGen(MINPOSTID, MAXPOSTID);
        SQueue queue1(priorityFn1, MAXHEAP, SKEW);
        const int numNodes = 150;
        for (int i = 0; i < numNodes; i++) {
            queue1.insertPost(randomPost(randGen));
        }
        SQueue queue2(priorityFn1, MAXHEAP, SKEW);
        queue2 = queue1;
        vector<int> order1, order2;
        while (queue1.numPosts() > 0) {
            Post p = queue1.getNextPost();
            order1.push_back(priorityFn1(p));
        }
        while (queue2.numPosts() > 0) {
            Post p = queue2.getNextPost();
            order2.push_back(priorityFn1(p));
        }
        return (order1 == order2);
    }
        
        // Test assignment operator for edge case (assigning empty queue).
    bool testAssignmentOperatorEdge() {
        SQueue nonEmpty(priorityFn2, MINHEAP, LEFTIST);
        SQueue empty(priorityFn2, MINHEAP, LEFTIST);
        Random randGen(MINPOSTID, MAXPOSTID);
        for (int i = 0; i < 50; i++) {
            nonEmpty.insertPost(randomPost(randGen));
        }
        nonEmpty = empty;
        return (nonEmpty.numPosts() == 0);
    }
        
    // Test that attempting to dequeue an empty queue throws out_of_range.
    bool testDequeueEmpty() {
        try {
            SQueue queue(priorityFn1, MAXHEAP, SKEW);
            queue.getNextPost();
        } catch (const out_of_range&) {
            return true;
        } catch (...) {
            return false;
        }
        return false;
    }
        
    // Test that merging queues with different priority functions throws domain_error.
    bool testMergeDifferentPriorityFunctions() {
        SQueue queue1(priorityFn1, MAXHEAP, SKEW);
        SQueue queue2(priorityFn2, MINHEAP, SKEW);
        try {
            queue1.mergeWithQueue(queue2);
        } catch (const domain_error&) {
            return true;
        } catch (...) {
            return false;
        }
        return false;
    }
};
    
// ---------------------- Main Function ----------------------
int main() {
    Tester tester;
    int passed = 0;
    const int total = 14;
        
    cout << "Running testsx..." << endl;
        
    if (tester.testMinHeapInsert()) { cout << "testMinHeapInsert PASSED" << endl; ++passed; }
    else cout << "testMinHeapInsert FAILED" << endl;
        
    if (tester.testMaxHeapInsert()) { cout << "testMaxHeapInsert PASSED" << endl; ++passed; }
    else cout << "testMaxHeapInsert FAILED" << endl;
        
    if (tester.testMinHeapRemoval()) { cout << "testMinHeapRemoval PASSED" << endl; ++passed; }
    else cout << "testMinHeapRemoval FAILED" << endl;
        
    if (tester.testMaxHeapRemoval()) { cout << "testMaxHeapRemoval PASSED" << endl; ++passed; }
    else cout << "testMaxHeapRemoval FAILED" << endl;
        
    if (tester.testLeftistNPLValues()) { cout << "testLeftistNPLValues PASSED" << endl; ++passed; }
    else cout << "testLeftistNPLValues FAILED" << endl;
        
    if (tester.testLeftistHeapProperty()) { cout << "testLeftistHeapProperty PASSED" << endl; ++passed; }
    else cout << "testLeftistHeapProperty FAILED" << endl;
        
    if (tester.testPriorityChangeRebuild()) { cout << "testPriorityChangeRebuild PASSED" << endl; ++passed; }
    else cout << "testPriorityChangeRebuild FAILED" << endl;
        
    if (tester.testEmptyMerge()) { cout << "testEmptyMerge PASSED" << endl; ++passed; }
    else cout << "testEmptyMerge FAILED" << endl;
        
    if (tester.testCopyConstructorNormal()) { cout << "testCopyConstructorNormal PASSED" << endl; ++passed; }
    else cout << "testCopyConstructorNormal FAILED" << endl;
        
    if (tester.testCopyConstructorEdge()) { cout << "testCopyConstructorEdge PASSED" << endl; ++passed; }
    else cout << "testCopyConstructorEdge FAILED" << endl;
        
    if (tester.testAssignmentOperatorNormal()) { cout << "testAssignmentOperatorNormal PASSED" << endl; ++passed; }
    else cout << "testAssignmentOperatorNormal FAILED" << endl;
        
    if (tester.testAssignmentOperatorEdge()) { cout << "testAssignmentOperatorEdge PASSED" << endl; ++passed; }
    else cout << "testAssignmentOperatorEdge FAILED" << endl;
        
    if (tester.testDequeueEmpty()) { cout << "testDequeueEmpty PASSED" << endl; ++passed; }
    else cout << "testDequeueEmpty FAILED" << endl;
        
    if (tester.testMergeDifferentPriorityFunctions()) { cout << "testMergeDifferentPriorityFunctions PASSED" << endl; ++passed; }
    else cout << "testMergeDifferentPriorityFunctions FAILED" << endl;
        
    cout << "\nTests Passed: " << passed << " out of " << total << endl;
    return 0;
}

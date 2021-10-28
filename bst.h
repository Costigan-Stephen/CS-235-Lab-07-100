/***********************************************************************
 * Header:
 *    BST
 * Summary:
 *    Our custom implementation of a BST for set and for map
 *      __       ____       ____         __
 *     /  |    .'    '.   .'    '.   _  / /
 *     `| |   |  .--.  | |  .--.  | (_)/ /
 *      | |   | |    | | | |    | |   / / _
 *     _| |_  |  `--'  | |  `--'  |  / / (_)
 *    |_____|  '.____.'   '.____.'  /_/
 *
 *    This will contain the class definition of:
 *        BST                 : A class that represents a binary search tree
 *        BST::iterator       : An iterator through BST
 * Author
 *    Stephen Costigan, Alexander Dohms, Jonathan Colwell
 ************************************************************************/

#pragma once
 
#ifdef DEBUG
#define debug(x) x
#else // !DEBUG
#define debug(x)
#endif // !DEBUG

#include <cassert>
#include <utility>
#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <utility>    // for std::pair

namespace custom
{

/*****************************************************************
 * BINARY SEARCH TREE
 * Create a Binary Search Tree
 *****************************************************************/
template <typename T>
class BST
{
public:
   //
   // Construct - Finished | Alexander
   //

    BST() : root(nullptr), numElements(0) {}                                                                          //Default Constructor
    BST(const BST& rhs) : root(nullptr), numElements(0) { *this = rhs; }                                              //Copy constructor 
    BST(BST&& rhs) : root(rhs.root), numElements(rhs.numElements) { rhs.root = nullptr; rhs.numElements = 0; }   //Move Constructor
    BST(const std::initializer_list<T>& il) : root(nullptr), numElements(0) { *this = il; }                             //Initializer List Constructor
    ~BST() { clear(); }

   //
   // Assign
   //

   BST & operator = (const BST &  rhs); 
   BST & operator = (      BST && rhs);
   BST & operator = (const std::initializer_list<T>& il);
   void swap(BST & rhs);

   //
   // Iterator
   //

   class iterator;
   iterator   begin() const noexcept;
   iterator   end()   const noexcept { return iterator(nullptr); }

   //
   // Access
   //

   iterator find(const T& t);


   // 
   // Insert
   //

   std::pair<iterator, bool> insert(const T&  t, bool keepUnique = false);
   std::pair<iterator, bool> insert(      T&& t, bool keepUnique = false);

   //
   // Remove
   // 

   iterator erase(iterator& it);
   void   clear() noexcept;
   
   

   // 
   // Status
   //

   bool   empty() const noexcept { return numElements == 0; }
   size_t size()  const noexcept { return numElements;   }
   

#ifdef DEBUG // make this visible to the unit tests
public:
#else
private:
#endif

   class BNode;
   BNode * root;              // root node of the binary search tree
   void removeNode(BNode* pNode);
   size_t numElements;        // number of elements currently in the tree
};


/*****************************************************************
 * BINARY NODE
 * A single node in a binary tree. Note that the node does not know
 * anything about the properties of the tree so no validation can be done.
 *****************************************************************/
template <typename T>
class BST <T> :: BNode
{
public:
   // 
   // Construct
   //
    BNode() : pLeft(nullptr), pRight(nullptr), pParent(nullptr), data(T()) {}				// Default Constructor
    BNode(const T& t) : pParent(nullptr), pLeft(nullptr), pRight(nullptr), data(t) {}		// Copy Constructor
    BNode(T&& t) : pLeft(nullptr), pRight(nullptr), data(std::move(t)) {}					// Move Constructor

   //
   // Insert
   //
   void addLeft (BNode * pNode);
   void addRight(BNode * pNode);
   void addLeft (const T &  t);
   void addRight(const T &  t);
   void addLeft (      T && t);
   void addRight(      T && t);

   void assign(BNode* pDest, const BNode* pSrc);
   void clear(BNode* pThis);

   // 
   // Status
   //
   bool isRightChild(BNode* pNode) const { return pNode->data > pNode->pParent->data; } // DON'T KNOW IF IT CHANGES ANYTHING
   bool isLeftChild (BNode* pNode) const { return pNode->data < pNode->pParent->data; } // <-/

   //
   // Data
   //
   T data;                  // Actual data stored in the BNode
   BNode* pLeft;            // Left child - smaller
   BNode* pRight;           // Right child - larger
   BNode* pParent;          // Parent
   bool isRed;              // Red-black balancing stuff
};

/**********************************************************
 * BINARY SEARCH TREE ITERATOR
 * Forward and reverse iterator through a BST
 *********************************************************/
template <typename T>
class BST <T> :: iterator
{
public:
   // constructors and assignment
   iterator(BNode* p = nullptr): pNode(p) { }
   iterator(const iterator& rhs) { pNode = rhs.pNode; }
   iterator & operator = (const iterator & rhs)
   {
        if (&rhs != nullptr)
            pNode = rhs.pNode;
        return *this;
   }

   // compare
   bool operator != (const iterator& rhs) const { return rhs.pNode != this->pNode ; }
   bool operator == (const iterator& rhs) const { return rhs.pNode == this->pNode ; }

   // de-reference. Cannot change because it will invalidate the BST
   const T & operator * () const 
   {
       return pNode->data;
      /*return *(new T);*/ 
   }

   // increment and decrement
   iterator & operator ++ ();
   iterator   operator ++ (int postfix)
   {
       iterator it = *this;
       ++(*this);
       return it;
   }
   iterator & operator -- ();
   iterator   operator -- (int postfix)
   {
       iterator it = *this;
       --(*this); 
       return it;
   }

   // must give friend status to remove so it can call getNode() from it
   friend BST <T> :: iterator BST <T> :: erase(iterator & it);

#ifdef DEBUG // make this visible to the unit tests
public:
#else
private:
#endif
   
    // the node
    BNode * pNode;
};


/*********************************************
 *********************************************
 *******************       *******************
 *******************  BST  *******************
 *******************       *******************
 *********************************************
 *********************************************/

/*********************************************
 * BST :: ASSIGNMENT OPERATOR
 * Copy one tree to another
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (const BST <T> & rhs)
{
    root->assign(root, rhs.root);
    numElements = rhs.numElements;
    return *this;
}

/*********************************************
 * BST :: ASSIGNMENT OPERATOR with INITIALIZATION LIST
 * Copy nodes onto a BTree
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (const std::initializer_list<T>& il)
{
    clear();
    for (T t : il) {
        insert(t); // WILL PROB INCREASE % WHEN INSERT IS DONE
    }
    return *this;
}


/*********************************************
 * BST :: ASSIGN-MOVE OPERATOR
 * Move one tree to another
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (BST <T> && rhs)
{
    clear();
    swap(rhs);
    return *this;
}

/*********************************************
 * BST :: SWAP
 * Swap two trees
 ********************************************/
template <typename T>
void BST <T> :: swap (BST <T>& rhs)
{
    auto tempRoot = rhs.root;
    rhs.root = root;
    root = tempRoot;

    auto tempElements = rhs.numElements;
    rhs.numElements = numElements;
    numElements = tempElements;
}

/*****************************************************
 * BST :: INSERT
 * Insert a node at a given location in the tree
 ****************************************************/
template <typename T>
std::pair<typename BST <T> :: iterator, bool> BST <T> :: insert(const T & t, bool keepUnique)
{
   std::pair<iterator, bool> pairReturn(end(), false);
   return pairReturn;
}

template <typename T>
std::pair<typename BST <T> ::iterator, bool> BST <T> ::insert(T && t, bool keepUnique)
{
   std::pair<iterator, bool> pairReturn(end(), false);
   return pairReturn;
}

/*************************************************
 * BST :: ERASE
 * Remove a given node as specified by the iterator
 ************************************************/
template <typename T>
typename BST <T> ::iterator BST <T> :: erase(iterator & it)
{  
    /*// node is empty
    if (it.pNode == nullptr)
    {
        removeNode(it.pNode);
        return end();
    }

    // node has no children
    if (it.pNode->pRight == nullptr && it.pNode->pLeft == nullptr)
    {
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pRight == it.pNode)
            it.pNode->pParent->pRight = nullptr;
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pLeft == it.pNode)
            it.pNode->pParent->pLeft = nullptr;
        removeNode(it.pNode);
        return end();
    }
    //one child
    if (it.pNode->pRight == nullptr && it.pNode->pLeft != nullptr) // right exists, left does not
    {
        it.pNode->pLeft->pParent = it.pNode->pParent;
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pRight == it.pNode)
            it.pNode->pParent->pRight = it.pNode->pLeft;
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pLeft == it.pNode)
            it.pNode->pParent->pLeft = it.pNode->pLeft;
        removeNode(it.pNode);
        return end();
    }
    if (it.pNode->pLeft == nullptr && it.pNode->pRight != nullptr) // left exists, right does not
    {
        it.pNode->pRight->pParent = it.pNode->pParent;
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pRight == it.pNode)
            it.pNode->pParent->pRight = it.pNode->pRight;
        if (it.pNode->pParent != nullptr && it.pNode->pParent->pLeft == it.pNode)
            it.pNode->pParent->pLeft = it.pNode->pRight;
        removeNode(it.pNode);
        return end();
    }
    else if (it.pNode->pLeft != nullptr && it.pNode->pRight != nullptr) // node has two children
    {
        BNode* newNode = it.pNode;
        if (newNode->pRight != nullptr)
        {
            newNode = newNode->pRight;
            while (newNode->pLeft != nullptr)
            {
                newNode = newNode->pLeft;
            }
            it.pNode->data = newNode->data;
            newNode->pParent->pLeft = nullptr;
            if (newNode->pRight != nullptr)
            {
                newNode->pRight->pParent = newNode->pParent;
                newNode->pParent->pLeft = newNode->pRight;
                newNode->pRight = nullptr;
            }
            delete newNode;
            newNode = nullptr;
        }
    }*/ 
    return end();
}

/*****************************************************
 * BST :: CLEAR
 * Removes all the BNodes from a tree
 ****************************************************/
template <typename T>
void BST <T> ::clear() noexcept
{
   
   //Example from pList
   // loop is unnecessary for the checks, but probably good practice
//   while (pParent != NULL)
//   {
//      BNode* pDelete = pParent;
//       pParent = pHead->pNext;
//       delete pDelete;
//   }
//   pTail = nullptr;
 
   root = nullptr;
   numElements = 0;
}

/****************************************************
 * BST :: REMOVENODE (Created by Steve)
 * Remove the specified node
 ****************************************************/
template<typename T>
inline void BST<T>::removeNode(BNode* pNode)
{
    if (pNode == nullptr)
        delete pNode;

    BNode * pNext = (pNode->isRightChild(pNode) ? pNode->pRight : pNode->pLeft);
    
    if (pNode != this->root)
    {
        if (pNode->pParent->pLeft == pNode)
        {
            pNode->pParent->pLeft = pNext;
        }
        else
        {
            pNode->pParent->pRight = pNext;
        }
    }
    else
    {
        this->root = pNext;
        pNext->pParent = nullptr;
    }
    delete pNode;
}


/*****************************************************
 * BST :: BEGIN
 * Return the first node (left-most) in a binary search tree
 ****************************************************/
template <typename T>
typename BST <T> :: iterator custom :: BST <T> :: begin() const noexcept
{
    if (root == nullptr)
        return nullptr;

    BNode* leftdata = root;
    while (leftdata->pLeft)
        leftdata = leftdata->pLeft;

    return iterator(leftdata);
} 
 

/****************************************************
 * BST :: FIND
 * Return the node corresponding to a given value
 ****************************************************/
template <typename T>
typename BST <T> :: iterator BST<T> :: find(const T & t)
{
    BNode* p = root;
    iterator it = iterator(p);
    while (p != nullptr)
    {
        it++;
        if (p->data == t)
        {
            return iterator(p);
        }
        else if (p->data < t)
        {
            p = p->pRight;
        }
        else
        {
            p = p->pLeft;
        }  
    }
}


/******************************************************
 ******************************************************
 **********************          **********************
 **********************  B NODE  **********************
 **********************          **********************
 ******************************************************
 ******************************************************/

 
/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addLeft (BNode * pNode)
{
    // does not increase % 
    if (pNode)
        pParent = pNode->pParent;
    pLeft = pNode->pLeft;
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addRight (BNode * pNode)
{
    // does not increase % 
    if (pNode)
        pParent = pNode->pParent;
    pRight = pNode->pRight;
}

/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST<T> :: BNode :: addLeft (const T & t)
{
    // does not increase %
    if (t)
        this.pLeft->data = new BNode(t);
   
}

/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST<T> ::BNode::addLeft(T && t)
{
    // does not increase %
    if (t)
        this.pLeft->data = new BNode(t);
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addRight (const T & t)
{
    // does not increase %
    if (t)
        this.pRight->data = new BNode(t);
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> ::BNode::addRight(T && t)
{
    // does not increase %
    if (t)
        this.pRight->data = new BNode(t);
}



/*************************************************
 *************************************************
 *****************            ********************
 *****************  ITERATOR  ********************
 *****************            ********************
 *************************************************
 *************************************************/     

/**************************************************
 * BST ITERATOR :: INCREMENT PREFIX
 * advance by one
 *************************************************/
template <typename T>
typename BST <T> :: iterator & BST <T> :: iterator :: operator ++ () 
{
    if (pNode == nullptr)
        return *this;

    if (pNode->pRight != nullptr)
    {
        pNode = pNode->pRight;
        while (pNode->pLeft)
            pNode = pNode->pLeft;
        return *this;
    }

    BNode* pAdd = pNode;
    pNode = pNode->pParent; 

    if (pNode == nullptr)
        return *this;

    if (pAdd == pNode->pLeft)
        return *this;

    while (pNode != nullptr && pAdd == pNode->pRight)
    {
        pAdd = pNode;
        pNode = pNode->pParent;
    }

    return *this;
}

/**************************************************
 * BST ITERATOR :: DECREMENT PREFIX
 * advance by one
 *************************************************/
template <typename T>
typename BST <T> :: iterator & BST <T> :: iterator :: operator -- ()
{
    if (pNode == nullptr)
        return *this;

    if (pNode->pLeft != nullptr)
    {
        pNode = pNode->pLeft;
        while (pNode->pRight)
            pNode = pNode->pRight;
        return *this;
    }

    BNode* pAdd = pNode;
    pNode = pNode->pParent;

    if (pNode == nullptr)
        return *this;

    if (pAdd == pNode->pRight)
        return *this;

    while (pNode != nullptr && pAdd == pNode->pLeft)
    {
        pAdd = pNode;
        pNode = pNode->pParent;
    }

    return *this;

}

/**********************************************
 * assign
 * copy the values from pSrc onto pDest preserving
 * as many of the nodes as possible.
 *********************************************/
template <typename T>
void BST<T> ::BNode::assign(BST<T> ::BNode* pDest, const BST<T> ::BNode* pSrc)
{

    // Source is Empty
    if (!pSrc) {
        clear(pDest);
        return;
    }

    // Neither the Source nor Destination are Empty
    if (pDest && pSrc) {
        pDest->data = pSrc->data;
        assign(pDest->pRight, pSrc->pRight);
        assign(pDest->pLeft, pSrc->pLeft);
    }

    // Destination is Empty
    if (!pDest && pSrc) {
        pDest = new BST::BNode(pSrc->data);
        assign(pDest->pRight, pSrc->pRight);
        assign(pDest->pLeft, pSrc->pLeft);
    }

    // Setting parent values
    if (pDest->pRight)
        pDest->pRight->pParent = pDest;
    if (pDest->pLeft)
        pDest->pLeft->pParent = pDest;
}

/*****************************************************
 * DELETE BINARY TREE
 * Delete all the nodes below pThis including pThis
 * using postfix traverse: LRV
 ****************************************************/
template <class T>
void BST<T>::BNode::clear(BNode* pThis)
{
    if (!pThis)
        return;

    clear(pThis->pLeft);
    clear(pThis->pRight);
    pThis = NULL;
}

} // namespace custom



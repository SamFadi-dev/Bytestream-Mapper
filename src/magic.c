/// @authors EL MASRI Sam & SICIM Merve
// Code based on the red-black tree implementation from INFO0027-2 course

#include <stdlib.h>
#include <assert.h>
#include "magic.h"

// Constants for red-black tree
#define RED 1
#define BLACK 0

/// @brief Opaque type for the MAGIC ADT node
typedef struct Node
{
    int pos;           // position in input stream
    int delta;         // +len for add, -len for remove
    int totalDelta;    // sum of deltas in subtree
    struct Node *left, *right, *parent;
    int color;         // RED or BLACK
} Node;

struct magic
{
    Node *root;
    Node *NIL;
};

//==============================================================================
//============================== MAGIC PRIVATE API =============================
//==============================================================================

/// @brief Create a new node with the given parameters of a MAGIC instance
/// @param m MAGIC instance
/// @param pos Starting position
/// @param delta Delta value
/// @return New node
static Node* createNode(MAGIC m, int pos, int delta)
{
    assert(m);

    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;
    node->pos = pos;
    node->delta = delta;
    node->totalDelta = delta;
    node->left = node->right = node->parent = m->NIL;
    node->color = RED;
    return node;
}

/// @brief Update the totalDelta field of a node
/// @param node 
static void updateTotalDelta(Node *node)
{
    if (node)
        node->totalDelta = node->delta + node->left->totalDelta + node->right->totalDelta;
}

/// @brief Rotate the tree to the left like in a red-black tree
/// @param m MAGIC instance
/// @param x Node to rotate
static void rotateLeft(MAGIC m, Node *x)
{
    assert(m && x);

    Node *y = x->right;
    x->right = y->left;
    if (y->left != m->NIL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == m->NIL)
        m->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;

    updateTotalDelta(x);
    updateTotalDelta(y);
}

/// @brief Rotate the tree to the right like in a red-black tree
/// @param m MAGIC instance
/// @param y Node to rotate
static void rotateRight(MAGIC m, Node *y) 
{
    assert(m && y);

    Node *x = y->left;
    y->left = x->right;
    if (x->right != m->NIL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == m->NIL)
        m->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;

    updateTotalDelta(y);
    updateTotalDelta(x);
}

/// @brief Fix the tree after an insertion like in a red-black tree
/// @param m MAGIC instance
/// @param z Node to fix
static void fixInsert(MAGIC m, Node *z) 
{
    assert(m && z);

    while (z->parent->color == RED) 
    {
        if (z->parent == z->parent->parent->left) 
        {
            Node *y = z->parent->parent->right;
            if (y->color == RED) 
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } 
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rotateLeft(m, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(m, z->parent->parent);
            }
        } 
        else 
        {
            Node *y = z->parent->parent->left;
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } 
            else 
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rotateRight(m, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateLeft(m, z->parent->parent);
            }
        }
    }
    m->root->color = BLACK;
}

/// @brief Insert a delta into the tree
/// @param m MAGIC instance
/// @param pos Starting position
/// @param delta Delta value
static void insertDelta(MAGIC m, int pos, int delta)
{
    assert(m);

    Node *z = createNode(m, pos, delta);
    if (!z) return;
    Node *y = m->NIL;
    Node *x = m->root;

    while (x != m->NIL)
    {
        y = x;
        if (z->pos < x->pos)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == m->NIL)
        m->root = z;
    else if (z->pos < y->pos)
        y->left = z;
    else
        y->right = z;

    fixInsert(m, z);

    // Update all totalDeltas up to the root
    while (z != m->NIL)
    {
        updateTotalDelta(z);
        z = z->parent;
    }
}

/// @brief Get the cumulative delta up to a given position
/// @param node Node to start from
/// @param NIL NIL node
/// @param pos Position to reach
/// @return Cumulative delta
static int getCumulativeDelta(Node *node, Node *NIL, int pos)
{
    assert(node && NIL);

    if (node == NIL) return 0;
    if (pos <= node->pos)
        return getCumulativeDelta(node->left, NIL, pos);
    else
        return node->delta + node->left->totalDelta + getCumulativeDelta(node->right, NIL, pos);
}

/// @brief Destroy the tree recursively
/// @param node Node to destroy
/// @param NIL NIL node
static void destroyTree(Node *node, Node *NIL)
{
    if (node == NIL) return;
    destroyTree(node->left, NIL);
    destroyTree(node->right, NIL);
    free(node);
}

//==============================================================================
//==============================MAGIC PUBLIC API================================
//==============================================================================

MAGIC MAGICinit()
{
    MAGIC m = malloc(sizeof(struct magic));
    if(!m) return NULL;
    m->NIL = malloc(sizeof(Node));
    if(!m->NIL) return NULL;
    m->NIL->color = BLACK;
    m->NIL->totalDelta = 0;
    m->root = m->NIL;
    return m;
}

void MAGICadd(MAGIC m, int pos, int length)
{
    assert(m);

    insertDelta(m, pos, length);
}

void MAGICremove(MAGIC m, int pos, int length)
{
    assert(m);

    insertDelta(m, pos, -length);
}

int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos) 
{
    assert(m);

    if (direction == STREAM_IN_OUT) 
    {
        return pos + getCumulativeDelta(m->root, m->NIL, pos);
    } 
    else 
    {
        // OUT to IN mapping: dynamic binary search
        int low = 0;
        int high = 1;
        while (1)
        {
            int mapped = high + getCumulativeDelta(m->root, m->NIL, high);
            if (mapped >= pos) break;
            high *= 2; // exponential backoff
            if (high > 1e9) return -1; // safety cap
        }

        while (low <= high)
        {
            int mid = (low + high) / 2;
            int mapped = mid + getCumulativeDelta(m->root, m->NIL, mid);
            if (mapped == pos) return mid;
            else if (mapped < pos) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }
}

void MAGICdestroy(MAGIC m)
{
    assert(m);

    destroyTree(m->root, m->NIL);
    free(m->NIL);
    free(m);
}

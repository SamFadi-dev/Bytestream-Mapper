/// @authors EL MASRI Sam & SICIM Merve
// Code based on the implementation of the INFO0027-2 red-black tree

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include "magic.h"

// Constants for red-black tree
#define RED 1
#define BLACK 0

/// @brief Type for the MAGIC ADT node
typedef struct Node 
{
    int pos;                              // position in input stream
    int delta;                           // +len for add, -len for remove
    int totalDelta;                     // cumulative delta
    struct Node *left, *right, *parent;// child nodes and parent
    int color;                        // RED or BLACK (for red-black tree)
} Node;

struct magic 
{
    Node *root;         // root of the red-black tree
    Node *NIL;          // sentinel node
    int max_input_pos;  // max position in input stream
    int *outMapping;    // output -> input cache
    int outMappingSize; // cache size
    int *inMapping;     // input -> output cache
    int inMappingSize;  // input -> output cache size
    int cacheValid;     // if 1, cache is valid
    int cacheDirtyFrom; // First position to update in cache
};

//=============================================================================
//========================== STATIC FUNCTIONS =================================
//=============================================================================

/// @brief Create a new node for the red-black tree.
/// @param m Pointer to the MAGIC instance
/// @param pos Position in the input stream
/// @param delta Delta value (+len for add, -len for remove)
/// @return Pointer to the new node
static Node* createNode(MAGIC m, int pos, int delta);
/// @brief Update the total delta value of a node and its ancestors.
/// @param node Pointer to the node to update
static void updateTotalDelta(Node *node);
/// @brief Rotate the tree to the left around a node.
/// @param m Pointer to the MAGIC instance
/// @param x Pointer to the node to rotate
static void rotateLeft(MAGIC m, Node *x);
/// @brief Rotate the tree to the right around a node.
/// @param m Pointer to the MAGIC instance
/// @param y Pointer to the node to rotate
static void rotateRight(MAGIC m, Node *y);
/// @brief Fix the red-black tree after insertion.
/// @param m Pointer to the MAGIC instance
/// @param z Pointer to the newly inserted node
static void fixInsert(MAGIC m, Node *z);
/// @brief Insert a delta into the red-black tree.
/// @param m Pointer to the MAGIC instance
/// @param pos Position in the input stream
/// @param delta Delta value (+len for add, -len for remove)
static void insertDelta(MAGIC m, int pos, int delta);
/// @brief Get the cumulative delta value up to a given position.
/// @param node Pointer to the current node
/// @param NIL Pointer to the NIL node
/// @param pos Position to check
/// @return Cumulative delta value
static int getCumulativeDelta(Node *node, Node *NIL, int pos);
/// @brief Destroy the red-black tree.
/// @param node Pointer to the current node
/// @param NIL Pointer to the NIL node
static void destroyTree(Node *node, Node *NIL);
/// @brief Check if a position is actually removed.
/// @param node Pointer to the current node
/// @param NIL Pointer to the NIL node
/// @param pos Position to check
/// @return 1 if removed, 0 otherwise
static int IsRemoved(Node *node, Node *NIL, int pos);
/// @brief Mark removed positions in the cache.
/// @param node Pointer to the current node
/// @param NIL Pointer to the NIL node
/// @param removed Pointer to the removed positions array
/// @param maxInput Maximum input position
static void markRemovedPositions(Node *node, Node *NIL, bool *removed, int maxInput);
/// @brief Update the local cache
/// @param m Pointer to the MAGIC instance
static void updateCacheLocal(MAGIC m);
/// @brief Invalidate the cache to force a recalculation.
/// @param m Pointer to the MAGIC instance
static void invalidateCache(MAGIC m);

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

static void updateTotalDelta(Node *node) 
{
    if (node) 
    {
        node->totalDelta = node->delta;
        if (node->left) node->totalDelta += node->left->totalDelta;
        if (node->right) node->totalDelta += node->right->totalDelta;
    }
}

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

    // update totalDelta for x and y
    updateTotalDelta(x);
    updateTotalDelta(y);
}

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

    // update totalDelta for y and x
    updateTotalDelta(y);
    updateTotalDelta(x);
}

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

static void insertDelta(MAGIC m, int pos, int delta) 
{
    assert(m && pos >= 0 && delta != 0);

    // Check if the position is valid
    if (pos > m->max_input_pos)
        m->max_input_pos = pos;
    Node *x = m->root, *y = m->NIL;

    // Find the position to insert
    while (x != m->NIL) 
    {
        y = x;
        if (pos < x->pos)
            x = x->left;
        else if (pos > x->pos)
            x = x->right;
        else 
        {
            x->delta += delta;
            while (x != m->NIL) 
            {
                updateTotalDelta(x);
                x = x->parent;
            }
            return;
        }
    }
    Node *z = createNode(m, pos, delta);
    z->parent = y;
    if (y == m->NIL)
        m->root = z;
    else if (pos < y->pos)
        y->left = z;
    else
        y->right = z;
    updateTotalDelta(z);
    fixInsert(m, z);

    // Update totalDelta for all ancestors
    while (z != m->NIL) 
    {
        updateTotalDelta(z);
        z = z->parent;
    }
}

static int getCumulativeDelta(Node *node, Node *NIL, int pos) 
{
    int sum = 0;
    while (node != NIL) 
    {
        if (pos < node->pos)
            node = node->left;
        else 
        {
            sum += node->delta + node->left->totalDelta;
            node = node->right;
        }
    }
    return sum;
}

static void destroyTree(Node *node, Node *NIL) 
{
    if (node == NIL) return;
    destroyTree(node->left, NIL);
    destroyTree(node->right, NIL);
    free(node);
}

static void invalidateCache(MAGIC m) 
{
    m->cacheValid = 0;
    m->cacheDirtyFrom = 0;
}

static int IsRemoved(Node *node, Node *NIL, int pos) 
{
    if (node == NIL)
        return 0;
        
    // Check if the current node is a removal
    if (node->delta < 0) {
        int start = node->pos;
        int end = start - node->delta;  // La plage de suppression est [start, end)
        if (pos >= start && pos < end)
            return 1;
    }
    
    // Check in the left subtree
    // If the position is less than the current node's position, check the left subtree
    if (pos < node->pos)
        return IsRemoved(node->left, NIL, pos);
    
    // Check in the right subtree
    return IsRemoved(node->right, NIL, pos);
}

static void markRemovedPositions(Node *node, Node *NIL, bool *removed, int maxInput) 
{
    if (node == NIL)
        return;
    markRemovedPositions(node->left, NIL, removed, maxInput);
    if (node->delta < 0) 
    {
        // Mark the range of removed positions
        // The range should be [start, end)
        int start = node->pos;
        int end = start - node->delta;
        if (end > maxInput + 1)
            end = maxInput + 1;

        // Check if the range is valid
        for (int i = start; i < end; i++) 
        {
            removed[i] = true;
        }
    }

    markRemovedPositions(node->right, NIL, removed, maxInput);
}

static void updateCacheLocal(MAGIC m) 
{
    int maxInput = m->max_input_pos;
    // Force the cache to be updated
    m->cacheDirtyFrom = 0;
    
    // Check if the cache is valid
    if (m->inMappingSize < maxInput + 1) 
    {
        m->inMapping = realloc(m->inMapping, (maxInput + 1) * sizeof(int));
        if (!m->inMapping) 
        {
            perror("Realloc inMapping");
            exit(EXIT_FAILURE);
        }
        m->inMappingSize = maxInput + 1;
    }

    // Check if the outMapping is valid
    bool *removed = calloc(maxInput + 1, sizeof(bool));
    if (!removed) {
        perror("Allocation error in updateCacheLocal");
        exit(EXIT_FAILURE);
    }
    markRemovedPositions(m->root, m->NIL, removed, maxInput);

    // Loop through the removed positions and update the cache
    for (int i = m->cacheDirtyFrom; i <= maxInput; i++) 
    {
        int cumulative = getCumulativeDelta(m->root, m->NIL, i);
        int newMapping = removed[i] ? -1 : i + cumulative;
        m->inMapping[i] = newMapping;
        if (newMapping == -1)
            continue;
        if (newMapping >= m->outMappingSize) 
        {
            int newSize = newMapping + 1;
            m->outMapping = realloc(m->outMapping, newSize * sizeof(int));
            if (!m->outMapping) 
            {
                perror("Realloc outMapping");
                exit(EXIT_FAILURE);
            }
            for (int j = m->outMappingSize; j < newSize; j++)
                m->outMapping[j] = -1;
            m->outMappingSize = newSize;
        }
        m->outMapping[newMapping] = i;
    }
    free(removed);
    m->cacheDirtyFrom = maxInput + 1;
    m->cacheValid = 1;
}

//=============================================================================
//============================== MAGIC API ====================================
//=============================================================================

MAGIC MAGICinit() 
{
    MAGIC m = malloc(sizeof(struct magic));
    m->NIL = malloc(sizeof(Node));
    m->NIL->color = BLACK;
    m->NIL->totalDelta = 0;
    m->NIL->left = m->NIL->right = m->NIL->parent = NULL;
    m->root = m->NIL;
    m->max_input_pos = 0;
    m->outMapping = malloc(1 * sizeof(int));
    m->outMapping[0] = -1;
    m->outMappingSize = 1;
    m->inMapping = malloc(1 * sizeof(int));
    m->inMapping[0] = -1;
    m->inMappingSize = 1;
    m->cacheValid = 0;
    m->cacheDirtyFrom = 0;
    return m;
}

void MAGICadd(MAGIC m, int pos, int length) 
{
    assert(m && length > 0);

    // Get the current input position
    int input_pos;
    if (m->max_input_pos == 0 && m->root == m->NIL)
        input_pos = -1;
    else
        input_pos = MAGICmap(m, STREAM_OUT_IN, pos);
    
    // Insert the delta into the red-black tree at the correct position
    if (input_pos != -1) 
    {
        insertDelta(m, input_pos, length);
        if (input_pos < m->cacheDirtyFrom)
            m->cacheDirtyFrom = input_pos;
    } 
    else 
    {
        int max_out = m->max_input_pos + getCumulativeDelta(m->root, m->NIL, m->max_input_pos);
        // Check if the position is greater than the maximum output position
        // If so, we need to insert the delta at the input position
        if (pos > max_out) 
        {
            input_pos = m->max_input_pos + (pos - max_out);
            insertDelta(m, input_pos, length);
            if (input_pos < m->cacheDirtyFrom)
                m->cacheDirtyFrom = input_pos;
        } 
        else 
        {
            // If the position is less than the maximum output position,
            // we need to find the correct position to insert the delta
            // in the red-black tree
            for (int i = pos - 1; i >= 0; --i) 
            {
                int candidate = MAGICmap(m, STREAM_OUT_IN, i);
                if (candidate != -1) 
                {
                    insertDelta(m, candidate + 1, length);
                    if ((candidate + 1) < m->cacheDirtyFrom)
                        m->cacheDirtyFrom = candidate + 1;
                    invalidateCache(m);
                }
            }
            insertDelta(m, 0, length);
            m->cacheDirtyFrom = 0;
        }
    }

    // Update the cache
    invalidateCache(m);
}


void MAGICremove(MAGIC m, int pos, int length) 
{
    assert(m && length > 0);

    // Always remove from the updated input stream
    int input_pos = MAGICmap(m, STREAM_OUT_IN, pos);
    if (input_pos == -1) 
    {
        // Update the cache if it's not valid
        if (!m->cacheValid) updateCacheLocal(m);
        // If the cache is valid, we need to find the correct position
        int low = pos, high = m->outMappingSize - 1;
        int candidate_out = -1;

        // Binary search to find the first position in the outMapping
        // that is not -1 and is greater than the current position
        while (low <= high) 
        {
            int mid = (low + high) / 2;
            if (m->outMapping[mid] != -1) 
            {
                candidate_out = mid;
                high = mid - 1;
            } 
            else 
            {
                low = mid + 1;
            }
        }
        if (candidate_out != -1 && pos < candidate_out)
            input_pos = m->outMapping[candidate_out];
    }
    // If the input position is still -1, we need to find the first
    // position in the outMapping that is not -1 and is greater than the current position
    if (input_pos != -1) 
    {
        insertDelta(m, input_pos, -length);
        if (input_pos < m->cacheDirtyFrom)
            m->cacheDirtyFrom = input_pos;
    }
    // Otherwise, we need to find the first position in the outMapping
    // that is not -1 and is greater than the current position 
    else 
    {
        for (int i = pos + 1; i <= m->max_input_pos; ++i) 
        {
            int candidate = MAGICmap(m, STREAM_OUT_IN, i);
            if (candidate != -1) 
            {
                insertDelta(m, candidate, -length);
                if (candidate < m->cacheDirtyFrom)
                    m->cacheDirtyFrom = candidate;
                break;
            }
        }
    }
    invalidateCache(m);
}


int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos) 
{
    assert(m && pos >= 0);

    // direction == STREAM_IN_OUT
    if (direction == STREAM_IN_OUT) 
    {
        // Update the cache if it's not valid
        if (!m->cacheValid) updateCacheLocal(m);
        
        // Work with the input mapping cache
        if (pos < m->inMappingSize) return m->inMapping[pos];
        else 
        {
            if (IsRemoved(m->root, m->NIL, pos))
                return -1;
            return pos + getCumulativeDelta(m->root, m->NIL, pos);
        }
    } 
    // direction == STREAM_OUT_IN
    else 
    { 
        if (!m->cacheValid) updateCacheLocal(m);

        // Work with the output mapping cache
        if (pos < m->outMappingSize) return m->outMapping[pos];
        else 
        {
            int low = 0, high = pos + 100;
            int candidate = -1;
            // Binary search to find the first position in the outMapping
            while (low <= high) 
            {
                int mid = (low + high) / 2;
                int mapped = mid + getCumulativeDelta(m->root, m->NIL, mid);
                if (mapped == pos) 
                {
                    candidate = mid;
                    break;
                } 
                else if (mapped < pos) 
                {
                    low = mid + 1;
                } 
                else 
                {
                    high = mid - 1;
                }
            }
            if (candidate != -1 && !IsRemoved(m->root, m->NIL, candidate))
                return candidate;
            else
                return -1;
        }
    }
}

void MAGICdestroy(MAGIC m) 
{
    destroyTree(m->root, m->NIL);
    free(m->NIL);
    free(m->outMapping);
    free(m->inMapping);
    free(m);
}
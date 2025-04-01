/// @authors EL MASRI Sam & SICIM Merve
// Code based on the red-black tree implementation from the INFO0027-2 course

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
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
    int max_input_pos;
    // --- Ajout du cache pour la correspondance OUT->IN ---
    int *cacheMapping; // tableau: pour chaque position de sortie, la position d'entrée associée (ou -1 si invalide)
    int cacheSize;     // taille du tableau cache (max_out + 1)
    int cacheValid;    // indicateur de validité (1 = cache à jour, 0 = invalide)
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
    {
        node->totalDelta = node->delta;
        if (node->left) node->totalDelta += node->left->totalDelta;
        if (node->right) node->totalDelta += node->right->totalDelta;
    }
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
    if (pos > m->max_input_pos) 
    {
        m->max_input_pos = pos;
    }
    Node *x = m->root, *y = m->NIL;
    while (x != m->NIL)
    {
        y = x;
        if (pos < x->pos) x = x->left;
        else if (pos > x->pos) x = x->right;
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
    if (y == m->NIL) m->root = z;
    else if (pos < y->pos) y->left = z;
    else y->right = z;
    updateTotalDelta(z);
    fixInsert(m, z);
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

static int isActuallyRemoved(Node *node, Node *NIL, int pos) {
    if (node == NIL)
        return 0;
    // Vérifie le nœud courant s'il correspond à une suppression couvrant pos.
    if (node->delta < 0) {
        int start = node->pos;
        int end = start - node->delta;
        if (pos >= start && pos < end)
            return 1;
    }
    // Recherche dans les deux sous-arbres.
    return isActuallyRemoved(node->left, NIL, pos) || isActuallyRemoved(node->right, NIL, pos);
}


static void updateCache(MAGIC m) {
    // Détermine la position de sortie maximum
    int maxOut = 0;
    for (int i = 0; i <= m->max_input_pos; i++) {
        if (!isActuallyRemoved(m->root, m->NIL, i)) {
            int out = i + getCumulativeDelta(m->root, m->NIL, i);
            if (out > maxOut) maxOut = out;
        }
    }
    // Alloue ou réalloue le cache
    if (m->cacheMapping) {
        free(m->cacheMapping);
    }
    m->cacheSize = maxOut + 1;
    m->cacheMapping = malloc(sizeof(int) * m->cacheSize);
    if (!m->cacheMapping) {
        perror("malloc cacheMapping");
        exit(EXIT_FAILURE);
    }
    // Initialise à -1 (position d'entrée invalide)
    for (int i = 0; i < m->cacheSize; i++) {
        m->cacheMapping[i] = -1;
    }
    // Remplit le cache pour chaque position d'entrée valide
    for (int i = 0; i <= m->max_input_pos; i++) {
        if (!isActuallyRemoved(m->root, m->NIL, i)) {
            int out = i + getCumulativeDelta(m->root, m->NIL, i);
            if (out < m->cacheSize)
                m->cacheMapping[out] = i;
        }
    }
    m->cacheValid = 1;
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
    m->NIL = malloc(sizeof(Node));
    m->NIL->color = BLACK;
    m->NIL->totalDelta = 0;
    m->NIL->left = m->NIL->right = m->NIL->parent = NULL;
    m->root = m->NIL;
    m->max_input_pos = 0;
    // Initialisation du cache
    m->cacheMapping = NULL;
    m->cacheSize = 0;
    m->cacheValid = 0;
    return m;
}

void MAGICadd(MAGIC m, int pos, int length) 
{
    assert(m && length > 0);
    int input_pos = MAGICmap(m, STREAM_OUT_IN, pos);
    if (input_pos != -1) {
        insertDelta(m, input_pos, length);
    } else {
        // Calcul de la position de sortie maximale actuelle.
        int max_out = m->max_input_pos + getCumulativeDelta(m->root, m->NIL, m->max_input_pos);
        // Si pos est supérieur à max_out, on étend le flux.
        if (pos > max_out) {
            input_pos = m->max_input_pos + (pos - max_out);
            insertDelta(m, input_pos, length);
        } else {
            // Cas moins fréquent : on recherche un candidat en rétrogradant.
            for (int i = pos - 1; i >= 0; --i) {
                int candidate = MAGICmap(m, STREAM_OUT_IN, i);
                if (candidate != -1) {
                    insertDelta(m, candidate + 1, length);
                    goto invalidate_cache;
                }
            }
            insertDelta(m, 0, length);
        }
    }
    invalidate_cache:
    m->cacheValid = 0; // Invalide le cache après modification
}

void MAGICremove(MAGIC m, int pos, int length)
{
    assert(m && length > 0);
    int input_pos = MAGICmap(m, STREAM_OUT_IN, pos);
    if (input_pos == -1) {
        // Si pos est inférieur à la plus petite position de sortie valide,
        // on considère que la suppression s'applique au début (input 0).
        if (!m->cacheValid) {
            updateCache(m);
        }
        int min_valid_out = -1;
        for (int i = 0; i < m->cacheSize; i++) {
            if (m->cacheMapping[i] != -1) {
                min_valid_out = i;
                break;
            }
        }
        if (min_valid_out != -1 && pos < min_valid_out) {
            input_pos = m->cacheMapping[min_valid_out];
        }
    }
    if (input_pos != -1)
    {
        insertDelta(m, input_pos, -length);
    } 
    else 
    {
        for (int i = pos + 1; i <= m->max_input_pos + 1; ++i)
        {
            int candidate = MAGICmap(m, STREAM_OUT_IN, i);
            if (candidate != -1)
            {
                insertDelta(m, candidate, -length);
                goto invalidate_cache2;
            }
        }
    }
    invalidate_cache2:
    m->cacheValid = 0; // Invalide le cache après modification
}

int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos)
{
    assert(m && pos >= 0);
    
    if (direction == STREAM_IN_OUT) {
        if (isActuallyRemoved(m->root, m->NIL, pos)) return -1;
        return pos + getCumulativeDelta(m->root, m->NIL, pos);
    } else { // STREAM_OUT_IN
        // Si le cache n'est pas valide, on le met à jour.
        if (!m->cacheValid) {
            updateCache(m);
        }
        // Si pos est couvert par le cache, on le retourne.
        if (pos < m->cacheSize)
            return m->cacheMapping[pos];
        else {
            // Sinon, on effectue une recherche binaire sur un intervalle étendu.
            int low = 0, high = pos + 100; // On étend la recherche d'une marge (ici 100).
            int candidate = -1;
            while (low <= high) {
                int mid = (low + high) / 2;
                int mapped = mid + getCumulativeDelta(m->root, m->NIL, mid);
                if (mapped == pos) {
                    candidate = mid;
                    break;
                } else if (mapped < pos) {
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }
            if (candidate != -1 && !isActuallyRemoved(m->root, m->NIL, candidate))
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
    free(m);
}

//==============================================================================
//==============================MAGIC DEBUGGING=================================
//==============================================================================

void MAGICstream(MAGIC m, int max_in) 
{
    char out_chars[2048] = {0};
    int out_pos_max = 0;
    for (int i = 0; i <= max_in; ++i) {
        int mapped = MAGICmap(m, STREAM_IN_OUT, i);
        if (mapped != -1) {
            out_chars[mapped] = (i < 26) ? ('a' + i) : '.';
            if (mapped > out_pos_max) out_pos_max = mapped;
        }
    }
    char next_added = 'R';
    for (int i = 0; i <= out_pos_max + 10; ++i)
    {
        if (!out_chars[i])
        {
            int back = MAGICmap(m, STREAM_OUT_IN, i);
            if (back == -1)
            {
                out_chars[i] = (next_added <= 'Z') ? next_added++ : '*';
                if (i > out_pos_max) out_pos_max = i;
            }
        }
    }
    printf("OUT pos: ");
    for (int i = 0; i <= out_pos_max; ++i)
        printf("%2d ", i);
    printf("\nOUT:     ");
    for (int i = 0; i <= out_pos_max; ++i)
        printf(" %c ", out_chars[i] ? out_chars[i] : '.');
    printf("\n");
}
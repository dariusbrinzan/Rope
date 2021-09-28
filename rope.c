#include "./rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define EMPTY ""

// Create a node with the given text
RopeNode* makeRopeNode(const char* str)
{
    // Allocate memory for the node
    RopeNode* new_Node = NULL;
    new_Node = calloc(1, sizeof(RopeNode));
    DIE(!new_Node, "Couldn't create new node!");

    // Fill node's fields
    new_Node->left = NULL;
    new_Node->right = NULL;
    new_Node->str = str;
    new_Node->weight = strlen(new_Node->str);

    // Return the created node
    return new_Node;
}

RopeTree* makeRopeTree(RopeNode* root)
{
    // Create tree
    RopeTree* new_Tree = NULL;
    new_Tree = calloc(1, sizeof(RopeTree));
    DIE(!new_Tree, "Couldn't create tree!");

    // Assign tree's root
    new_Tree->root = root;

    // Return the created tree
    return new_Tree;
}

// Print the whole tree
void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

// Print the whole tree
void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}


int getTotalWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    return rt->weight + getTotalWeight(rt->right);
}


RopeTree* concat(RopeTree* rt1, RopeTree* rt2)
{
    // Create new tree and assign it an empty root
    RopeTree* new_Tree = NULL;
    new_Tree = makeRopeTree(makeRopeNode(strdup(EMPTY)));

    // Assing new values
    new_Tree->root->right = rt2->root;
    new_Tree->root->left  = rt1->root;
    new_Tree->root->weight = getTotalWeight(rt1->root);

    // Return the newly created tree
    return new_Tree;
}

char __indexRope(RopeNode* root, int idx)
{
    // Search by index
    if (!root -> left && !root -> right) {
        // Return character
        return root->str[idx];
    }

    // Get through all the nodes
    if (root->weight <= idx) {
        return __indexRope(root->right, idx-root->weight);
    } else {
        return __indexRope(root->left, idx);
    }
}

char indexRope(RopeTree* rt, int idx)
{
    return __indexRope(rt->root, idx);
}

char* search(RopeTree* rt, int start, int end)
{
    // Create string to be returned
    char* searched_string = NULL;
    searched_string = calloc(NODE_DATA_MAX_LENGTH, sizeof(char));
    DIE(!searched_string, "Couldn't search string!");

    // Save in the string all the found items
    for (int i = start; i < end; i++) {
        searched_string[i - start] = indexRope(rt, i);
    }

    // Return the string's address
    return searched_string;
}

SplitPair SplitUtil(RopeNode* node, int idx) {
    if (idx < node->weight) {
        SplitPair pair = SplitUtil(node->left, idx);
        SplitPair result = {pair.left, 
                            concat(makeRopeTree(pair.right),
                                   makeRopeTree(node->right))->root};
        return result;
    } else if (idx > node->weight) {
        SplitPair pair = SplitUtil(node->right, idx - node->weight);
        SplitPair result = {concat(makeRopeTree(node->left),
                                   makeRopeTree(pair.left))->root,
                            pair.right};
        return result;
    } else {
        SplitPair result = {node->left, node->right};
        return result;
    }
}

SplitPair split(RopeTree* rt, int idx)
{
    // Create split pair to be returned

    int tmp_idx = idx;

    RopeNode* curr_node = rt->root;
    RopeNode* prev_node = NULL;
    while (curr_node) {
        prev_node = curr_node;
        if (idx < curr_node->weight) {
            curr_node = curr_node->left;
        } else {
            idx -= curr_node->weight;
            curr_node = curr_node->right;
        }
    }

    if (idx) {
        char left_str[256];
        char right_str[256];

        for (int i = 0; i < idx; ++i) {
            left_str[i] = prev_node->str[i];
        }
        left_str[idx] = '\0';

        for (int i = idx; i < prev_node->weight; ++i) {
            right_str[i] = prev_node->str[i];
        }
        right_str[prev_node->weight - idx] = '\0';

        RopeNode* left_node = makeRopeNode(left_str);
        RopeNode* right_node = makeRopeNode(right_str);

        prev_node->left = left_node;
        prev_node->right = right_node;
        prev_node->str = NULL;
        prev_node->weight = idx;
    }

    return SplitUtil(rt->root, tmp_idx);
}

RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    SplitPair pair = split(rt, idx);
    return concat(concat(makeRopeTree(pair.left),
                         makeRopeTree(makeRopeNode(str))), 
                  makeRopeTree(pair.right));
}

RopeTree* delete(RopeTree* rt, int start, int len) {
    SplitPair pair1 = split(rt, start);
    SplitPair pair2 = split(makeRopeTree(pair1.right), start + len);
    return concat(makeRopeTree(pair1.left),
                  makeRopeTree(pair2.right));
}

// FINAL 10p -> complex test involving all operations


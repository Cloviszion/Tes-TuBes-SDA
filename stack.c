#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "queue.h"
#include "tree.h"

extern HistoryNode* historyHead;
extern QueueNode* queueHead;
extern TreeNode* root;

StackNode* Stack_createNode(const char* operation, TreeNode* node) {
    StackNode* stackNode = (StackNode*)malloc(sizeof(StackNode));
    if (!stackNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(stackNode->operation, operation);
    stackNode->node = node;
    stackNode->next = NULL;
    return stackNode;
}

void Stack_pushAdd(StackNode** head, const char* operation) {
    StackNode* stackNode = Stack_createNode(operation, NULL);
    stackNode->next = *head;
    *head = stackNode;
}

void Stack_pushDelete(StackNode** head, TreeNode* node, const char* operation) {
    StackNode* stackNode = Stack_createNode(operation, node);
    stackNode->next = *head;
    *head = stackNode;
}

void Stack_undoAdd(StackNode** addStack, QueueNode** queueHead, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*addStack) {
        printf("No add operations to undo!\n");
        return;
    }
    
    StackNode* stackNode = *addStack;
    *addStack = (*addStack)->next;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Undo %s", stackNode->operation);
    addToHistory(&historyHead, operation);
    
    char* name = strstr(stackNode->operation, "Added ") + 6;
    char* end = strchr(name, ' ');
    if (end) *end = '\0';
    
    QueueNode* current = *queueHead;
    QueueNode* prev = NULL;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                *queueHead = current->next;
            }
            free(current);
            free(stackNode);
            printf("Undo add successful!\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    
    TreeNode* node = Tree_findNode(root, name);
    if (node) {
        for (int i = 0; i < root->childCount; i++) {
            for (int j = 0; j < root->children[i]->childCount; j++) {
                if (root->children[i]->children[j] == node) {
                    for (int k = j; k < root->children[i]->childCount - 1; k++) {
                        root->children[i]->children[k] = root->children[i]->children[k + 1];
                    }
                    root->children[i]->childCount--;
                    root->children[i]->children = (TreeNode**)realloc(root->children[i]->children, root->children[i]->childCount * sizeof(TreeNode*));
                    free(node);
                    free(stackNode);
                    printf("Undo add successful!\n");
                    return;
                }
            }
        }
    }
    free(stackNode);
    printf("Node not found for undo!\n");
}

void Stack_undoDelete(StackNode** deleteStack, TreeNode* root, void (*addToHistory)(HistoryNode**, const char*)) {
    if (!*deleteStack) {
        printf("No delete operations to undo!\n");
        return;
    }
    
    StackNode* stackNode = *deleteStack;
    *deleteStack = (*deleteStack)->next;
    
    char operation[200];
    snprintf(operation, sizeof(operation), "Undo %s", stackNode->operation);
    addToHistory(&historyHead, operation);
    
    TreeNode* node = stackNode->node;
    for (int i = 0; i < root->childCount; i++) {
        Tree_addNode(root->children[i], node);
        break; // Add to first valid parent
    }
    
    free(stackNode);
    printf("Undo delete successful!\n");
}

void Stack_free(StackNode* head) {
    while (head) {
        StackNode* temp = head;
        head = head->next;
        free(temp);
    }
}
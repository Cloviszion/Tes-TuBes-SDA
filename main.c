#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "history.h"
#include "tree.h"
#include "queue.h"
#include "stack.h"

TreeNode* root = NULL;
QueueNode* queueHead = NULL;
StackNode* addStack = NULL;
StackNode* deleteStack = NULL;
HistoryNode* historyHead = NULL;

void initTree() {
    FILE* file = fopen("jawabarat_hierarchy.json", "r");
    if (!file) {
        printf("Error opening JSON file!\n");
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = (char*)malloc(length + 1);
    if (!data) {
        printf("Memory allocation failed!\n");
        fclose(file);
        exit(1);
    }
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);
    
    cJSON* json = cJSON_Parse(data);
    free(data);
    if (!json) {
        printf("Error parsing JSON!\n");
        return;
    }
    
    root = Tree_parseJSON(json);
    cJSON_Delete(json);
    if (root) {
        History_add(&historyHead, "Initialized tree from JSON");
        printf("Tree initialized successfully!\n");
    }
}

void addToQueue() {
    char name[100], type[20], parentName[100];
    printf("Enter entity name: ");
    scanf(" %[^\n]", name);
    printf("Enter entity type (provinsi/kota/kecamatan/kelurahan/rw/rt): ");
    scanf(" %[^\n]", type);
    printf("Enter parent name: ");
    scanf(" %[^\n]", parentName);
    
    Queue_add(&queueHead, name, type, parentName, History_add, Stack_pushAdd);
}

void manageQueue() {
    int choice;
    do {
        printf("\nQueue Management Submenu\n");
        printf("1. Process Data to Tree/JSON\n");
        printf("2. Edit Data in Queue\n");
        printf("3. Delete Data from Queue\n");
        printf("4. Display Queue\n");
        printf("5. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: Queue_processToTree(&queueHead, root, History_add); break;
            case 2: {
                char name[100];
                printf("Enter entity name to edit: ");
                scanf(" %[^\n]", name);
                Queue_edit(queueHead, name, History_add);
                break;
            }
            case 3: {
                char name[100];
                printf("Enter entity name to delete: ");
                scanf(" %[^\n]", name);
                Queue_delete(&queueHead, name, History_add);
                break;
            }
            case 4: Queue_display(queueHead); break;
            case 5: break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 5);
}

void deleteNode() {
    char name[100];
    printf("Enter node name to delete: ");
    scanf(" %[^\n]", name);
    Tree_deleteNode(&root, name, History_add, Stack_pushDelete);
}

void editNode() {
    char name[100], newName[100], newType[20];
    printf("Enter node name to edit: ");
    scanf(" %[^\n]", name);
    printf("Enter new name: ");
    scanf(" %[^\n]", newName);
    printf("Enter new type: ");
    scanf(" %[^\n]", newType);
    Tree_editNode(root, name, newName, newType, History_add);
}

void searchNode() {
    char name[100];
    printf("Enter node name to search: ");
    scanf(" %[^\n]", name);
    Tree_searchNode(root, name);
}

void displaySubtree() {
    char name[100];
    printf("Enter node name to display subtree: ");
    scanf(" %[^\n]", name);
    Tree_displaySubtree(root, name);
}

void displayStats() {
    int stats[6] = {0}; // provinsi, kota, kecamatan, kelurahan, rw, rt
    Tree_calculateStats(root, stats);
    printf("Statistics:\n");
    printf("Provinsi: %d\n", stats[0]);
    printf("Kota: %d\n", stats[1]);
    printf("Kecamatan: %d\n", stats[2]);
    printf("Kelurahan: %d\n", stats[3]);
    printf("RW: %d\n", stats[4]);
    printf("RT: %d\n", stats[5]);
}

void saveTreeToJSON() {
    if (!root) {
        printf("Tree is empty!\n");
        return;
    }
    
    cJSON* json = cJSON_CreateObject();
    Tree_saveToJSON(root, json);
    
    char* output = cJSON_Print(json);
    FILE* file = fopen("jawabarat_updated.json", "w");
    if (!file) {
        printf("Error opening output file!\n");
        cJSON_Delete(json);
        free(output);
        return;
    }
    
    fprintf(file, "%s", output);
    fclose(file);
    cJSON_Delete(json);
    free(output);
    
    History_add(&historyHead, "Saved tree to jawabarat_updated.json");
    printf("Tree saved to jawabarat_updated.json!\n");
}

void cleanup() {
    Tree_free(root);
    Queue_free(queueHead);
    Stack_free(addStack);
    Stack_free(deleteStack);
    History_free(historyHead);
}

int main() {
    int choice;
    do {
        printf("\nMain Menu\n");
        printf("1. Initialize Tree from JSON\n");
        printf("2. Add New Entity\n");
        printf("3. Manage Queue\n");
        printf("4. Delete Entity\n");
        printf("5. Edit Entity\n");
        printf("6. Search Data\n");
        printf("7. Display Hierarchy (Preorder)\n");
        printf("8. Display Hierarchy (Level-Order)\n");
        printf("9. Display Subtree\n");
        printf("10. Calculate Statistics\n");
        printf("11. Display Operation History\n");
        printf("12. Undo Add\n");
        printf("13. Redo Add\n");
        printf("14. Undo Delete\n");
        printf("15. Save to JSON\n");
        printf("16. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: initTree(); break;
            case 2: addToQueue(); break;
            case 3: manageQueue(); break;
            case 4: deleteNode(); break;
            case 5: editNode(); break;
            case 6: searchNode(); break;
            case 7: if (root) Tree_displayPreorder(root, 0); else printf("Tree is empty!\n"); break;
            case 8: Tree_displayLevelOrder(root); break;
            case 9: displaySubtree(); break;
            case 10: displayStats(); break;
            case 11: History_display(historyHead); break;
            case 12: Stack_undoAdd(&addStack, &queueHead, root, History_add); break;
            case 13: printf("Redo add not implemented.\n"); break;
            case 14: Stack_undoDelete(&deleteStack, root, History_add); break;
            case 15: saveTreeToJSON(); break;
            case 16: cleanup(); printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 16);
    
    return 0;
}
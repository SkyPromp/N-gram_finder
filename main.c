#include <stdio.h>
#include <stdlib.h>
#define MAX_SERIALIZABLE 5  // 13 is the theoretical max, but is way too big to use practically in a hashmap


typedef struct Buffer{
    size_t len;
    char* cp;
} Buffer;

typedef struct Tree{
    unsigned long long id;
    unsigned long long value;
    struct Tree* left;
    struct Tree* right;
} Tree;

typedef struct LinkedList{
    unsigned long long id;
    unsigned long long value;
    struct LinkedList* next;
} LinkedList;

unsigned long long pow(unsigned long long, unsigned long long);
void printBuff(Buffer*);
void buffAdd(Buffer*, char);
int readNext(Buffer*, FILE*);
void buffClear(Buffer*);
unsigned long long serializeBuffer(Buffer*);  // 27 ** n + 27 ** (n - 1) ... for array lookup
void printDeserializeBuffer(unsigned long long, FILE*);
void storeTree(Tree*, unsigned long long);
void freeLinkedList(LinkedList*);
void appendToLinkedList(LinkedList**, LinkedList*);
void treeToLinkedList(Tree*, LinkedList*);
void BSTInsert(Tree*, unsigned long long, unsigned long long);
void linkedListToBST(Tree*, LinkedList*);  // TODO
void sortTree(Tree**);
void printTree(Tree*, FILE*);
void freeTree(Tree*);
size_t lenTree(Tree*);
size_t lenLinkedList(LinkedList*);

int main(){
    FILE *fptr;
    fptr = fopen("../words.txt", "r");

    if (!fptr) {
        printf("File Not Found!");
        return 1;
    }

    const size_t WORD_LENGTH = 3;
    char ngram[WORD_LENGTH + 1];
    ngram[WORD_LENGTH] = '\0';
    Buffer b;
    Buffer* buff = &b;
    buff->len = WORD_LENGTH;
    buff->cp = ngram;
    buffClear(buff);

    Tree* tree = malloc(sizeof(Tree));
    tree->id = 1;
    tree->value = 0;
    tree->left = NULL;
    tree->right = NULL;

    while(readNext(buff, fptr)){
        unsigned long long id = serializeBuffer(buff);
        storeTree(tree, id);
    }

    fclose(fptr);

    fptr = fopen("../output.txt", "w");
    printTree(tree, fptr);
    fclose(fptr);

    fptr = fopen("../sorted_output.txt", "w");
    sortTree(&tree);
    printf("tree length: %llu\n", lenTree(tree)); // LEN = 7104
    printTree(tree, fptr);
    fclose(fptr);

    freeTree(tree);

    return 0;
}

unsigned long long pow(unsigned long long value, unsigned long long exponent){
    unsigned long long output = 1;

    for(int i = 0; i < exponent; i++){
        output *= value;
    }

    return output;
}

void buffAdd(Buffer* buff, char character){
    for(size_t i = 1; i < buff->len; i++){
        buff->cp[i - 1] = buff->cp[i];
    }

    buff->cp[buff->len - 1] = character;
}

int readNext(Buffer* buff, FILE* fptr){
    char last_char;
    do{
        last_char = (char)fgetc(fptr);
        if('A' <= last_char && last_char <= 'Z') last_char += 'a' - 'A';
        buffAdd(buff, last_char);

        if('a' > last_char || last_char > 'z') buffClear(buff);
    } while (last_char != EOF && buff->cp[0] == '\0');

    return last_char != EOF;
}

void printBuff(Buffer* buff){
    for(size_t i = 0; i < buff->len; i++){
        putchar(buff->cp[i]);
    }

    putchar('\n');
}

void buffClear(Buffer* buff){
    for(size_t i = 0; i < buff->len; i++){
        buff->cp[i] = '\0';
    }
}

unsigned long long serializeBuffer(Buffer* buff){
    unsigned long long value = 0;

    if (buff->len > MAX_SERIALIZABLE) return 0;

    for(size_t i = 0; i < buff->len; i++){
        value += pow(27, i) * (buff->cp[i] - 'a' + 1);
    }

    return value;
}

void printDeserializeBuffer(unsigned long long id, FILE* fptr){
    while(id > 0){
        char last_char = id % 27 - 1 + 'a';
        id /= 27; // integer division
        putc(last_char, fptr);
    }
}

void storeTree(Tree* tree, unsigned long long id){
    if(tree->id == id) tree->value += 1;
    else if(id > tree->id){
        if(tree->right == NULL){
            Tree* new_tree = malloc(sizeof(Tree));
            new_tree->value = 1;
            new_tree->id = id;
            new_tree->left = NULL;
            new_tree->right = NULL;
            tree->right = new_tree;
        } else{
            storeTree(tree->right, id);
        }
    }else{
        if(tree->left == NULL){
            Tree* new_tree = malloc(sizeof(Tree));
            new_tree->value = 1;
            new_tree->id = id;
            new_tree->left = NULL;
            new_tree->right = NULL;
            tree->left = new_tree;
        } else{
            storeTree(tree->left, id);
        }
    }
}

void printTree(Tree* tree, FILE* fptr){
    if(tree == NULL) return;

    printTree(tree->left, fptr);
    printTree(tree->right, fptr);
    if(tree->id != 1){
        fprintf(fptr, "ID: %llu | BUFF: ", tree->id);
        printDeserializeBuffer(tree->id, fptr);
        fprintf(fptr, " | VALUE: %llu\n", tree->value);
    }
}

void appendToLinkedList(LinkedList** list, LinkedList* element){
    if(*list == NULL) *list = element;
    else{
        LinkedList* current = *list;

        while(current->next != NULL){ // TODO BUGFIX crashes at 2nd iteration
            current = current->next;
        }

        current->next = element;
    }
}

void treeToLinkedList(Tree* tree, LinkedList* list){
    if(tree == NULL) return;

    LinkedList* next = malloc(sizeof(LinkedList));
    next->value = tree->value;
    next->id = tree->id;
    next->next = NULL;

    appendToLinkedList(&list, next);

    treeToLinkedList(tree->left, list);
    treeToLinkedList(tree->right, list);
}

void BSTInsert(Tree* tree, unsigned long long value, unsigned long long id){  // TODO BUGFIX VALUES DISAPPEAR (ROOT MIGHT BE OVERWRITTEN)
    if(value > tree->value){
        if(tree->left == NULL){
            Tree* new_tree = malloc(sizeof(Tree));
            new_tree->value = value;
            new_tree->id = id;
            new_tree->left = NULL;
            new_tree->right = NULL;
            tree->left = new_tree;
        } else{
            BSTInsert(tree->left, value, id);
        }
    }else{
        if(tree->right == NULL){
            Tree* new_tree = malloc(sizeof(Tree));
            new_tree->value = value;
            new_tree->id = id;
            new_tree->left = NULL;
            new_tree->right = NULL;
            tree->right = new_tree;
        } else{
            BSTInsert(tree->right, value, id);
        }
    }
}

void linkedListToBST(Tree* tree, LinkedList* linkedList){  // TODO BUGFIX at 2063 inserts the tree gets reset somehow
    LinkedList* last = linkedList;
    int a = 1;
    while(last != NULL){
//        printf("%d %llu %p\n", a++, lenTree(tree), tree);
        BSTInsert(tree, last->value, last->id);
        last = last->next;
    }
}

void freeLinkedList(LinkedList* linkedList){
    if(linkedList == NULL) return;
    freeLinkedList(linkedList->next);
    free(linkedList);
}

void sortTree(Tree** treeptr){
    Tree* tree = *treeptr;
    LinkedList* linkedList = malloc(sizeof(LinkedList));
    linkedList->next = NULL;
    linkedList->value = 0;
    linkedList->id = 1;

    printf("tree length: %llu\n", lenTree(tree));  // LEN = 9166

    treeToLinkedList(tree, linkedList);
    printf("linked list length: %llu\n", lenLinkedList(linkedList));  // LEN = 9167 (+1 from root node)
    freeTree(tree);
    tree = malloc(sizeof(Tree));
    tree->id = 1;
    tree->value = 0;
    tree->right = NULL;
    tree->left = NULL;
    linkedListToBST(tree, linkedList);
    printf("tree length: %llu\n", lenTree(tree)); // LEN = 7104
    *treeptr = tree;
    //freeLinkedList(linkedList);
}

void freeTree(Tree* tree){
    if(tree == NULL) return;
    if(tree->left != NULL) freeTree(tree->left);
    if(tree->right != NULL) freeTree(tree->right);
    free(tree);
}

size_t lenTree(Tree* tree){
    if(tree == NULL) return 0;
    return lenTree(tree->left) + lenTree(tree->right) + 1;
}

size_t lenLinkedList(LinkedList* linkedList){
    size_t total = 0;
    LinkedList* list = linkedList;

    while(list != NULL) {
        total += 1;
        list = list->next;
    }

    return total;
}

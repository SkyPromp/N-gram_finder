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

unsigned long long pow(unsigned long long, unsigned long long);
void printBuff(Buffer* buff);
void buffAdd(Buffer*, char);
int readNext(Buffer*, FILE*);
void buffClear(Buffer*);
unsigned long long serializeBuffer(Buffer*);  // 27 ** n + 27 ** (n - 1) ... for array lookup
void printDeserializeBuffer(unsigned long long);
void storeTree(Tree*, unsigned long long);
void printTree(Tree*);


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
    Tree t;
    Tree* tree = &t;
    tree->id = 1;
    tree->value = 0;
    tree->left = NULL;
    tree->right = NULL;

    while(readNext(buff, fptr)){
//        printBuff(buff);
        unsigned long long id = serializeBuffer(buff);
        storeTree(tree, id);
    }

    printTree(tree);

    fclose(fptr);
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
        buffAdd(buff, last_char);

        if(last_char == '\n') buffClear(buff);
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

void printDeserializeBuffer(unsigned long long id){
    printf("deserialized: ");

    while(id > 0){
        char last_char = id % 27 - 1 + 'a';
        id /= 27; // integer division
        putchar(last_char);
    }

    //putchar('\n');
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

//            printf("right ");
//            printDeserializeBuffer(id);
//            putchar('\n');
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

//            printf("left ");
//            printDeserializeBuffer(id);
//            putchar('\n');
        } else{
            storeTree(tree->left, id);
        }
    }
}

void printTree(Tree* tree){
    if(tree == NULL) return;

    if(tree->id != 1){
        printf("ID: %llu | BUFF: ", tree->id);
        printDeserializeBuffer(tree->id);
        printf(" | VALUE: %llu\n", tree->value);
    }

    if(tree->left != NULL) printTree(tree->left);
    if(tree->right != NULL) printTree(tree->right);
}

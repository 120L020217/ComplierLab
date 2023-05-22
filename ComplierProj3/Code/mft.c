#include "mft.h"

int convert2binary(char* text){
    if (*text == '\0') return -1;
    char* temp = text;
    int ret = 0;
    if (text[1] == 'x' || text[1] == 'X'){ // hexadecimal
        temp += 2;
        while(*(temp) != '\0'){
            ret = ret << 4;
            switch (*(temp))
            {
            case '0':
                ret += 0;
                break;
            case '1':
                ret += 1;
                break;
            case '2':
                ret += 2;
                break;
            case '3':
                ret += 3;
                break;
            case '4':
                ret += 4;
                break;
            case '5':
                ret += 5;
                break;
            case '6':
                ret += 6;
                break;
            case '7':
                ret += 7;
                break;
            case '8':
                ret += 8;
                break;
            case '9':
                ret += 9;
                break;
            case 'A':
            case 'a':
                ret += 10;
                break;
            case 'B':
            case 'b':
                ret += 11;
                break;
            case 'C':    
            case 'c':
                ret += 12;
                break;
            case 'D':
            case 'd':
                ret += 13;
                break;
            case 'E':
            case 'e':
                ret += 14;
                break;
            case 'F':
            case 'f':
                ret += 15;
                break;
 
            default:
                break;
            }
            temp++;
        }
        return ret;
    }else if(text[0] == '0' && text[1] != '\0'){ // octal
        temp++;
        while(*(temp) != '\0'){
            ret = ret << 3;
            ret += (*(temp) - '0');
            temp++;
        }
        return ret;
    }else{ // binary
        return atoi(temp);
    }
}

/**
 * @brief  add a non-terminator
 * 
 * @param line passed by yylineno
 * @param name : name(passed by programmer and MUST be const string)
 * @return struct MFT* 
 */
struct MFT* addNode(int line, char* name, enum TYPE type, int argc, ...){
    struct MFT* new = (struct MFT*)malloc(sizeof(struct MFT));
    new->line = line;
    new->name = name;
    new->type = type;

    if (!argc){
        new->child = NULL;
        new->brother = NULL;
        return new;
    }
    va_list vaList;
    va_start(vaList, argc);
    struct MFT* next = va_arg(vaList, struct MFT*);
    if (next != NULL)
        new->child = next;
    for(int i = 1; i < argc; ++i){
        next->brother = va_arg(vaList, struct MFT*);
        // if (next->brother != NULL)
        next = next->brother;
        // else{
        //     next->brother = va_arg(vaList, struct MFT*);
        //     next = next->brother;
        //     i++;
        // }
    }
    return new;
}
/**
 * @brief add a terminator
 * 
 * @param line :passed by yylineno
 * @param name : name of node(passed by programmer and MUST be const string)
 * @param val :value of node(passed by yytext)
 * @return struct MFT* 
 */
struct MFT* addLeaf(int line, char* name, enum TYPE type, char* val){
    struct MFT* new = (struct MFT*)malloc(sizeof(struct MFT));
    new->line = line;
    new->name = name;
    new->type = type;
    int vallen = strlen(val) + 1;
    if (!strcmp(name, "ID")){
        new->id = (char*)malloc(sizeof(char) * vallen);
        strcpy(new->id, val);
    }else if(!strcmp(name, "FLOAT")){
        new->f = atof(val);
    }else if(!strcmp(name, "INT")){
        new->i = convert2binary(val);
    }else if(!strcmp(name, "TYPE")){
        new->id = (char*)malloc(sizeof(struct MFT));
        strcpy(new->id, val);
    }else if(!strcmp(name, "RELOP")){
        new->id = (char*)malloc(sizeof(char) * vallen);
        strcpy(new->id, val);
    }

    new->child = NULL;
    new->brother = NULL;
    return new;
}

void release(struct MFT* root){
    if (root == NULL){
        return ;
    }
    struct MFT* temp = root->child;
    struct MFT* temp_next;
    while(temp != NULL){
        temp_next = temp->brother;
        release(temp);
        temp = temp_next;
    }
    free(root);
}
/**
 * @brief 
 * 
 * @param root 
 * @param layer represent which layer the node is located(start with 1)
 */
void preorder(struct MFT* root, int layer){
    if (root->type != NODE_NULL){
         for (int i = 1; i < layer; ++i){
            printf("  ");
    }
    }
   
    struct MFT* next;
    if (root->type == NOT_A_TOKEN){
        printf("%s (%d)\n", root->name, root->line);
    }else if(root->type == TOKEN_TYPE || root->type == TOKEN_ID){
        printf("%s: %s\n", root->name, root->id);
    }else if(root->type == TOKEN_FLOAT){
        printf("%s: %f\n", root->name, root->f);
    }else if(root->type == TOKEN_INT){
        printf("%s: %d\n", root->name, root->i);
    }else if(root->type == TOKEN_OTHERS){
        printf("%s\n", root->name);
    }else if(root->type == NODE_NULL){
        ;
    }
    if (root->child != NULL){
        preorder(root->child, layer + 1);
        next = root->child->brother;
        while(next){
            preorder(next, layer + 1);
            next = next->brother;
        }
    }
}

// int main(){

//     struct MFT *l1, *l2, *l3, *l4, *l5;
//     struct MFT *n1, *n2;
//     l1 = addLeaf(3, "INT", TOKEN_INT, "037");
//     l2 = addLeaf(3, "STAR", TOKEN_OTHERS, "*");
//     l3 = addLeaf(3, "INT", TOKEN_INT, "0x1");
//     l4 = addLeaf(2, "PLUS", TOKEN_OTHERS, "+");
//     l5 = addLeaf(2, "INT", TOKEN_INT, "0x1");

//     n1 = addNode(2, "ji", NOT_A_TOKEN, 3, l1, l2, l3);
//     n2 = addNode(1, "he", NOT_A_TOKEN, 3, n1, l4, l5);

//     preorder(n2, 1);
//     delete(n2);
// }
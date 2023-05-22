#include "inter.h"

InterCodes ics;
extern Table table;
// int var_no = 0;
int lab_no = 0;
int tmp_no = 0;
#define MAX_ARG_NUM 10

Operand newOperand(int kind, ...){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    va_list vaList;
    va_start(vaList, kind);
    switch (kind)
    {
    case TEMP:
        op->u.var_no = tmp_no++;
        break;
    case CONSTANT:
        op->u.value = va_arg(vaList, int);
        break;
    case OP_LABEL:
        op->u.var_no = lab_no++;
        break;
    case VARIABLE:
    case ADDRESS:
    case OP_FUNC:
    case OP_RELOP:
        op->u.name = va_arg(vaList, char*);
        break;
    default:
        break;
    }
    va_end(vaList);
    return op;
}
void deleteOperand(Operand op){
    // if (!op) return;
    int kind = op->kind;
    switch (kind)
    {
    case VARIABLE:
    case ADDRESS:
    case OP_FUNC:
    case OP_RELOP:
        free(op->u.name);
        break;
    default:
        break;
    }
    free(op);
    op = NULL;
}
void fprintOperand(char* t, Operand op){
    int kind = op->kind;
    char str[3];
    switch (kind)
    {
    case TEMP:
        snprintf(str, sizeof(str), "%d", op->u.var_no);
        strcat(t, "t");
        strcat(t, str);
        break;
    case CONSTANT:
        snprintf(str, sizeof(str), "%d", op->u.value);
        strcat(t, "#");
        strcat(t, str);
        break;
    case OP_LABEL:
        snprintf(str, sizeof(str), "%d", op->u.var_no);
        strcat(t, "label");
        strcat(t, str);
        break;
    case VARIABLE:
    case ADDRESS:
    case OP_FUNC:
    case OP_RELOP:
        // printf("test6\n");
        strcat(t, op->u.name);
        // printf("test7\n");
        break; 
    default:
        break;
    }
}

InterCode newInterCode(int kind, ...){
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = kind;
    va_list vaList;
    va_start(vaList, kind);
    switch (kind)
    {
    case LABEL:
    case FUNC:
    case GOTO:
    case CODE_RETURN:
    case ARG:
    case PARAM:
    case READ:
    case WRITE:
        ic->u.sinop.val = va_arg(vaList, Operand);
        break;
    case ASSIGN:
    case GET_AD:
    case READ_AD:
    case WRITE_AD:
    case CALL:
        ic->u.binop.right = va_arg(vaList, Operand);
        ic->u.binop.left = va_arg(vaList, Operand);
        break;
    case ADD:
    case SUB:
    case MUL:
    case CODE_DIV:
        ic->u.triop.result = va_arg(vaList, Operand);
        ic->u.triop.op1 = va_arg(vaList, Operand);
        ic->u.triop.op2 = va_arg(vaList, Operand);
        break;
    case CODE_IF:
        ic->u.conjump.x = va_arg(vaList, Operand);
        ic->u.conjump.relop = va_arg(vaList, Operand);
        ic->u.conjump.y = va_arg(vaList, Operand);
        ic->u.conjump.z = va_arg(vaList, Operand);
        break;
    case DEC:
        ic->u.dec.x = va_arg(vaList, Operand);
        ic->u.dec.size = va_arg(vaList, int);
        break;
    default:
        break;
    }
    va_end(vaList);
}
void deleteInterCode(InterCode ic){
    if (!ic) return;
    int kind = ic->kind;
    switch (kind)
    {
    case LABEL:
    case FUNC:
    case GOTO:
    case CODE_RETURN:
    case ARG:
    case PARAM:
    case READ:
    case WRITE:
        deleteOperand(ic->u.sinop.val);
        break;
    case ASSIGN:
    case GET_AD:
    case READ_AD:
    case WRITE_AD:
    case CALL:
        deleteOperand(ic->u.binop.right);
        deleteOperand(ic->u.binop.left);
        break;
    case ADD:
    case SUB:
    case MUL:
    case CODE_DIV:
        deleteOperand(ic->u.triop.result);
        deleteOperand(ic->u.triop.op1);
        deleteOperand(ic->u.triop.op2);
        break;
    case CODE_IF:
        deleteOperand(ic->u.conjump.x);
        deleteOperand(ic->u.conjump.relop);
        deleteOperand(ic->u.conjump.y);
        deleteOperand(ic->u.conjump.z);
        break;
    case DEC:
        deleteOperand(ic->u.dec.x);
        break;
    default:
        break;
    }
    free(ic);
    ic = NULL;
}
void fprintInterCode(InterCode ic, FILE* f){
    int kind = ic->kind;
    char temp[50] = {'\0'};
    switch (kind)
    {
    case LABEL:
        strcat(temp, "LABEL ");
        fprintOperand(temp, ic->u.sinop.val);
        strcat(temp, " :");
        break;
    case FUNC:
        strcat(temp, "FUNCTION ");
        fprintOperand(temp, ic->u.sinop.val);
        strcat(temp, " :");
        break;
    case GOTO:
        strcat(temp, "GOTO ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case CODE_RETURN:
        strcat(temp, "RETURN ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case ARG:
        strcat(temp, "ARG ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case PARAM:
        strcat(temp, "PARAM ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case READ:
        strcat(temp, "READ ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case WRITE:
        strcat(temp, "WRITE ");
        fprintOperand(temp, ic->u.sinop.val);
        break;
    case ASSIGN:
        fprintOperand(temp, ic->u.binop.left);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.binop.right);
        break;
    case GET_AD:
        fprintOperand(temp, ic->u.binop.left);
        strcat(temp, " := &");
        fprintOperand(temp, ic->u.binop.right);
        break;
    case READ_AD:
        fprintOperand(temp, ic->u.binop.left);
        strcat(temp, " := *");
        fprintOperand(temp, ic->u.binop.right);
        break;
    case WRITE_AD:
        strcat(temp, "*");
        fprintOperand(temp, ic->u.binop.left);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.binop.right);
        break;
    case CALL:
        // printf("test1\n");
        fprintOperand(temp, ic->u.binop.left);
        // printf("test2\n");
        strcat(temp, " := CALL ");
        // printf("test3\n");
        fprintOperand(temp, ic->u.binop.right);
        // printf("test4: %s\n", ic->u.binop.right->u.name);
        break;
    case ADD:
        fprintOperand(temp, ic->u.triop.result);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.triop.op1);
        strcat(temp, " + ");
        fprintOperand(temp, ic->u.triop.op2);
        break;
    case SUB:
        fprintOperand(temp, ic->u.triop.result);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.triop.op1);
        strcat(temp, " - ");
        fprintOperand(temp, ic->u.triop.op2);
        break;
    case MUL:
        fprintOperand(temp, ic->u.triop.result);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.triop.op1);
        strcat(temp, " * ");
        fprintOperand(temp, ic->u.triop.op2);
        break;
    case CODE_DIV:
        fprintOperand(temp, ic->u.triop.result);
        strcat(temp, " := ");
        fprintOperand(temp, ic->u.triop.op1);
        strcat(temp, " / ");
        fprintOperand(temp, ic->u.triop.op2);
        break;
    case CODE_IF:
        strcat(temp, "IF ");
        // printf("test3\n");
        fprintOperand(temp, ic->u.conjump.x);
        strcat(temp, " ");
        // printf("test4\n");
        fprintOperand(temp, ic->u.conjump.relop);
        strcat(temp, " ");
        // printf("test5\n");
        fprintOperand(temp, ic->u.conjump.y);
        strcat(temp, " GOTO ");
        fprintOperand(temp, ic->u.conjump.z);
        break;
    case DEC:
        char str[5];
        snprintf(str, sizeof(str), "%d", ic->u.dec.size);
        strcat(temp, "DEC ");
        fprintOperand(temp, ic->u.dec.x);
        strcat(temp, " ");
        strcat(temp, str);
        strcat(temp, " ");
        break;
    default:
        break;
    }
    fprintf(f, "%s\n", temp);
}

InterCodes newInterCodes(){
    // head node
    InterCodes ics = (InterCodes)malloc(sizeof(struct InterCodes_));
    ics->code = NULL;
    ics->next = NULL;
    ics->next = NULL;
    return ics;
}
void addInterCodes(InterCodes ics, InterCode code){
    // insert into the front
    InterCodes new_ics = (InterCodes)malloc(sizeof(struct InterCodes_));
    new_ics->code = code;
    new_ics->prev = ics;
    new_ics->next = ics->next;
    if (ics->next) ics->next->prev = new_ics;
    ics->next = new_ics;
}
void deleteInterCodes(InterCodes ics){
    InterCodes pre = NULL;
    while(ics){
        pre = ics;
        ics = ics->next;
        deleteInterCode(pre->code);
        free(pre);
        pre = NULL;
    }
}
void fprintInterCodes(InterCodes ics, FILE* f){
    InterCodes temp = ics;
    while(temp->next){
        temp = temp->next;
    }
    while(temp->code){
        // printf("test\n");
        fprintInterCode(temp->code, f);
        // printf("test1\n");
        temp = temp->prev;
        // printf("test2\n");
    }
}

void transProgram(Tree root){
    transExtDefList(root->child);
}
void transExtDefList(Tree root){
    if (root->child){
        transExtDef(root->child);
        transExtDefList(root->child->brother);
    }
}
void transExtDef(Tree root){
    transFunDec(root->child->brother);
    transCompSt(root->child->brother->brother);
}

void transVarDec(Tree root){ // insert an item(varity defination)
    int size = 1;
    Tree t = root;
    while (!strcmp(t->child->name, "VarDec")){
        size *= t->child->brother->brother->i;
        t = t->child;
    }
    if (size != 1){ // 不考虑选做，只有int类型的变量
        addInterCodes(ics, newInterCode(DEC, newOperand(VARIABLE, newString(t->child->id)), size * 4));
    }
}

void transFunDec(Tree root){ // insert an item(function defination)
    addInterCodes(ics, newInterCode(FUNC, newOperand(OP_FUNC, newString(root->child->id))));
    if (!strcmp(root->child->brother->brother->name, "VarList")){
        transVarList(root->child->brother->brother);
    }
}
void transVarList(Tree root){
    transParamDec(root->child);
    if (root->child->brother){
        transVarList(root->child->brother->brother);
    }
}
void transParamDec(Tree root){ // 函数参数只有简单类型，没有数组
    addInterCodes(ics, newInterCode(PARAM, newOperand(VARIABLE, newString(root->child->brother->child->id))));
}

void transCompSt(Tree root){
    transDefList(root->child->brother);
    transStmtList(root->child->brother->brother);
}

void transDefList(Tree root){
    if(root->child){
        transDef(root->child);
        transDefList(root->child->brother);
    }
}
void transDef(Tree root){
    transDecList(root->child->brother);
}
void transDecList(Tree root){
    transDec(root->child);
    if (root->child->brother){
        transDecList(root->child->brother->brother);
    }
}
void transDec(Tree root){
    transVarDec(root->child);
}

void transStmtList(Tree root){
    if (root->child){
        transStmt(root->child);
        transStmtList(root->child->brother);
    }
}
void transStmt(Tree root){ // check the return type
    if (!strcmp(root->child->name, "Exp")){
        transExp(root->child, NULL);
    }else if (!strcmp(root->child->name, "CompSt")){
        transCompSt(root->child);
    }else if (!strcmp(root->child->name, "RETURN")){
        Operand place = newOperand(TEMP);
        transExp(root->child->brother, place);
        addInterCodes(ics, newInterCode(CODE_RETURN, place));
    }else if (!strcmp(root->child->name, "IF")){
        Operand tLabel = newOperand(OP_LABEL);
        Operand fLabel = newOperand(OP_LABEL);
        transCond(root->child->brother->brother, tLabel, fLabel);
        addInterCodes(ics, newInterCode(LABEL, tLabel));
        transStmt(root->child->brother->brother->brother->brother);
        if (root->child->brother->brother->brother->brother->brother){
            Operand nLabel = newOperand(OP_LABEL);
            addInterCodes(ics, newInterCode(GOTO, nLabel));
            addInterCodes(ics, newInterCode(LABEL, fLabel));
            transStmt(root->child->brother->brother->brother->brother->brother->brother);
            addInterCodes(ics, newInterCode(LABEL, nLabel));
        }else{
            addInterCodes(ics, newInterCode(LABEL, fLabel));
        }
    }else if (!strcmp(root->child->name, "WHILE")){
        Operand nLabel = newOperand(OP_LABEL);
        Operand tLabel = newOperand(OP_LABEL);
        Operand fLabel = newOperand(OP_LABEL);
        addInterCodes(ics, newInterCode(LABEL, nLabel));
        transCond(root->child->brother->brother, tLabel, fLabel);
        addInterCodes(ics, newInterCode(LABEL, tLabel));
        transStmt(root->child->brother->brother->brother->brother);
        addInterCodes(ics, newInterCode(GOTO, nLabel));
        addInterCodes(ics, newInterCode(LABEL, fLabel));
    }
}
void transExp(Tree root, Operand place){
    if (!strcmp(root->child->name, "INT")){
        if (place){
            tmp_no--;
            place->kind = CONSTANT;
            place->u.value = root->child->i;
        }
    }else if (!strcmp(root->child->name, "ID") && !(root->child->brother)){
        if (place){
            tmp_no--;
            place->kind = VARIABLE;
            // printf("%s\n", root->child->id);
            // place = newOperand(CONSTANT, 1);
            place->u.name = newString(root->child->id);
        }
    }else if (!strcmp(root->child->name, "MINUS")){
        Operand place1 = newOperand(TEMP);
        transExp(root->child->brother, place1);
        Operand zero = newOperand(CONSTANT, 0);
        addInterCodes(ics, newInterCode(SUB, place, zero, place1));
    }else if (!strcmp(root->child->name, "LP")){
        transExp(root->child->brother, place);
    }else if (root->child->brother && !strcmp(root->child->brother->name, "ASSIGNOP")){
        // place不存任何内容（事实上，传进来的place就应该为null，与415行对应）
        // 注意先翻译右侧表达式，再翻译左侧表达式，‘=’从右向左赋值。
        Operand place2 = newOperand(TEMP);
        transExp(root->child->brother->brother, place2);
        if (place2->kind == ADDRESS){
            Operand place3 = newOperand(TEMP);
            addInterCodes(ics, newInterCode(READ_AD, place2, place3));
            Operand place1 = newOperand(TEMP);
            transExp(root->child, place1);
            if (place1->kind == ADDRESS){
                addInterCodes(ics, newInterCode(WRITE_AD, place3, place1));
            }else{
                addInterCodes(ics, newInterCode(ASSIGN, place3, place1));
            }
        }else{
            Operand place1 = newOperand(TEMP);
            transExp(root->child, place1);
            if (place1->kind == ADDRESS){
                addInterCodes(ics, newInterCode(WRITE_AD, place2, place1));
            }else{
                addInterCodes(ics, newInterCode(ASSIGN, place2, place1));
            }
        }
    }else if (root->child->brother && !strcmp(root->child->brother->name, "PLUS")){
        Operand place1 = newOperand(TEMP);
        Operand place2 = newOperand(TEMP);
        transExp(root->child, place1);
        transExp(root->child->brother->brother, place2);
        addInterCodes(ics, newInterCode(ADD, place, place1, place2));
    }else if (root->child->brother && !strcmp(root->child->brother->name, "MINUS")){
        Operand place1 = newOperand(TEMP);
        Operand place2 = newOperand(TEMP);
        transExp(root->child, place1);
        transExp(root->child->brother->brother, place2);
        addInterCodes(ics, newInterCode(SUB, place, place1, place2));
    }else if (root->child->brother && !strcmp(root->child->brother->name, "STAR")){
        Operand place1 = newOperand(TEMP);
        Operand place2 = newOperand(TEMP);
        transExp(root->child, place1);
        transExp(root->child->brother->brother, place2);
        addInterCodes(ics, newInterCode(MUL, place, place1, place2));
    }else if (root->child->brother && !strcmp(root->child->brother->name, "DIV")){
        Operand place1 = newOperand(TEMP);
        Operand place2 = newOperand(TEMP);
        transExp(root->child, place1);
        transExp(root->child->brother->brother, place2);
        addInterCodes(ics, newInterCode(CODE_DIV, place, place1, place2));
    }else if (root->child->brother && !strcmp(root->child->brother->name, "LB")){
        // 翻译数组（可以是高维数组，执行后place保存目标元素的地址）
        tmp_no--;
        place->kind = ADDRESS;
        place->u.name = "addr";
        int offset = 0;
        int size = 1;
        Tree tr = root->child;
        while(!strcmp(tr->name, "Exp")){
            tr = tr->child;
        }
        TableItem symbol = getItem(table, tr->id);
        Type symType = symbol->type;
        Operand arrayAddr = newOperand(ADDRESS, newString(tr->id));
        addInterCodes(ics, newInterCode(GET_AD, arrayAddr, place));

        tr = root->child;
        while(!strcmp(tr->name, "Exp")){
            Operand temp = newOperand(TEMP);
            addInterCodes(ics, newInterCode(MUL, temp, newOperand(CONSTANT, tr->brother->brother->child->i), newOperand(CONSTANT, size)));
            // offset += size * (tr->brother->brother->i);
            addInterCodes(ics, newInterCode(ADD, place, place, temp));
            size *= (symType->u.array.size);
            symType = symType->u.array.eleType;
            tr = tr->child;
        }
    }else if (root->child->brother && !strcmp(root->child->brother->name, "LP")){
        // place存放返回值
        char * funcName = root->child->id;
        if (!strcmp(root->child->brother->brother->name, "Args")){ // 有参
            Operand argList[MAX_ARG_NUM] = {NULL};
            transArgs(root->child->brother->brother, argList, 0);
            
            int i = 0;
            while(argList[i]){
                addInterCodes(ics, newInterCode(ARG, argList[i]));
                i++;
            }
            if (!strcmp(funcName, "write")){
                addInterCodes(ics, newInterCode(WRITE, argList[0]));
                if(place) addInterCodes(ics, newInterCode(ASSIGN, newOperand(CONSTANT, 0), place));
                return;
            }
        }else{ // 无参
            if (!strcmp(funcName, "read")){
                addInterCodes(ics, newInterCode(READ, place));
                return;
            }
        }
        addInterCodes(ics, newInterCode(CALL, newOperand(OP_FUNC, newString(funcName)), place));
    }
}
void transCond(Tree root, Operand label_true, Operand label_false){
    if (!strcmp(root->child->name, "NOT")){
        transCond(root->child->brother, label_false, label_true);
    }else if (root->child->brother && !strcmp(root->child->brother->name, "RELOP")){
        Operand place1 = newOperand(TEMP);
        Operand place2 = newOperand(TEMP);
        transExp(root->child, place1);
        transExp(root->child->brother->brother, place2);
        Operand op = newOperand(OP_RELOP, newString(root->child->brother->id));
        // printf("%s\n", root->child->brother->id);
        addInterCodes(ics, newInterCode(CODE_IF, place1, op, place2, label_true));
        addInterCodes(ics, newInterCode(GOTO, label_false));
    }else if (root->child->brother && !strcmp(root->child->brother->name, "AND")){
        Operand label = newOperand(OP_LABEL);
        transCond(root->child, label, label_false);
        addInterCodes(ics, newInterCode(LABEL, label));
        transCond(root->child->brother->brother, label_true, label_false);
    }else if (root->child->brother && !strcmp(root->child->brother->name, "OR")){
        Operand label = newOperand(OP_LABEL);
        transCond(root->child, label_true, label);
        addInterCodes(ics, newInterCode(LABEL, label));
        transCond(root->child->brother->brother, label_true, label_false);
    }else{
        Operand place = newOperand(TEMP);
        Operand op = newOperand(OP_RELOP, newString("!="));
        Operand ins = newOperand(CONSTANT, 0);
        transExp(root, place);
        addInterCodes(ics, newInterCode(CODE_IF, place, op, ins, label_true));
        addInterCodes(ics, newInterCode(GOTO, label_false));
    }
}
void transArgs(Tree root, Operand* argList, int index){
    Operand place = newOperand(TEMP);
    transExp(root->child, place);
    argList[index] = place;
    if (root->child->brother){
        transArgs(root->child->brother->brother, argList, index + 1);
    }
}
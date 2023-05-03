#include "semantic.h"

Table table;
void Program(Tree root){ // the entry of semantic analyze
    // Program-> ExtDefList
    assert(root != NULL);
    ExtDefList(root->child);
}
void ExtDefList(Tree root){
    // ExtDefList-> ExtDef ExtDefList
    //           |  null  
    assert(root != NULL); 
    if (root->child){
        // printTable(table);
        ExtDef(root->child);
        // printTable(table);
        // printf("top: %d\n", table->top);
        ExtDefList(root->child->brother);
    }
}
void ExtDef(Tree root){
    // ExtDef-> Specifier ExtDecList SEMI
    //       |  Specifier SEMI
    //       |  Specifier FunDec CompSt     
    assert(root != NULL);
    Type type = Specifier(root->child);
    if (!strcmp(root->child->brother->name, "ExtDecList")){
        ExtDecList(root->child->brother, type);
    }else if (!strcmp(root->child->brother->name, "FunDec")){
        FunDec(root->child->brother, type);
        CompSt(root->child->brother->brother, type);
    }
    deleteType(type);
}

Type Specifier(Tree root){
    // Specifier-> TYPE
    //          |  StructSpecifier
    assert(root != NULL);
    if (!strcmp(root->child->name, "TYPE")){
        if (!strcmp(root->child->id, "float")){
            return newType(BASIC, 1, TYPE_FLOAT);
        }else if (!strcmp(root->child->id, "int")){
            return newType(BASIC, 1, TYPE_INT);
        }
    }else{
        return StructSpecifier(root->child);
    }
}
Type StructSpecifier(Tree root){ // insert an item(structure type defination)
    // StructSpecifier-> STRUCT OptTag LC DefList RC
    //                |  STRUCT Tag
    assert(root != NULL);
    TableItem ti;
    Type type = NULL;
    FieldList fl;
    char str[10] = {0};
    if (!strcmp(root->child->brother->name, "OptTag")){
        // printField(fl);
        fl = DefList(root->child->brother->brother->brother, 0);
        // printField(fl);
        if (isRedifined(fl)){ // error15
            char msg[100] = {0};
            sprintf(msg, "Redefined field .");
            printError(REDEF_FEILD, root->line, msg);
        }
        if (root->child->brother->child){
            type = newType(STRUCTURE, 3, newString(root->child->brother->child->id), 
                            countField(fl), copyField(fl));
        }else{ // anonymity
            sprintf(str, "%d", table->anonymity);
            type = newType(STRUCTURE, 3, newString(str),
                            countField(fl), copyField(fl));
            (table->anonymity)++;
        }
        deleteField(fl);
        ti = newTableItem(table->top, newString(type->u.structure.name),
                            type, NULL, NULL);
        
        // insert
        if (isdefined(table, ti)){ // error16
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", ti->name);
            printError(REDEF_STRUCT, root->line, msg);
            deleteItem(ti);
        }else{
            addItem(table, ti);
        }
    }else if (!strcmp(root->child->brother->name, "Tag")){
        // find
        char * name = root->child->brother->child->id;
        if (!getItem(table, name)){ // error17
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", name);
            printError(UNDEF_STRUCT, root->line, msg);
        }else{
            type = getItem(table, name)->type;
        }
    }

    return copyType(type);
}

void ExtDecList(Tree root, Type type){
    // ExtDecList-> VarDec
    //           |  VarDec COMMA ExtDecList
    assert(root);
    //
    VarDec(root->child, type, 1);
    if (root->child->brother){
        ExtDecList(root->child->brother->brother, type);
    }
}
/* 返回插入的(或不必插入)符号， 若重定义返回null*/
TableItem VarDec(Tree root, Type type, int flag){ // insert an item(varity defination)
    // VarDec-> ID
    //       |  VarDec LB INT RB
    assert(root);
    
    TableItem ti = NULL;
    if (!strcmp(root->child->name, "ID")){
        ti = newTableItem(table->top, newString(root->child->id),
                    copyType(type), NULL, NULL);
    }else if(!strcmp(root->child->name, "VarDec")){
        int counter = 0;
        Tree temp = root->child;
        while(!strcmp(temp->name, "VarDec")){
            counter++;
            temp = temp->child;
        }
        char *name = temp->id;
        Type arrayType = newType(ARRAY, 2, copyType(type), counter);
        ti = newTableItem(table->top, newString(name), arrayType, NULL, NULL);
    }
    
    // insert
    // printItem(ti);
    if (flag){ // 对于结构体内部变量声明的语句，不加入符号表，加入fieldList存在结构体Type中
        if (isdefined(table, ti)){ // error3
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", ti->name);
            printError(REDEF_VAR, root->line, msg);
            deleteItem(ti);
            return NULL;
        }else{
            addItem(table, ti);
            return ti;
        }
    }
    return ti;
}

void FunDec(Tree root, Type type){ // insert an item(function defination)
    // FunDec-> ID LP VarList RP
    //       |  ID LP RP
    assert(root);
    TableItem ti;
    char* name = newString(root->child->id);
    Type funcType;
    if (!strcmp(root->child->brother->brother->name, "VarList")){
        // 函数参数进符号表， stack需要提前加1，最后需要恢复。
        (table->top)++;
        FieldList fl = VarList(root->child->brother->brother);
        (table->top)--;
        // if (isRedifined(fl)){ // error3
        //     char msg[100] = {0};
        //     sprintf(msg, "Redefined Variable \"%s\".", ti->name);
        //     printError(REDEF_VAR, root->line, msg);
        // }
        funcType = newType(FUNCTION, 3, countField(fl), copyType(type), copyField(fl));
        deleteField(fl);
    }else{
        funcType = newType(FUNCTION, 3, 0, copyType(type), NULL);
    }
    ti = newTableItem(table->top, name, funcType, NULL, NULL);
    // printItem(ti);
    // insert
    if (isdefined(table, ti)){ // error4
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", ti->name);
        printError(REDEF_FUNC, root->line, msg);
    }else{
        addItem(table, ti);
    }
}
FieldList VarList(Tree root){
    // VarList-> ParamDec COMMA VarList
    //        |  ParamDec
    assert(root);
    FieldList fi = ParamDec(root->child);
    if (root->child->brother){
        if (fi){
            fi->next = VarList(root->child->brother->brother);
        }
        else{
            fi = VarList(root->child->brother->brother);
        }
    }
    return fi;
}
FieldList ParamDec(Tree root){ // insert params
    // ParamDec-> Specifier VarDec
    assert(root);
    Type type = Specifier(root->child);
    TableItem ti = VarDec(root->child->brother, type, 2);
    if(!ti) return NULL;
    FieldList fi = newField(newString(ti->name), copyType(type), NULL);
    deleteType(type);
    return fi;
}

void CompSt(Tree root, Type retType){
    // CompSt-> LC DefList StmtList RC
    assert(root);
    (table->top)++;

    // printf("compst top: %d\n", table->top);
    //
    DefList(root->child->brother, 1);
    StmtList(root->child->brother->brother, retType);
 
    clearStackTop(table);
    (table->top)--;
}

FieldList DefList(Tree root, int flag){ // flag 用来判断是不是结构体类型定义 0表示是结构体类型定义
    // DefList-> Def DefList
    //        |  null
    assert(root != NULL);
    FieldList fi = NULL; 
    if (root->child){
        // printField(fi);
        fi = Def(root->child, flag);
        // printField(fi);
        if (fi){
            fi->next = DefList(root->child->brother, flag);
        }else{
            fi = DefList(root->child->brother, flag);
        }
    }
    // printField(fi);
    return fi;
}
FieldList Def(Tree root, int flag){
    // Def-> Specifier DecList SEMI
    assert(root);
    FieldList fi;
    Type type = Specifier(root->child);
    // printField(fi);
    fi = DecList(root->child->brother, type, flag);
    deleteType(type);
    // printField(fi);
    return fi;
}
FieldList DecList(Tree root, Type type, int flag){
    // DecList-> Dec
    //        |  Dec COMMA DecList
    assert(root);
    FieldList fi = Dec(root->child, type, flag);
    if(root->child->brother){
        if (fi){
            fi->next = DecList(root->child->brother->brother, type, flag);
        }else{
            fi = DecList(root->child->brother->brother, type, flag);
        }
    }
    return fi;
}
FieldList Dec(Tree root, Type type, int flag){
    // Dec-> VarDec
    //    |  VarDec ASSIGNOP Exp
    assert(root);
    TableItem ti = VarDec(root->child, type, flag);
    // printItem(ti);
    if (!ti) return NULL;

    if(root->child->brother){
        // TODO 把值存在对应符号 
        if (!flag){ // error18 结构体类型定义内赋值
            char msg[100] = {0};
            sprintf(msg, "Other.");
            printError(OTHER, root->line, msg);
        }
        Type expType = Exp(root->child->brother->brother);
        if (!checkType(type, expType)){ // error5
            char msg[100] = {0};
            sprintf(msg, "Type mismatched for assignment.");
            printError(TYPE_MISMATCH_ASSIGN, root->line, msg);
        }
        deleteType(expType);
    }
    FieldList fi = newField(newString(ti->name), copyType(type), NULL);
    return fi;
}

void StmtList(Tree root, Type retType){
    // StmtList-> Stmt StmtList
    //         |  null
    assert(root);
    if (root->child){
        Stmt(root->child, retType);
        StmtList(root->child->brother, retType);
    }
}
void Stmt(Tree root, Type retType){ // check the return type
    // Stmt-> Exp SEMI
    //     |  CompSt
    //     |  RETURN Exp ESMI
    //     |  IF LP Exp RP Stmt
    //     |  IF LP Exp RP Stmt ELSE Stmt
    //     |  WHILE LP Exp RP Stmt
    assert(root);
    if (!strcmp(root->child->name, "Exp")){
        //
        Exp(root->child);
    }else if(!strcmp(root->child->name, "CompSt")){
        CompSt(root->child, retType);
    }else if (!strcmp(root->child->name, "RETURN")){
        Type expType = Exp(root->child->brother);
        if (!checkType(retType, expType)){ // error8
            char msg[100] = {0};
            sprintf(msg, "Type mismatched for return.");
            printError(TYPE_MISMATCH_RETURN, root->line, msg);
        }
        deleteType(expType);
    }else{
        //
        Exp(root->child->brother->brother);
        Stmt(root->child->brother->brother->brother->brother, retType);
        if(root->child->brother->brother->brother->brother->brother){
            Stmt(root->child->brother->brother->brother->brother->brother->brother, retType);
        }
    }
}
Type Exp(Tree root){
    // Exp -> Exp ASSIGNOP Exp  error5 6

    //     | Exp AND Exp        error7
    //     | Exp OR Exp         error7
    //     | Exp RELOP Exp      error7
    //     | Exp PLUS Exp       error7
    //     | Exp MINUS Exp      error7
    //     | Exp STAR Exp       error7
    //     | Exp DIV Exp        error7

    //     | LP Exp RP
    //     | MINUS Exp
    //     | NOT Exp            

    //     | ID LP Args RP      error2* 9* 11*
    //     | ID LP RP           error2* 11*
    //     | Exp LB Exp RB      error10* 12*
    //     | Exp DOT ID         error13* 14*

    //     | ID                 error1* 2*
    //     | INT
    //     | FLOAT
    assert(root);
    Type type;
    if (!strcmp(root->child->name, "ID") && !root->child->brother){
        char* name = root->child->id;
        TableItem ti = getItem(table, name);
        if (!ti){ // error1 2
            char msg[100] = {0};
            sprintf(msg, "Undefined Variable \"%s\"", name);
            printError(UNDEF_VAR, root->line, msg);
            return NULL;
        }
        return copyType(ti->type);
    }else if (!strcmp(root->child->name, "INT")){
        type = newType(BASIC, 1, TYPE_INT);
        type->rightValueFlag = 1;
        return type;
    }else if (!strcmp(root->child->name, "FLOAT")){
        type = newType(BASIC, 1, TYPE_FLOAT);
        type->rightValueFlag = 1;
        return type;
    }else if (root->child->brother){
        if (!strcmp(root->child->brother->name, "ASSIGNOP")){
            Type expTypeLeft = Exp(root->child);
            // assert(expTypeLeft);
            // printType(expTypeLeft);
            if (expTypeLeft && expTypeLeft->rightValueFlag){ // error6
                char msg[100] = {0};
                sprintf(msg, "The left-hand side of an assignment must be a variable.");
                printError(LEFT_VAR_ASSIGN, root->line, msg);
                return NULL;
            }

            Type expTypeRight = Exp(root->child->brother->brother);
            if (!checkType(expTypeLeft, expTypeRight)){ // error5
                char msg[100] = {0};
                sprintf(msg, "Type mismatchedfor assignment.");
                printError(TYPE_MISMATCH_ASSIGN, root->line, msg);
                return NULL;
            }
            
            return copyType(expTypeLeft);
        }else if(!strcmp(root->child->brother->name, "LP")){
            char* name = root->child->id;
            TableItem ti = getItem(table, name);
            if (!ti){ // error1 2
                char msg[100] = {0};
                sprintf(msg, "Undefined Function \"%s\"", name);
                printError(UNDEF_FUNC, root->line, msg);
                return NULL;
            }
            if (ti->type->kind != FUNCTION){ // error11
                char msg[100] = {0};
                sprintf(msg, "Variable is not a Function.");
                printError(NOT_A_FUNC, root->line, msg);
                return NULL;
            }
            if (!strcmp(root->child->brother->brother->name, "Args")){
                Args(root->child->brother->brother, ti);
            }
            return copyType(ti->type->u.func.retType);
        }else if(!strcmp(root->child->brother->name, "LB")){
            type = Exp(root->child);
            if (type->kind != ARRAY){ // error10
                char msg[100] = {0};
                sprintf(msg, "Variable is not a Array.");
                printError(NOT_A_ARRAY, root->line, msg);
                return NULL;
            }
            Type indexType = Exp(root->child->brother->brother);
            if (indexType->kind != BASIC || indexType->u.basic != TYPE_INT){ // error12
                char msg[100] = {0};
                sprintf(msg, "Variable is not a Integer.");
                printError(NOT_A_INT, root->line, msg);
                return NULL;
            }
            return copyType(type->u.array.eleType);
        }else if(!strcmp(root->child->brother->name, "DOT")){
            type = Exp(root->child);
            if (type->kind != STRUCTURE){ // error13
                char msg[100] = {0};
                sprintf(msg, "Illegal use of \".\". ");
                printError(ILLEGAL_USE_DOT, root->line, msg);
                return NULL;
            }
            Type fieldType = containField(type->u.structure.field, root->child->brother->brother->id);
            if(!fieldType){ // error14
                char msg[100] = {0};
                sprintf(msg, "Non-existentfield \"%s\".", root->child->brother->brother->id);
                printError(NONEXISTFIELD, root->line, msg);
                return NULL;
            }
            return copyType(fieldType);
        }else if(!strcmp(root->child->brother->name, "Exp")){
            type = Exp(root->child->brother);
            if (strcmp(root->child->name, "LP") && type->kind != BASIC){ // error18
                char msg[100] = {0};
                sprintf(msg, "Others.");
                printError(OTHER, root->line, msg);
                return NULL;
            }
            return copyType(type);
        }else{
            Type expTypeLeft = Exp(root->child);
            // assert(expTypeLeft);
            Type expTypeRight = Exp(root->child->brother->brother);
            if (!expTypeLeft || !expTypeRight) return NULL;
            if (!checkType(expTypeLeft, expTypeRight)
                || expTypeLeft->kind != BASIC){ // error7
                char msg[100] = {0};
                sprintf(msg, "Type mismatched for operands.");
                printError(TYPE_MISMATCH_OP, root->line, msg);
                return NULL;
            }
            return copyType(expTypeLeft);
        }
    }
}
void Args(Tree root, TableItem funcInfo){
    // Args-> Exp COMMA Args
    //     |  Exp
    assert(root);
    int counter = 1;
    Tree temp = root;
    while (temp->child->brother){
        counter++;
        temp = temp->child->brother->brother;
    }
    if(funcInfo->type->u.func.argc != counter){ //error9
        char msg[100] = {0};
        sprintf(msg, "Function is not applicable for arguments.");
        printError(FUNC_AGRC_MISMATCH, root->line, msg);
    }
    FieldList fl = funcInfo->type->u.func.field;
    Tree tr = root;
    Type t = Exp(tr->child);
    while (fl){
        if (!checkType(fl->fieldType, t)){ // error9
            char msg[100] = {0};
            sprintf(msg, "Function is not applicable for arguments.");
            printError(FUNC_AGRC_MISMATCH, root->line, msg);
        }
        fl = fl->next;
        tr = tr->child->brother->brother;
        t = Exp(tr->child);
    }
}

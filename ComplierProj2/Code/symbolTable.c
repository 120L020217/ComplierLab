#include "symbolTable.h"

// Type
Type newType(Kind kind, int argc, ...){
    assert(kind == BASIC || kind == ARRAY 
    || kind == STRUCTURE || kind == FUNCTION);
    Type t = (Type)malloc(sizeof(struct _Type));
    assert(t != NULL);
    t->kind = kind;
    va_list vaList;
    switch (kind)
    {
    case BASIC:
        va_start(vaList, argc);
        t->u.basic = va_arg(vaList, BasicType);
        break;
    case ARRAY:
        va_start(vaList, argc);
        t->u.array.eleType = va_arg(vaList, Type);
        t->u.array.size = va_arg(vaList, int);
        break;
    case STRUCTURE:
        va_start(vaList, argc);
        t->u.structure.name = newString(va_arg(vaList, char *));
        t->u.structure.num = va_arg(vaList, int);
        t->u.structure.field = va_arg(vaList, FieldList);
        break;
    case FUNCTION:
        va_start(vaList, argc);
        t->u.func.argc = va_arg(vaList, int);
        t->u.func.retType = va_arg(vaList, Type);
        t->u.func.field = va_arg(vaList, FieldList);
        break;
    default:
        break;
    }
    va_end(vaList);
    return t;
}
Type copyType(Type src){ // deep copy
    if (src == NULL) return NULL;
    Type t = (Type)malloc(sizeof(struct _Type));
    assert(t != NULL);
    t->kind = src->kind;
    switch (t->kind)        
    {
    case BASIC :
        t->u.basic = src->u.basic;
        break;
    case ARRAY:
        t->u.array.eleType = copyType(src->u.array.eleType);
        t->u.array.size = src->u.array.size;
        break;
    case STRUCTURE:
        t->u.structure.name = newString(src->u.structure.name);
        t->u.structure.num = src->u.structure.num;
        t->u.structure.field = copyField(src->u.structure.field);
        break;
    case FUNCTION:
        t->u.func.argc = src->u.func.argc;
        t->u.func.retType = copyType(src->u.func.retType);
        t->u.func.field = copyField(src->u.func.field);
        break;
    default:
        break;
    }
    return t;
}
void deleteType(Type type){
    if (type == NULL) return;
    switch (type->kind) 
    {
    case BASIC:
        break;
    case ARRAY:
        deleteType(type->u.array.eleType);
        type->u.array.eleType = NULL;
        break;
    case STRUCTURE:
        free(type->u.structure.name);
        type->u.structure.name = NULL;
        FieldList temp = type->u.structure.field;
        while(temp){
            FieldList del = temp;
            temp = temp->next;
            deleteField(del);
        }
        type->u.structure.field = NULL;
        break;
    case FUNCTION:
        deleteType(type->u.func.retType);
        type->u.func.retType = NULL;
        FieldList temp1 = type->u.func.field;
        while(temp1){
            FieldList del = temp1;
            temp1 = temp1->next;
            deleteField(del);
        }
        type->u.func.field = NULL;
        break;
    default:
        break;
    }
    free(type);
}
// false-->different true-->same
bool checkType(Type type1, Type type2){
    // printType(type1);
    // printType(type2);
    if(type1 == NULL || type2 == NULL) return true;
    assert(type1->kind != FUNCTION && type2->kind != FUNCTION);
    if (type1->kind != type2->kind){
        return false;
    }else{
        switch (type1->kind)
        {
        case BASIC:
            return type1->u.basic == type2->u.basic;
            break;
        case ARRAY:
            return type1->u.array.size == type2->u.array.size
                && checkType(type1->u.array.eleType, type2->u.array.eleType);
            break;
        case STRUCTURE:
            if (type1->u.structure.num != type2->u.structure.num){
                return false;
            }
            FieldList temp1 = type1->u.structure.field;
            FieldList temp2 = type2->u.structure.field;
            while(temp1 != NULL){
                if (!checkType(temp1->fieldType, temp2->fieldType))
                    return false;
                temp1 = temp1->next;
                temp2 = temp2->next;
            }
            return true;
            break;
        default:
            break;
        }
    }
    return true;
}
void printType(Type type){
    printf("Type: %d ", type->kind);
    if (type->kind == BASIC){
        printf("BasicType: %d ", type->u.basic);
    }
}

// FieldList
FieldList newField(char * name, Type type, FieldList next){
    FieldList fl = (FieldList)malloc(sizeof(struct _FieldList));
    assert(fl != NULL);
    fl->name = newString(name); /*此处新开辟一段空间存name，
                                是因为不想在delete fieldlist时，
                                释放原来存放name的空间*/
    fl->fieldType = type;
    fl->next = next;
    return fl;
}
FieldList copyField(FieldList src){ // 就是复制链表
    if (src == NULL) return NULL;
    FieldList head = NULL, cur = NULL;
    FieldList temp = src;
    while (temp) {
        if (!head) {
            head = newField(temp->name, copyType(temp->fieldType), NULL);
            cur = head;
            temp = temp->next;
        } else {
            cur->next = newField(temp->name, copyType(temp->fieldType), NULL);
            cur = cur->next;
            temp = temp->next;
        }
    }
    return head;
}
void deleteField(FieldList field){ // 删除一个结点
    if (field == NULL) return; 
    free(field->name);
    field->name = NULL;
    deleteType(field->fieldType);
    field->fieldType = NULL;
    free(field);
}
int countField(FieldList field){
    int counter = 0;
    FieldList temp = field;
    while(temp){
        counter++;
        temp = temp->next;
    }
    return counter;
}
bool isRedifined(FieldList field){ // 暴力检查是否有重复
    if (field == NULL) return false;
    FieldList stake = field;
    FieldList temp = field->next;
    while(stake){
        temp = stake->next;
        while(temp){
            if (!strcmp(stake->name, temp->name))
                return true;
            temp = temp->next;
        }
        stake = stake->next;
    }
    return false;
}
Type containField(FieldList field, char *name){
    FieldList temp = field;
    while(temp){
        if(!strcmp(temp->name, name)){
            return temp->fieldType;
        }
        temp = temp->next;
    }
    return NULL;
}
void printField(FieldList field){
    FieldList temp = field;
    while(temp){
        printType(temp->fieldType);
        temp = temp->next;
    }
}

// TableItem
TableItem newTableItem(int depth, char* name, Type type, TableItem hn, TableItem sn){
    TableItem ti = (TableItem)malloc(sizeof(struct _TableItem));
    assert(ti != NULL);
    ti->depth = depth;
    ti->name = newString(name);
    ti->type = type;
    ti->hashNext = hn;
    ti->stackNext = sn;
    return ti;
}
void deleteItem(TableItem item){
    if (item == NULL) return;
    free(item->name);
    deleteType(item->type);
    free(item);
}
void printItem(TableItem item){
    printf("Item: %s\n", item->name);
}

// SymbolTable
Table newTable(){
    // 建立伪头部，方便链表增删
    Table t = (Table)malloc(sizeof(struct _Table));
    assert(t != NULL);
    for (int i = 0; i < HASHSIZE; i++){
        t->hash[i] = newTableItem(-1, "", NULL, NULL, NULL);
    }
    for (int i = 0; i < STACKSIZE; i++){
        t->stack[i] = newTableItem(i, "", NULL, NULL, NULL);
    }
    t->top = 0;
    t->anonymity = 0;
    return t;
}
void deleteTable(Table table){
    for (int i = 0; i < HASHSIZE; i++){
        TableItem temp = table->hash[i];
        while(temp){
            TableItem del = temp;
            temp = temp->hashNext;
            deleteItem(del);
        }
        table->hash[i] = NULL;
    }
    for(int i = 0; i < STACKSIZE; i++){
        deleteItem(table->stack[i]);
        table->stack[i] = NULL;
    }
    free(table);
}
/* 返回距当前最近的定义 没找到返回null*/
TableItem getItem(Table table, const char* name){
    unsigned int hashcode = hashCode(name);
    TableItem ti = table->hash[hashcode];
    // printItem(ti);
    while(ti){
        // printf("%s %s\n", ti->name, name);
        if (!strcmp(ti->name, name)) {
            // printf("3\n");
            return ti;
        }
        ti = ti->hashNext;
    }
    return NULL;
}
bool isdefined(Table table, TableItem item){
    assert(item != NULL);
    assert(item->name != NULL);
    TableItem ti = getItem(table, item->name);
    if (ti == NULL) return false;
    // 同名同作用域的符号才是重定义的
    if (ti->depth == item->depth) return true;
    return false;
}
void addItem(Table table, TableItem item){
    assert(table != NULL && item != NULL);
    unsigned int hashcode = hashCode(item->name);
    TableItem ti = table->hash[hashcode];
    item->hashNext = ti->hashNext;
    ti->hashNext = item;
    TableItem tis = table->stack[item->depth];
    item->stackNext = tis->stackNext;
    tis->stackNext = item;
}
void removeItem(Table table, TableItem item){
    assert(table != NULL && item != NULL);
    unsigned int hashcode = hashCode(item->name);
    TableItem last = table->hash[hashcode];
    TableItem cur = last->hashNext;
    assert(cur != NULL);
    while(cur != item){
        last = cur;
        cur = cur->hashNext;
    }
    last->hashNext = cur->hashNext;
    deleteItem(item);
}
void clearStackTop(Table table){
    TableItem temp = table->stack[table->top];
    while(temp->stackNext){
        TableItem del = temp->stackNext;
        temp->stackNext = del->stackNext;
        removeItem(table, del);
    }
    // (table->top)--;
}
void printTable(Table table){
    for(int i = 0; i < STACKSIZE; i++){
        TableItem ti = table->stack[i];
        printf("stack(%d): \n", i);
        while (ti->stackNext)
        {
            /* code */
            
            printItem(ti->stackNext);
            ti = ti->stackNext;
        }
    }
}
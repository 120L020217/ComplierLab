# 一、一些假设

1. 只有常规整型变量
2. 无结构体、只有一维数组
3. 函数参数都是简单变量（整型）
4. 函数返回均为简单变量
5. 无函数声明
6. 无词法语法语义错误
7. 无全局变量、无作用域的概念、变量均不重名

# 二、TODO

## 在符号表中加read（）write（）以实现与控制台接口

# 三、一些debug

## 1.  
在 C 语言中，字符串常量（例如："this is dest" 和 "this is source"）通常存储在常量存储区（也称为只读存储区或静态存储区）中。这些字符串常量是在编译时分配的，并且在程序的整个执行期间保持不变。

然而，需要注意的是，`strcat` 函数的第一个参数必须是一个指向目标字符串的字符数组，而不是一个指向字符串常量的指针。如果目标字符串是一个字符数组，则它可以存储在栈上或静态存储区上。

以下是一个示例，说明如何使用 `strcat` 函数将两个字符串连接到目标字符串：

```
#include <stdio.h>
#include <string.h>

int main() {
    char dest[50] = "this is dest";
    const char* source = "this is source";

    strcat(dest, source);

    printf("Concatenated string: %s\n", dest);

    return 0;
}
```

在上面的示例中，`dest` 是一个字符数组，存储在栈上。`source` 是一个指向字符串常量的指针，指向常量存储区的字符串。`strcat` 函数将 `source` 中的内容追加到 `dest` 中。

请注意，在使用 `strcat` 函数时，确保目标字符串的缓冲区足够大，以容纳源字符串的内容和最终的连接结果。否则，可能会导致缓冲区溢出和未定义行为。

## 2. 一个有趣的bug
对于Exp->INT | ID，我尝试把Exp的继承属性place，由原来的临时变量，更换为符号表中的变量or立即数，来优化代码，减少一些冗余的赋值，和不必要的寄存器开销。因此，有如下代码：
```
    if (!strcmp(root->child->name, "INT")){
        if (place){
            tmp_no--;
            deleteOperand(place);
            place = newOperand(CONSTANT, root->child->i);
        }
    }else if (!strcmp(root->child->name, "ID") && !(root->child->brother)){
        if (place){
            tmp_no--;
            deleteOperand(place);
            // printf("%s\n", root->child->id);
            // place = newOperand(CONSTANT, 1);
            place = newOperand(VARIABLE, newString(root->child->id));
        }
    }
```
然而，我的测试用例出现了这样的结果：
```
FUNC fact :
PARAM n
IF ==#1 GOTO label0 (注意这里条件表达式的翻译)
GOTO label1
LABLE label0 :
RETURN 
GOTO label2
LABLE label1 :
RETURN t0
```
INT正确翻译（其实是巧合），ID却是一个空串。经过一些debug，我分析原因在实参形参的传递上，因为形参是实参的副本，所以改变形参的指向，实参的指向并未改变，然而实参指向的内存却实实在在的被释放了。至于为什么INT却能“正确”翻译，大概是newOperand时刚好利用了刚被释放的内存空间。
## 3. 对实验2中符号表操作的更改
因为我们采取独立的方式做实验三（为了整体模块更清晰），而不是一边建符号表一边做代码翻译，而是遍历一次语法树建表，再遍历一次语法树做翻译。实验二因为有选做不同作用域的符号表，所以一个函数内定义的符号随函数检查完毕而被清空。我们实验三翻译数组时需要用到符号表中的信息，因为语义检查后函数的符号表被清掉，代码翻译时查不到我们需要的信息，所以我在semantic部分注释掉函数检查完清符号表的动作。
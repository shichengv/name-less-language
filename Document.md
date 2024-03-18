# name-less language 参考

这篇用来介绍 name-less 语言的一些特性。

## 特性

### 代码块

&emsp; **"\{ }"** 包裹的代码块具有专属的局部环境。**"\{ }"** 也代表一个表达式序列，类似于Scheme语言的 (begin exp1 exp2 ...)。表达式序列的返回值为最后一个表达式的值。当前局部环境中所有的临时常量数据(包括返回值)都被存储在其父环境的临时数据回收器中。当一个环境被清理时，它的临时数据回收器会清理所有的临时常量。当一个代码块返回的值为 lambda 类型时，并且，该lambda类型的函数所在的环境为当前的环境，那么该环境将会被延迟回收。如果，该lambda类型的函数所在的环境被添加到了当前的环境的待回收环境序列中，该环境也不会被删除。除此之外，该环境在表达式序列执行完毕后由解释器自动回收。

### 条件分支

&emsp; if 语句也被视为一句表达式，它的返回值为最后执行的一个表达式的结果。如果条件成立，那么它返回 "then" 表达式序列的最后一个值，否则返回 "else" 表达式序列。解析分支条件时并不检测它的条件结果类型，只判断他的位向量是否为0。对于 false 或 nil 类型，它们的位向量始终为 0，所以采取 else分支。由于数字是由double类型存储，所以即使函数返回值为0(数字0)，条件也是成立的，仍然采取 then 分支。
&emsp; then关键字的作用是用来忽略条件表达式的括号，如果为条件表达式增加括号，则必须省略then关键字，所以对于如下语法，if语句也是成立的（奇丑无比），建议使用统一的风格。

```
if (a > 5)
	a = 3;
else if a < 2 then
	a = 9;
else if a > 9
{
	a = a + 13;
}
else
	a = 0;
```

### def 关键字

def 关键字用来定义一个变量，在name-less中，变量在使用时不需要事先声明，所以def在声明一个变量时基本上等同于 " = " [赋值语句]。def 的出现时为了使定义变量或者函数的时候更清晰，它等同于下面的语法：

def 用来强调声明，` = ` 强调赋值，或者更改，类似于 `set!`。

```
# 定义 x 变量为 5
def x 5;
# 定义sum函数，接受两个参数，返回它们的和
def sum(x, y) x + y;
```

```
x = 5;
sum = λ(x, y) x + y;
```
上面两种写法完全相同。

### nil (空类型)

`nil` 代表一个空类型的值。nil 的位向量为0。

事实上，nil并不作为关键字，它只是一个值，类型标志为 `VALUE_IS_NIL`，它的位向量为0。在全局变量初始化时被加入到全局变量环境中。

### 序对

**pair**

`pair` 代表一个序对，它拥有首项和尾项，可以通过内置的一些方法来操作该序对。
```
# 构造一个新的序对，该序对的首项为"Skadi", 尾项为 nil。
skadi = ("Skadi", nil); 
```

**list**

`list` 仅仅是 `pair` 的语法糖。

```
# 解释器会自动为list的末尾添加 nil
characters = list("Skadi", "Orcinus orca", "Kal'tsit", "Lynx lynx");
characters = pair("Skadi", pair("Orcinus orca", pair("Kal'tsit", pair("Lynx lynx", nil))));
```

## 库函数

***

**`pair`**

```
pair(front-value, rear-value);
```
例程描述：
&emsp; 构造一个新的序对，返回新构造的序对。

参数：
 - `front-value`: 首项的值。
 - `rear-value`: 尾项的值。

返回值：
&emsp; 新构造的序对。

***

**`set-car!`**

```
set-car!( &pair, value );
```

例程描述：
&emsp; 设置序对首项的值

参数：
  - `&pair`: 一个序对结构的引用。
  - `value`: 序对首项的新值

返回值：
&emsp; 返回一个bool值，永远为true。

***

**`set-cdr!`**

```
set-cdr!( &pair, value );
```

例程描述：
&emsp; 设置序对尾项的值

参数：
  - `&pair`: 一个序对结构的引用。
  - `value`: 序对尾项的新值

返回值：
&emsp; 返回一个bool值，永远为true。
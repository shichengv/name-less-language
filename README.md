<p align="center">
  <img src="./img/lambda.png" />
</p>

# <center> name-less language

***

<center>仿Scheme语言，拥有极其糟糕的性能和阴间的语法</center>

***


## 介绍

**name-less** 语言作为一门简单的"函数式"语言，C语言语法风格的Scheme语言(bushi)。
在 **name-less** 的世界中，代码被视为以 ';' 分隔的表达式序列。表达式都将返回它自己值。 

**name-less**语言使用 `.nl` 作为代码源文件的扩展名。下面是 **name-less** 的hello, world代码示例:

```
# hello.nl
print("hello, world\n");	# 打印hello, world
```

它拥有非常少的关键字，部分关键字只是作为语法糖，并不影响该语言的功能。
```
if then else lambda λ true false def list nil
```

## 构建

请通过更改 `name-less-language/lambda/framework.h` 来选择构建目标平台(默认构建Linux平台)。

**Windows**
请使用Visual Studio编译调试。

**Linux**
使用下列命令来构建解释器程序，解释器程序将存放在 `../../build` 文件夹。
```
git clone https://github.com/shichengv/name-less-language
cd name-less-language/lambda
make install
cd ../../build
ls
```

## 教程

[参考手册](./Document.md)
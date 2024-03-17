# 1. 赋值测试
# b = -1;
# a = 6 + 7;
# 
# # 2. 条件测试
# a = 3;
# b = 5;
# if a > b then
# 	c = 1;
# else if a == b then
# 	c = 0;
# else
# 	c = 4;

# 3. Object-Oriented Programming
# def make-account (balance) {
# 	# 提款
# 	def withdraw(amount){
# 		if (balance >= amount) {
# 			balance = balance - amount;
#             balance;
#         }
# 	}
# 
# 	# 存款
# 	def deposit(amount) {
# 		balance = balance + amount;
# 		balance;
# 	}
# 
# 	# 分发器，选择需要使用的方法
# 	def dispatch(m) {
# 		if (m == "withdraw")
# 			withdraw;
# 		else if (m == "deposit")
# 			deposit;
# 	}
# 
# 	# 返回这个分发器函数，这样解析器检测到返回是一个函数，就不会删除该局部环境
# 	dispatch;
# }
# 
# # 新建一个 account1 账户对象，余额 500
# def account1 make-account(500);
# def account2 make-account(500);
# 
# 
# # 一种语法糖风格，用来操作account对象的方法
# def withdraw(account, amount) {
# 	# 局部环境
# 	def withdraw account("withdraw");
# 	withdraw(amount);
# }
# def deposit(account, amount) {
# 	# 局部环境
# 	def deposit account("deposit");
# 	deposit(amount);
# }
# 
# withdraw(account1, 114);
# deposit(account1, 514);

# 4. 递归和迭代

#	递归计算阶乘
# factorial_recursion = λ(n) if n == 1 then 1; else n * factorial_recursion( n - 1 );

#	迭代计算阶乘
# factorial_iteration = λ(n) fact-iter(1, 1, n);
# fact-iter = λ(product, counter, max-count) if counter > max-count then product; else fact-iter( counter * product, counter + 1, max-count);

# print(factorial_recursion(6) + "\n");
# print(factorial_iteration(6) + "\n");

print("hello, world\n");
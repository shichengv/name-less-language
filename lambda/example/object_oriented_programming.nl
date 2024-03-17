# syntactic sugar, 实际上make-account仍然对应一个lambda表达式
def make-account (balance) {
	# 提款
	def withdraw(amount) {
		if (balance >= amount)
			balance = balance - amount;
		else
			print("Insufficient funds");
	}
	# 存款
	def deposit(amount) {
		balance = balance + amount;
	}
	# 分发器，选择需要使用的方法
	def dispatch(m) {
		if (m == "withdraw")
			withdraw;
		else if (m == "deposit")
			deposit;
		else
			print("Unknown request -- MAKE-ACCOUNT");
	}
	# 返回这个分发器函数，这样解析器检测到返回是一个函数，就不会删除该局部环境
	dispatch;
}

# 新建一个 account1 账户，余额 500
print("新建一个 account1 账户，余额 500$\n");
def account1 make-account(500);
# 新建一个 account2 账户，余额 777777 
print("新建一个 account2 账户，余额 777777$\n");
def account2 make-account(777777);

# 一种语法糖风格，用来操作account对象的方法
def withdraw(account, amount) {
	# 局部环境
	def withdraw account("withdraw");
	withdraw(amount);
}
def deposit(account, amount) {
	# 局部环境
	def deposit account("deposit");
	deposit(amount);
}

print("account1: \n");
print("\t提款 114，account1 的余额为：");
print(withdraw(account1, 114));
print("$\n");
print("\t存款 514，account1 的余额为：");
print(deposit(account1, 514));
print("$\n");

print("account2: \n");
print("\t提款 123456，account2 的余额为：");
print(withdraw(account2, 123456));
print("$\n");
print("\t存款 888，account2 的余额为：");
print(deposit(account2, 888));
print("$\n");

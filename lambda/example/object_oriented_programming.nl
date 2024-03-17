# syntactic sugar, ʵ����make-account��Ȼ��Ӧһ��lambda���ʽ
def make-account (balance) {
	# ���
	def withdraw(amount) {
		if (balance >= amount)
			balance = balance - amount;
		else
			print("Insufficient funds");
	}
	# ���
	def deposit(amount) {
		balance = balance + amount;
	}
	# �ַ�����ѡ����Ҫʹ�õķ���
	def dispatch(m) {
		if (m == "withdraw")
			withdraw;
		else if (m == "deposit")
			deposit;
		else
			print("Unknown request -- MAKE-ACCOUNT");
	}
	# ��������ַ���������������������⵽������һ���������Ͳ���ɾ���þֲ�����
	dispatch;
}

# �½�һ�� account1 �˻������ 500
print("�½�һ�� account1 �˻������ 500$\n");
def account1 make-account(500);
# �½�һ�� account2 �˻������ 777777 
print("�½�һ�� account2 �˻������ 777777$\n");
def account2 make-account(777777);

# һ���﷨�Ƿ����������account����ķ���
def withdraw(account, amount) {
	# �ֲ�����
	def withdraw account("withdraw");
	withdraw(amount);
}
def deposit(account, amount) {
	# �ֲ�����
	def deposit account("deposit");
	deposit(amount);
}

print("account1: \n");
print("\t��� 114��account1 �����Ϊ��");
print(withdraw(account1, 114));
print("$\n");
print("\t��� 514��account1 �����Ϊ��");
print(deposit(account1, 514));
print("$\n");

print("account2: \n");
print("\t��� 123456��account2 �����Ϊ��");
print(withdraw(account2, 123456));
print("$\n");
print("\t��� 888��account2 �����Ϊ��");
print(deposit(account2, 888));
print("$\n");

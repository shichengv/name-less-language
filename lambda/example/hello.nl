# 1. ��ֵ����
# b = -1;
# a = 6 + 7;
# 
# # 2. ��������
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
# 	# ���
# 	def withdraw(amount){
# 		if (balance >= amount) {
# 			balance = balance - amount;
#             balance;
#         }
# 	}
# 
# 	# ���
# 	def deposit(amount) {
# 		balance = balance + amount;
# 		balance;
# 	}
# 
# 	# �ַ�����ѡ����Ҫʹ�õķ���
# 	def dispatch(m) {
# 		if (m == "withdraw")
# 			withdraw;
# 		else if (m == "deposit")
# 			deposit;
# 	}
# 
# 	# ��������ַ���������������������⵽������һ���������Ͳ���ɾ���þֲ�����
# 	dispatch;
# }
# 
# # �½�һ�� account1 �˻�������� 500
# def account1 make-account(500);
# def account2 make-account(500);
# 
# 
# # һ���﷨�Ƿ����������account����ķ���
# def withdraw(account, amount) {
# 	# �ֲ�����
# 	def withdraw account("withdraw");
# 	withdraw(amount);
# }
# def deposit(account, amount) {
# 	# �ֲ�����
# 	def deposit account("deposit");
# 	deposit(amount);
# }
# 
# withdraw(account1, 114);
# deposit(account1, 514);

# 4. �ݹ�͵���

#	�ݹ����׳�
# factorial_recursion = ��(n) if n == 1 then 1; else n * factorial_recursion( n - 1 );

#	��������׳�
# factorial_iteration = ��(n) fact-iter(1, 1, n);
# fact-iter = ��(product, counter, max-count) if counter > max-count then product; else fact-iter( counter * product, counter + 1, max-count);

# print(factorial_recursion(6) + "\n");
# print(factorial_iteration(6) + "\n");

print("hello, world\n");
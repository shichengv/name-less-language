def make-queue() {
	internal-queue = pair(nil, nil);
	front-ptr = nil;
	rear-ptr = nil;

	def empty-queue?()
		typeid(nil) == typeid(front-ptr);

	def front-queue() {
		if empty-queue?()
		then
			error("FRONT called with an empty queue");
		else
			car(front-ptr);
	}

	# ����������뵽����ĩ�ˣ������޸Ĺ��Ķ�����Ϊֵ
	def insert-queue!(item) {
		new-pair = pair(item, nil);
		if empty-queue?()
		{
			set-car!(&internal-queue, new-pair);
			set-cdr!(&internal-queue, new-pair);

			front-ptr = car(internal-queue);
			rear-ptr = cdr(internal-queue);
		}
		else
		{
			# �����е����һ���cdr����Ϊnew-pair
			set-cdr!(&internal-queue, new-pair);
			# ���¶���ĩβָ��
			rear-ptr = cdr(internal-queue);
		}
	}
	# ɾ������ǰ�˵�����������޸ĺ�Ķ�����Ϊֵ
	def delete-queue!() {
		if empty-queue?()
		then 
			error("DELETE! called with an empty queue");
		else
		{
			set-car!(&internal-queue, cdr(front-ptr));
			front-ptr = car(internal-queue);
		}
	}

	def dispatch(message) {
		if message == "front-queue"
		then
			front-queue;
		else if message == "empty-queue?"
		then
			empty-queue?;
		else if message == "insert-queue!"
		then
			insert-queue!;
		else if message == "delete-queue!"
		then
			delete-queue!;
		else
			error("Unknown operation -- QUEUE");
	}

	dispatch;
}

def empty-queue?(queue) {
	empty? = queue("empty-queue?");
	empty?();
}

def front-queue(queue) {
	front = queue("front-queue");
	front();
}

def insert-queue!(queue, item) {
	insert! = queue("insert-queue!");
	insert!(item);
}

def delete-queue!(queue) {
	delete! = queue("delete-queue!");
	delete!()
}

# �½�һ���ն���
op = make-queue();

print("���� Skadi �������У����е�����Ϊ��");
insert-queue!(op, "Skadi");
print(front-queue(op));
print("\n");

print("ɾ������������� Oricinus orca �������У����е�����Ϊ��");
delete-queue!(op);
insert-queue!(op, "Oricinus orca");
print(front-queue(op));
print("\n");

print("ɾ������������� Specter �������У����е�����Ϊ��");
delete-queue!(op);
insert-queue!(op, "Specter");
print(front-queue(op));
print("\n");

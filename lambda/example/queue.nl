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

	# 将数据项插入到队列末端，返回修改过的队列作为值
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
			# 将队列的最后一项的cdr设置为new-pair
			set-cdr!(&internal-queue, new-pair);
			# 更新队列末尾指针
			rear-ptr = cdr(internal-queue);
		}
	}
	# 删除队列前端的数据项，返回修改后的队列作为值
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

# 新建一个空队列
op = make-queue();

print("插入 Skadi 到队列中，队列的首项为：");
insert-queue!(op, "Skadi");
print(front-queue(op));
print("\n");

print("删除队列首项，插入 Oricinus orca 到队列中，队列的首项为：");
delete-queue!(op);
insert-queue!(op, "Oricinus orca");
print(front-queue(op));
print("\n");

print("删除队列首项，插入 Specter 到队列中，队列的首项为：");
delete-queue!(op);
insert-queue!(op, "Specter");
print(front-queue(op));
print("\n");

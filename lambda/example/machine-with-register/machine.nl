# 寄存器机器

def for-each(proc, lst) {
    if lst == nil
    then
        nil;
    else
    {
        proc(car(lst));
        for-each(proc, cdr(lst));
    }
}

# === 模拟寄存器机器 === 
def assoc(key, records)
{
    if (records == nil)
        nil;
    else if (key == caar(records))
        car(records);
    else
        assoc(key, cdr(records));
}

# 定义寄存器对象
def make-register(name) {
    contents = "*unassigned*";
    dispatch = lambda(message) {
        if (message == "get")
            contents;
        else if (message == "set")
            lambda(value) contents = value;
        else
            error("Unknown request -- REGISTER");
    }    
    dispatch;
}

def get-register(machine, reg-name){
    get-reg = machine("get-register");
    get-reg(reg-name);
}

def get-contents(register) 
    register("get");

def set-contents!(register, value) {
    set-the-register-contents = register("set");
    set-the-register-contents(value);
}

# 定义栈对象
def make-stack() {
    s = nil;
    push = lambda(x) s = pair(x, s);
    pop = lambda() {
        if (s == nil)
            error("Error Stack -- POP");
        else
        {
            top = car(s);
            s = cdr(s);
            top;
        }
    }
    initialize = lambda() {
        s = nil;
        print("done\n");
    }
    dispatch = lambda(message) {
        if (message == "push")
            push;
        else if (message == "pop")
            pop;
        else if (message == "initialize")
            initialize;
        else
            error("Unknown request: STACK");
    }

    dispatch;
}

# 出栈动作
def pop(stack) {
    pop-the-stack = stack("pop");
    pop-the-stack(); 
}

# 入栈动作
def push(stack, value) {
    push-the-stack = stack("push");
    push-the-stack(value);
}

# 汇编语言
def assemble(controller-text, machine) {
    extract-labels(controller-text, lambda(insts, labels) {
        updates-insts!(insts, labels, machine);
        insts;
    });
}

def extract-labels(text, receive) {
    if text == nil
    then
        receive(list(), list());
    else
        extract-labels( cdr(text, lambda(insts, labels) {
            next-inst = car(text);
            if string?(next-inst)
            then
                receive(insts, pair(make-label-entry(next-inst, inst), labels));
            else
                receive(pair(make-instruction(next-inst), insts), labels);
        } ))
}

def update-insts!(insts, labels, machine) {

    pc = get-register(machine, "pc");
    flag = get-register(machine, "flag");
    stack = machine("stack");
    ops = machine("operations");

    for-each( lambda(inst) { 
        set-instruction-execution-proc!(inst, 
        make-execution-procedure(
            instruction-text(inst), labels, machine, pc, flag, stack, ops) ) }, 
            insts );
}

def make-instruction(text) 
    pair(text, nil);

def instruction-text(inst)
    car(inst);
def instruction-execution-proc(inst)
    cdr(inst);
def set-instruction-execution-proc!(inst, proc)
    set-cdr!(inst, proc);

def make-label-entry(label-name, insts)
    pair(label-name, insts);

def lookup-label(labels, label-name) {
    val = assoc(label-name, labels);
    if val
    then 
        cdr(val);
    else
        error("Undefined labes -- ASSEMBLE");
}


def make-execution-procedure(inst, labels, machine, pc, flag, stack, ops) {
    if (car(inst) == "assign")
        make-assign(inst, machine, labels, ops, pc);
    else if (car(inst) == "test")
        make-test(inst, machine, labels, ops, flag, pc);
    else if (car(inst) == "branch")
        make-branch(inst, machine, labels, flag, pc);
    else if (car(inst) == "goto")
        make-goto(inst, machine, labels, pc);
    else if (car(inst) == "save")
        make-save(inst, machine, stack, pc);
    else if (car(inst) == "restore")
        make-restore(inst, machine, stack, pc);
    else if (car(inst) == "preform")
        make-preform(inst, machine, labels, ops, pc);
    else
        error("Unknown instruction type -- ASSEMBLE");
}

# 从assign指令里提取目标寄存器的名字
def assign-reg-name(assign-instruction)
    cadr(assign-instruction);

# 提取值表达式
def assign-value-exp(assign-instruction)
    cddr(assign-instruction);

def advance-pc(pc)
    set-contents!(pc, cdr(get-contents(pc)));

# 新建一个寄存器机器
def make-new-machine() {
    pc = make-register("pc"); 
    flag = make-register("flag");
    stack = make-stack();
    the-instruction-sequence = nil;

    return = {

        the-ops = list( list("initialize-stack", lambda() { stack("initialize"); } ) );
        register-table = list( list("pc", pc), list("flag", flag));

        allocate-register = lambda(name) {
            if assoc(name, register-table)
            then
                error("Multiply defined register: " + name);
            else
                register-table = pair(list(name, make-register(name)), register-table);
            print("register-allocated");
        }

        lookup-register = lambda(name) {
            val = assoc(name, register-table);
            if val
            then
                cadr(val);
            else
                error("Unknown register:" + name);
        }

        execute = lambda() {
            insts = get-contents(pc);
            if insts
            then
                print("done");
            else
            {
                instruction-execute-proc(car(insts));
                execute();
            }
        }

        dispatch = lambda(message) {
            if (message == "start")
            {
                set-contents!(pc, the-instruction-sequence);
                execute();
            }
            else if (message == "install-instruction-sequence")
                lambda(seq) the-instruction-sequence = seq;
            else if (message == "allocate-register")
                allocate-register;
            else if (message == "get-register")
                lookup-register;
            else if (message == "install-operations")
                lambda(ops) append(&the-ops, ops);
            else if (message == "stack")
                stack;
            else if (message == "operations")
                the-ops;
            else 
                error("Unknown request -- MACHINE");
        }

        dispatch; 
        
    }
    
}
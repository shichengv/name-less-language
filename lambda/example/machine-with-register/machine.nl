# 寄存器机器

def assoc(key, records)
{
    if (records == nil)
        nil;
    else if (key == caar(records))
        car(records);
    else
        assoc(key, cdr(records));
}

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

def map(proc, lst) {
    if lst == nil
    then
        nil;
    else
        pair(proc(car(lst)), map(proc, cdr(lst)));
}












# === 模拟寄存器机器 === 

# =============================== 定义寄存器对象 =============================================
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
}

# 获取寄存器的值
def get-contents(register) {
    register("get");
}
# 更改寄存器的内容
def set-contents!(register, value) {
    set-the-register-contents = register("set");
    set-the-register-contents(value);
}


























# =================================== 定义栈对象 ===============================================
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
        print("\ndone\n");
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





















# ================================= 虚拟机定义 ================================================

# 新建一个寄存器机器
def make-new-machine() {
    pc = make-register("pc"); 
    flag = make-register("flag");
    stack = make-stack();
    the-instruction-sequence = nil;

    return = {

        the-ops = list( 
            list("initialize-stack", lambda() { stack("initialize"); } )
            );

        # 机器内部的寄存器表
        register-table = list( list("pc", pc), list("flag", flag));

        machine-info = lambda() {
            print("pc:");
            print(get-contents(pc));
            print("\n");

            print("flag:");
            print(get-contents(flag));
            print("\n");

            print("the-ops:");
            print(the-ops);
            print("\n");

            print("register-table:");
            print(register-table);
            print("\n");
        }

        # 为该机器新建一个寄存器
        allocate-register = lambda(name) {
            reg = make-register(name);
            if assoc(name, register-table)
            then
                error("Multiply defined register: " + name);
            else
                register-table = pair( list(name, reg), register-table);
            print(name + "  register-allocated\n");
            reg;
        }

        lookup-register = lambda(name) {
            val = assoc(name, register-table);
            if val
            then
                # 提取寄存器的值
                cadr(val);
            else
                error("Unknown register:" + name);
        }

        execute = lambda() {
            print("开始执行指令序列\n");
            print("PC寄存器的内容为:\n");
            insts = get-contents(pc);
            print(insts);
            print("\n");
            if insts == nil
            then
                print("done");
            else
            {
                inst-proc = instruction-execution-proc(car(insts));
                inst-proc();
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
                lambda(ops) append(the-ops, ops);
            else if (message == "stack")
                stack;
            else if (message == "operations")
                the-ops;
            else if (message == "machine-info")
                machine-info;
            else 
                error("Unknown request -- MACHINE");
        }

        dispatch; 
        
    }
}


# 用给定的名字在一部给定的机器里查看有关寄存器的内容
def get-register(machine, reg-name) {
    get-reg = machine("get-register");
    reg = get-reg(reg-name);
}

# 设置新的虚拟机的内容
def make-machine(register-names, ops, controller-text) {

    print("\n新建一个寄存器机器 make-new-machine()\n");
    machine = make-new-machine();

    print("\n安装一系列寄存器 machine(\"allocate-register\")\n");
    for-each(lambda(register-name) {
        allocate-register = machine("allocate-register");
        allocate-register(register-name);
    }, register-names);
    
    print("\n安装操作 machine(\"install-operations\")\n");
    install-operations = machine("install-operations");
    install-operations(ops);

    print("\n构造指令序列 machine(\"install-instruction-sequence\")\n");
    install-instruction-sequence = machine("install-instruction-sequence");
    install-instruction-sequence(assemble(controller-text, machine));

    print("\nMachine Info:\n");
    info = machine("machine-info");
    info();

    machine;
}


# 用来启动一个机器
def start(machine) {
    start-machine = machine("start");
    start-machine();
}


# 给定一个虚拟机和他的寄存器名字，获取它的值
def get-register-contents(machine, register-name) 
    print(get-contents(get-register(machine, register-name)));

# 给定一个虚拟机和他的寄存器名字，更改它的值
def set-register-contents!(machine, register-name, value) {
    set-contents!(get-register(machine, register-name), value);
    print("\n寄存器值更改完毕\n");
}






















# ==================================== 汇编语言 ========================================================
# 过程assemble是汇编程序的主要入口，它以一个控制器正文和相应的机器模型作为参数，
# 返回存储在模型里的指令序列。

# 该过程根据所提供的控制器正文构造出的初始指令表和标号列表。
# extract-labels 的第二个参数是一个过程，调用它去处理上面得到的结果。


def assemble(controller-text, machine) {
    print("\n构造汇编指令序列: ");
    print(controller-text);
    print("\n");
    extract-labels(controller-text, lambda(insts, labels) {
        update-insts!(insts, labels, machine);
        insts;
    });
}

#   list("_init", list("assing", "b", reg("a")), "_done")

def extract-labels(text, receive) {
    print("\n调用extract-labels()，当前的text为：");
    print(text);
    print("\n");
    if text == nil
    then
        receive(nil, nil);
    else
        extract-labels( cdr(text), lambda(insts, labels) {

            print("\n调用receive()，\n当前的insts为: ");
            print(insts);
            print("\n当前的labels为: ");
            print(labels);
            print("\n");

            next-inst = car(text);
            if string?(next-inst)
            then
                receive(insts, pair(make-label-entry(next-inst, insts), labels));
            else
                receive(pair(make-instruction(next-inst), insts), labels);
        } );
}


def update-insts!(insts, labels, machine) {
    print("\n更新指令序列 update-insts!:\n");
    print("insts: ");
    print(insts);
    print("\n");

    print("labels: ");
    print(labels);
    print("\n");

    pc = get-register(machine, "pc");
    flag = get-register(machine, "flag");
    stack = machine("stack");
    ops = machine("operations");

    new-insts = list();

    for-each( lambda(inst) { 
        set-instruction-execution-proc!(inst, 
        make-execution-procedure(
            instruction-text(inst), labels, machine, pc, flag, stack, ops) ); 
        print(inst);
        append!(&new-insts, inst);
        }, 
            insts );
    print("new-insts: ");
    print(new-insts);
    print("\n");
    set-register-contents!(machine, "pc", new-insts);

}

def make-instruction(text) {
    new_pair = pair(text, nil);
    print("调用make-instruction()，新序对为：");
    print(new_pair);
    print("\n");
    new_pair;
}

def instruction-text(inst)
    car(inst);
def instruction-execution-proc(inst) 
    cdr(inst);

def set-instruction-execution-proc!(inst, proc) {
    print("set-instruction-execution-proc!\n");
    set-cdr!(&inst, proc);
    print("inst: ");
    print(inst);
    print("\n");
    print("proc: ");
    print(proc);
    print("\n");
}

def make-label-entry(label-name, insts) {
    new_pair = pair(label-name, insts);
    print("调用make-label-entry()，新序对为：");
    print(new_pair);
    print("\n");
    new_pair;
}

def lookup-label(labels, label-name) {
    val = assoc(label-name, labels);
    if val
    then 
        cdr(val);
    else
        error("Undefined labes -- ASSEMBLE");
}




















# ======================== 为指令生成执行过程 ===================================

def make-execution-procedure(inst, labels, machine, pc, flag, stack, ops) {
    print("\n\n为指令生成执行过程make-execution-procedure()\n");
    print("inst: ");
    print(inst);
    print("\n");

    print("labels: ");
    print(labels);
    print("\n");

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

def tagged-list?(exp, name) {
    car(exp) == name;
}

def advance-pc(pc)
    set-contents!(pc, cdr(get-contents(pc)));

def op-assign(var, value)
    var = value;

def reg(name) list("reg", name);
def op(name) list("op", name);

def register-exp?(exp) tagged-list?(exp, "reg");
def register-exp-reg(exp) cadr(exp);
def constant-exp?(exp) tagged-list?(exp, "const");
def constant-exp-value(exp) cadr(exp);
def label-exp?(exp) tagged-list?(exp, "label");
def label-exp-label(exp) cadr(exp);

def operation-exp?(exp) 
    pair?(exp) && tagged-list?(car(exp), "op");
def operation-exp-op(operation-exp)
    cadr(car(operation-exp));
def operation-exp-operands(operation-exp)
    cdr(operation-exp);

def lookup-prim(symbol, operations) {
    val = assoc(symbol, operations);
    if val
    then 
        cadr(val);
    else
        error("Unknown operation: ASSEMBLE");
}

def make-operation-exp(exp, machine, labels, operations) {
    op = lookup-prim(operation-exp-op(exp), operations);
    aprocs = map(
        lambda(e) { make-primitive-exp(e, machine, labels); }, 
        operation-exp-operands(exp)
    );
    lambda() {
        5;
    }
}

def make-primitive-exp(exp, machine, labels) {
    if (constant-exp?(exp))
    {
        c = constant-exp-value(exp);
        lambda() c;
    }
    else if (label-exp?(exp))
    {
        insts = lookup-label(labels, label-exp-label(exp));
        lambda() insts;
    }
    else if (register-exp?(exp))
    {
        r = get-register(machine, register-exp-reg(exp));
        lambda() (get-contents(r));
    }
    else
    {
        error("Unknown expression type: ASSEMBLE");
    }
        
}



def make-assign(inst, machine, labels, operations, pc) {
    print("\n生成assign过程\n");
    target = get-register(machine, assign-reg-name(inst));
    value-exp = assign-value-exp(inst);

    value-proc = if (operation-exp?(value-exp))
                    make-operation-exp(value-exp, machine, labels, operations);
                 else
                    make-primitive-exp(car(value-exp), machine, labels);
    lambda() {
        set-contents!(target, value-proc());
        advance-pc(pc);
    }
}

# 从assign指令里提取目标寄存器的名字
def assign-reg-name(assign-instruction)
    cadr(assign-instruction);

# 提取值表达式
def assign-value-exp(assign-instruction)
    cddr(assign-instruction);


def assign(register-name, source) list("assign", register-name, source);
























# =================== 启动虚拟机 ===================================

# 新建一个赋值机器
assign-machine = make-machine(
        list("a", "b"),
        list( list("=", op-assign ) ),
        list("_init", assign("b", reg("a")), "_done")
    );

print("设置寄存器a的值为 5\n");
set-register-contents!(assign-machine, "a", 5);
print("启动虚拟机\n");
start(assign-machine);
print("查看寄存器b的值为: ");
get-register-contents(assign-machine, "b");
print("\n");


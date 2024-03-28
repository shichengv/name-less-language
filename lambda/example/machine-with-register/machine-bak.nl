# �Ĵ�������

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












# === ģ��Ĵ������� === 

# =============================== ����Ĵ������� =============================================
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

# ��ȡ�Ĵ�����ֵ
def get-contents(register) {
    get-register-contents = register("get");
    get-register-contents();
}
# ���ļĴ���������
def set-contents!(register, value) {
    set-the-register-contents = register("set");
    set-the-register-contents(value);
}


























# =================================== ����ջ���� ===============================================
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

# ��ջ����
def pop(stack) {
    pop-the-stack = stack("pop");
    pop-the-stack(); 
}

# ��ջ����
def push(stack, value) {
    push-the-stack = stack("push");
    push-the-stack(value);
}





















# ================================= ��������� ================================================

# �½�һ���Ĵ�������
def make-new-machine() {
    pc = make-register("pc"); 
    flag = make-register("flag");
    stack = make-stack();
    the-instruction-sequence = nil;

    return = {

        the-ops = list( 
            list("initialize-stack", lambda() { stack("initialize"); } )
            );

        # �����ڲ��ļĴ�����
        register-table = list( list("pc", pc), list("flag", flag));

        # Ϊ�û����½�һ���Ĵ���
        allocate-register = lambda(name) {
            if assoc(name, register-table)
            then
                error("Multiply defined register: " + name);
            else
                register-table = pair( list(name, make-register(name)), register-table);
            print(name + "register-allocated\n");
        }

        lookup-register = lambda(name) {
            val = assoc(name, register-table);
            if val
            then
                # ��ȡ�Ĵ�����ֵ
                cadr(val);
            else
                error("Unknown register:" + name);
        }

        execute = lambda() {
            insts = get-contents(pc);
            if insts == nil
            then
                print("done");
            else
            {
                instruction-execution-proc(car(insts));
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
            else 
                error("Unknown request -- MACHINE");
        }

        dispatch; 
        
    }
}


# �ø�����������һ�������Ļ�����鿴�йؼĴ���������
def get-register(machine, reg-name) {
    get-reg = machine("get-register");
    get-reg(reg-name);
}

# �����µ������������
def make-machine(register-names, ops, controller-text) {

    print("�½�һ���Ĵ������� make-new-machine()\n");
    machine = make-new-machine();

    print("��װһϵ�мĴ��� machine(\"allocate-register\")\n");
    for-each(lambda(register-name) {
        allocate-register = machine("allocate-register");
        allocate-register(register-name);
    }, register-names);
    
    print("��װ���� machine(\"install-operations\")\n");
    install-operations = machine("install-operations");
    install-operations(ops);

    print("����ָ������ machine(\"install-instruction-sequence\")\n");
    install-instruction-sequence = machine("install-instruction-sequence");
    install-instruction-sequence(assemble(controller-text, machine));

    machine;
}


# ��������һ������
def start(machine) {
    start-machine = machine("start");
    start-machine();
}


# ����һ������������ļĴ������֣���ȡ����ֵ
def get-register-contents(machine, register-name) 
    print(get-contents(get-register(machine, register-name)));

# ����һ������������ļĴ������֣���������ֵ
def set-register-contents!(machine, register-name, value) {
    set-contents!(get-register(machine, register-name), value);
    print("done");
}






















# ==================================== ������� ========================================================
# ����assemble�ǻ��������Ҫ��ڣ�����һ�����������ĺ���Ӧ�Ļ���ģ����Ϊ������
# ���ش洢��ģ�����ָ�����С�

# �ù��̸������ṩ�Ŀ��������Ĺ�����ĳ�ʼָ���ͱ���б�
# extract-labels �ĵڶ���������һ�����̣�������ȥ��������õ��Ľ����


def assemble(controller-text, machine) {
    print("������ָ������: ");
    print(controller-text);
    print("\n");
    extract-labels(controller-text, lambda(insts, labels) {
        update-insts!(insts, labels, machine);
        insts;
    });
}

#   list("_init", list("assing", "b", reg("a")), "_done")

def extract-labels(text, receive) {
    print("����extract-labels()����ǰ��textΪ��");
    print(text);
    print("\n");
    if text == nil
    then
        receive(nil, nil);
    else
        extract-labels( cdr(text), lambda(insts, labels) {

            print("����receive()����ǰ��instsΪ��");
            print(insts);
            print("\n");

            next-inst = car(text);
            if string?(next-inst)
            then
                receive(insts, pair(make-label-entry(next-inst, inst), labels));
            else
                receive(pair(make-instruction(next-inst), insts), labels);
        } );
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




















# ======================== Ϊָ������ִ�й��� ===================================

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

# ��assignָ������ȡĿ��Ĵ���������
def assign-reg-name(assign-instruction)
    cadr(assign-instruction);

# ��ȡֵ���ʽ
def assign-value-exp(assign-instruction)
    cddr(assign-instruction);


def assign(register-name, source) list("assign", register-name, source);
























# =================== ��������� ===================================

# �½�һ����ֵ����
assign-machine = make-machine(
        list("a", "b"),
        list( list("=", op-assign ) ),
        list("_init", assign("b", reg("a")), "_done")
    );

# ���������

# ���üĴ���a��ֵΪ 5;
# set-register-contents!(assign-machine, "a", 5);
# start(assign-machine);
# get-register-contents(assign-machine, "b");


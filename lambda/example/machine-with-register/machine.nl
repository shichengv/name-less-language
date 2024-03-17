# 寄存器机器
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

def get-contents(register) register("get");

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

# 新建一个寄存器机器
def make-new-machine() {
    pc = make-register("pc"); 
    flag = make-register("flag");
    stack = make-stack();
    the-instruction-sequence = nil;

    return = {

        the-ops = list( list("initialize-stack", lambda() stack("initialize"); ))
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
            if insts;
            then
                print("done");
            else
            {
                instruction-execute-proc(car(ints));
                execute();
            }
        }

        
        
    }
    
}
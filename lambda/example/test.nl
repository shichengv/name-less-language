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

reg1 = make-register("b");
set-contents!(reg1, 5);
print(get-contents(reg1));
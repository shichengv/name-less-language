items = list(1, lambda(x) { x; } );
sum = cadr(items);
x = car(items);
print(sum(x));

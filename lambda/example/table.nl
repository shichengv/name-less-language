# 表格的表示

# 二维表格，每个值保存再一个关键码之下。
def assoc(key, records)
{
    if (records == nil)
        nil;
    else if (key == caar(records))
        car(records);
    else
        assoc(key, cdr(records));
}
def make-table() {

    # 局部表
    local-table = list("*table*");

    lookup = lambda(key-1, key-2)
    {
        subtable = assoc(key-1, cdr(local-table));
        result = if (subtable != nil)
				{
					record = assoc(key-2, cdr(subtable));
					result = if (record != nil) cdr(record); else nil;
				}
				else
					nil;
    }

    insert! = lambda(key-1, key-2, value)
    {
        subtable = assoc(key-1, cdr(local-table));
        if (subtable != nil)
        {
            record = assoc(key-2, cdr(subtable));
            if (record != nil)
                set-cdr!(record, value);
            else
                set-cdr!(&subtable, pair( pair(key-2, value), cdr(subtable) ));
        }
        else
            set-cdr!(&local-table, pair(list(key-1, pair(key-2, value)), cdr(local-table)));
    }
    dispatch = lambda(message) {
        if(message == "lookup-proc")
            lookup;
        else if (message == "insert-proc!")
            insert!;
        else
            error("Unknown operation: TABLE");
    }
}

operation-table = make-table();
get = operation-table("lookup-proc");
put = operation-table("insert-proc!");

put("math", "+", 43);
print(get("math", "+"));




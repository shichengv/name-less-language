# skadi = pair(list("Orcinus orca", pair("Skadi", "Skadi the Corrupting")), "Specter");

# skadi = pair("Skadi", "Orcinus orca");
# set-cdr!(&skadi, "Skadi the Corrupting");
# print(skadi);

skadi = list("skadi", "Orcinus orca", "Specter", "Skadi the Corrupting");
set-car!(&skadi, "Skadi");
print(car(skadi));
print(cdr(skadi));

# ±éÀúÁĞ±í
def loop-list(items) {
	item = car(items);

	if typeid(item) != typeid(nil)
	{
		print(item + "\t");
		loop-list(cdr(items));
	}
}

# loop-list(skadi);
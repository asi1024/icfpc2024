x = input()
assert x[:8] == 'String("'
assert x[-2:] == '")'
x = x[8:]
x = x[:-2]
x = x.replace("\\n", "\n")
print(x)

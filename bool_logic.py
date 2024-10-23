a=1
b=1
c=0
d=0
f=not(a and (not b) and (a or b )) or  (not a and b) and not (a or(not b) or (not c ))
g= not((c or d) and a and not(c) and d and ((not a) and c or (not d)))

s1 =a and (a or b)
s2 =(a or b) and ((not a)or b)
s3=(a and (not b)or c) and (a or (not b))
s4=(a and b) or c and (not a)or (b and c) and (not b)
s5= (a or b or c) and (not a or b or c) or (a and b) or c and d
s6= a or b or (not c) and b or (a or not c ) or not ((not a) or b or ((not a)and c))
print("------------------")
print(f)
print(g)
print("------------------")
print(s1)
print(s2)
print("------------------")
print(bool(s1))
print(s2)

class Person():
    def is_major(self):
        return self.age>=18
    
toto = Person()
toto.age=20
titi= Person()
titi.age=15
nana=Person()
nana.age=56
nono=Person()
nono.age=42

a=toto.is_major()
b= not(titi.is_major())
c=nana.is_major()
d=nono.is_major()

"""
a=1
b=1
c=0
d=0
"""
f=not(a and (not b) and (a or b )) or  (not a and b) and not (a or(not b) or (not c ))
g= not((c or d) and a and not(c) and d and ((not a) and c or (not d)))

s1 =a and (a or b)
s2 =(a or b) and ((not a)or b)
s3=(a and (not b)or c) and (a or (not b))
s4=(a and b) or c and (not a)or (b and c) and (not b)
s5= (a or b or c) and (not a or b or c) or (a and b) or c and d
s6= a or b or (not c) and b or (a or not c ) or not ((not a) or b or ((not a)and c))
print("------------------")
print("f=",f)
print("g=",g)
print("------------------")
print("s1=",s1)
print("s2=",s2)
print("------------------")
print("s3=",s3)
print("s4=",s4)
print("------------------")
print("s5=",s5)
print("s6=",s6)
print("------------------")


s1 or (s2 and s3) == (s1 or s2) and (s1 or s3)
s1 and (s2 or s3) == (s1 and s2) or (s1 and s3)
s1 and s1 or s2 ==s1
s1 or (s1 and s2)==s1
s1 and not(s1) or s2 == s1 
s1 and s2 or(not s1 and s2)


s1 and not s1 == False
s1 or not s1 == True


"""
s1=
a or b
a = 1, b = 1.
Donc, a or b = 1.
a and (a or b) = 1 and 1 = 1 (vrai).

s2= (a or b) and ((not a) or b) = 1 and 1 = 1 (vrai).

s3= (a and (not b) or c) and (a or (not b)) = 0 and 1 = 0 (faux).
s4= 
a = 1, b = 1.
Donc, 1 and 1 = 1.
s4 = 1(vrai)
s5= (a or b or c) and (not a or b or c) or (a and b) or c and d = 1 and 1 or 1 or 0 = 1.
s6 = 1 or 1 or 1 = 1.

f = 1 (vrai)
g = 1 (vrai)
s1 = 1 (vrai)
s2 = 1 (vrai)
s3 = 0 (faux)
s4 = 1 (vrai)
s5 = 1 (vrai)
s6 = 1 (vrai)
"""


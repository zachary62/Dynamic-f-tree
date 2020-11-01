f = open("r2" , "w")
for j in range(5,50,5):
    f.write(str(j) + "\n")
f.write("50")
f.close()

f = open("r3" , "w")
for j in range(2,100,2):
    f.write(str(j) + "\n")
f.write("100")
f.close()

f = open("f1" , "w")
for j in range(1,10):
    f.write("1.01\n")
f.write("1.01")
f.close()

f = open("f2", "w")
for j in range(1,50):
    f.write("1.01\n")
f.write("1.01")
f.close()

f = open("f3" , "w")
for j in range(1,100):
    f.write("1.01\n")
f.write("1.01")
f.close()
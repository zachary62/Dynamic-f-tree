first_dim = 10
second_dim = 10000
third_dim = 10000000

step1 = second_dim/first_dim
step2 = third_dim/second_dim


for i in range(1,7,3):
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,first_dim):
            f.write("1.01\n")
        f.write("1.01")
        f.close()


for i in range(2,7,3):
    f = open("r" + str(i) , "w")
    for j in range(step1,second_dim,step1):
        f.write(str(j) + "\n")
    f.write(str(second_dim))
    f.close()
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,second_dim ):
            f.write("1.01\n")
        f.write("1.01")
        f.close()

for i in range(3,7,3):
    f = open("r" + str(i), "w")
    for j in range(step2,third_dim,step2):
        f.write(str(j) + "\n")
    f.write(str(third_dim))
    f.close()
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,third_dim):
            f.write("1.01\n")
        f.write("1.01")
        f.close()
relation_size = 10
num_dim = 10

for i in range(1,3*num_dim + 1,1):
        f = open("f" + str(i), "w")
        for j in range(1,relation_size):
            f.write("1.01\n")
        f.write("1.01")
        f.close()


for i in range(1,3*num_dim + 1,1):
    f = open("r" + str(i) , "w")
    for j in range(1,relation_size,1):
        f.write(str(j) + "\n")
    f.write(str(relation_size ))
    f.close()

f = open("schema" , "w")
f.write(str(num_dim) + " " + str(3*num_dim) + " " + str(3*num_dim) + "\n")
for i in range(1,3*num_dim + 1):
    f.write(str(relation_size) + " " + str(i) + "\n")

for i in range(1,num_dim + 1):
    f.write(str(i*3 - 3 + 1) + " " + str(i*3 - 3 + 2) + " " + str(i*3 - 3 + 3) + "\n")
relation_size = 10
num_dim = 3

for i in range(1,4*num_dim + 1,1):
        f = open("f" + str(i), "w")
        for j in range(1,relation_size):
            f.write("1.01\n")
        f.write("1.01")
        f.close()


for i in range(1,4*num_dim + 1,1):
    f = open("r" + str(i) , "w")
    for j in range(1,relation_size,1):
        f.write(str(j) + "\n")
    f.write(str(relation_size ))
    f.close()

f = open("schema" , "w")
f.write(str(num_dim) + " " + str(4*num_dim) + " " + str(4*num_dim) + "\n")
for i in range(1,4*num_dim + 1):
    f.write(str(relation_size) + " " + str(i) + "\n")

for i in range(1,num_dim + 1):
    f.write(str(i*4 - 4 + 1) + " " + str(i*4 - 4 + 2) + " " + str(i*4 - 4 + 3) +  " " + str(i*4 - 4 + 4) + "\n")
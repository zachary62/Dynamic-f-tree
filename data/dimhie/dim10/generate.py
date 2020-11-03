relation_size = 10000
num_dim = 3


for i in range(1,7*num_dim + 1,1):
        f = open("f" + str(i), "w")
        for j in range(1,relation_size):
            f.write("1.01\n")
        f.write("1.01")
        f.close()


for i in range(1,7*num_dim + 1,1):
    f = open("r" + str(i) , "w")
    for j in range(1,relation_size,1):
        f.write(str(j) + "\n")
    f.write(str(relation_size ))
    f.close()

f = open("schema" , "w")
f.write(str(num_dim) + " " + str(7*num_dim) + " " + str(7*num_dim) + "\n")
for i in range(1,7*num_dim + 1):
    f.write(str(relation_size) + " " + str(i) + "\n")

for i in range(1,num_dim + 1):
    f.write(str(i*7 - 7 + 1) + " " + str(i*7 - 7 + 2) + " " + str(i*7 - 7 + 3) +  " " + str(i*7 -7 + 4) + " " + 
    str(i*7 - 7 + 5) +  " " + str(i*7 - 7 + 6)+  " " + str(i*7 - 7 + 7) + "\n")
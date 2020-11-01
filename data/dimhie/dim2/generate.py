for i in range(1,7,3):
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,10):
            f.write("1.01\n")
        f.write("1.01")
        f.close()


for i in range(2,7,3):
    f = open("r" + str(i) , "w")
    for j in range(5,50,5):
        f.write(str(j) + "\n")
    f.write("50")
    f.close()
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,50):
            f.write("1.01\n")
        f.write("1.01")
        f.close()

for i in range(3,7,3):
    f = open("r" + str(i), "w")
    for j in range(2,100,2):
        f.write(str(j) + "\n")
    f.write("100")
    f.close()
    for k in range(1,6,1):
        f = open("f" + str(i*5 - 5 + k), "w")
        for j in range(1,100):
            f.write("1.01\n")
        f.write("1.01")
        f.close()
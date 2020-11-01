for i in range(1,21):
    f = open("f" + str(i) , "w")
    for j in range(1,10):
        f.write("1.01\n")
    f.write("1.01")
    f.close()
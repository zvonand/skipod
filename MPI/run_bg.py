import itertools
import os

nCpusList = [1, 2, 4, 8, 16, 32, 64, 128, 256]  
sizesList = [256, 512, 1024, 1546, 2048, 3072, 4096]

# os.system("mpixlcxx_r -o main main.cpp")

for nCpus in nCpusList:
    for sz in sizesList:
        fName = str(nCpus) + "_" + str(sz)
        if os.path.exists(fName + ".out") or os.path.exists(fName + ".err"):
            continue
        os.system("mpisubmit.bg -n " + str(nCpus) +
                  " -w 30:00"
                  " -stdout " + fName + ".out"
                  " -stderr " + fName + ".err"
                  " main -- " + str(sz))

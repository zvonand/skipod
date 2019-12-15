import itertools
import os

nThreadsList = [1, 2, 4, 8, 256, 512]
sizesList = [512, 1024, 1536, 2048, 2560]

for nThreads in nThreadsList:
    for sz in sizesList:
        fName = str(nThreads) + str(sz)
        if os.path.exists("omp/" + fName + ".out") or os.path.exists("omp/" + fName + ".err"):
            continue
        os.system("mpisubmit.pl -p 1"
                  " -W 00:30"
                  " -stdout " + "omp/" + fName + ".out"
                  " -stderr " + "omp/" + fName + ".err"
                  " ./omp_alg -- " + str(sz) + " " + str(nThreads))

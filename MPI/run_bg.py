import itertools
import os

nCpusList = [1, 2, 4, 8, 16, 32, 64, 128, 256]
sizesList = [256, 512, 1024, 1546, 2048, 3072, 4096]

# os.system("mpixlcxx_r -o mpi_alg mpi_alg.cpp")

for nCpus in nCpusList:
    for sz in sizesList:
        fName = "mpi/" + str(nCpus) + "_" + str(sz)
        if os.path.exists(fName + ".out") or os.path.exists(fName + ".err"):
            continue
        time = "00:15:00"
        if nCpus >= 128:
            time = "00:10:00"
        os.system("mpisubmit.bg -n " + str(nCpus) +
                  " -w " + time
                  " -stdout " + fName + ".out"
                  " -stderr " + fName + ".err"
                  " main -- " + str(sz))

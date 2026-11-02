g++ nonConcSet.cpp setController.cpp coarsePthreads.cpp main.cpp -o benchmark -fopenmp
./benchmark --threads 8 --ops 100000 --insert 20 --remove 50 --contains 30 --range 100000 --impl coarse_pth

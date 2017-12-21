Generally, just run the code with the command 'make'. Then you can see the result in 'trace.txt.out'.

In the file 'branchsimulator.h', there are 2 classes representing Branch History Register and Pattern History Table.

In the file 'branchsimulator.cpp', you can see the main function and the functions of the two classes.

The config file 'config.txt' contains two lines, the first with the value of m and the second with the value of k. A sample file for m=12 and k=2 is provided. The largest value of m is 32 (for 32 bit PCs), but we will not input an m larger than 20.

The trace file, 'trace.txt', contains one branch per line. Each line has the PC for the corresponding branch (in hex format) followed by a single bit indicating Taken (1) or Not Taken (0). A sample trace file is provided.

The output from the simulator is a file 'trace.txt.out' that has one line per branch. Each line has a single bit which indicates whether that branch was predicted as Taken (1) or Not Taken (0).

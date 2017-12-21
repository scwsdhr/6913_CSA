#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import re
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def main():
    rate = np.zeros((7,5))
    for k in range(0, 5):
        for m in range(10, 17):
            with open('config.txt', 'w') as f:
                f.write(str(m) + '\n' + str(k))
            rate[m - 10, k] = getrate()
        plt.figure(k)
        plt.plot(range(10, 17), rate[:, k], '-^')
        plt.xlabel('m')
        plt.ylabel('Misprediction Rate (%)')
        plt.title('Branch Misprediction Rate When k = ' + str(k))
        plt.savefig('Fig'+str(k)+'.png')

def getrate():
    console = os.popen('make').readlines()
    last_line = console[len(console)-1]
    last_line = re.split(' |%', last_line)
    return float(last_line[4])


if __name__ == '__main__':
    main()

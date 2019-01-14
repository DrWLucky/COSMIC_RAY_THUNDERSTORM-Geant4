#!/usr/bin/env python2.7
import platform
from subprocess import call
from functools import partial
from multiprocessing.dummy import Pool
import numpy as np
import random
import time
import sys
from datetime import datetime
import subprocess as sp

computer_name = platform.node()

#### functions definitions

############################

random.seed(datetime.now())
seedd = random.randint(1,1000000000)

################################################################################
### Defining commands to run

#efield_altitudes =  [9.,10.,11.,12.,13.,14.]# kilometers, record altitude

efield_altitudes = [12.0]# kilometers, record altitude

altitudes = [4.,      5.,     6.,     7.,     8.,     9.,     10.,     11.,     12.,     13.,      14.,      15.,      16.]
scales =    [1.5434,  1.7199, 1.9253, 2.1686, 2.4604, 2.8134, 3.2454,  3.7742,  4.4144,  5.1808,   6.0821,   7.1335,   8.3483]

altitudes = np.array(altitudes)
scales = np.array(scales)

efield_sizes = [1.0, 2.0] # does not matter if efield_altitudes > 20

#potential_list = [40., 60., 80. ,100., 120., 140., 160.]

# 0.284 MV/m
RREA_thres = 284.0 # MV/km
frac_rrea_thres_list = np.array([0.0, 0.25, 0.5, 0.75, 1., 1.25, 1.5]) # fraction of RREA threshold

if ("iftrom" in computer_name) or ("7370" in computer_name) or ("sarria-pc" in computer_name):
    nb_run = 1
else :
    nb_run = 1000
#potential_list = [200.]

nb_record_per_run = 100

# defining the commands to be run in parallel

commands=[]
excecutable = './mos_test'

#seedd += 1

for _ in range(nb_run):
    for size in efield_sizes:
        for alti_e in efield_altitudes:
        
            scale_fact = np.interp(alti_e, altitudes, scales)
            # converting "fraction of RREA" threshold to potential
            potential_list = RREA_thres * frac_rrea_thres_list * size / scale_fact
            
            print(potential_list)
            
            record_altitudes = [alti_e, alti_e-size/2.0, alti_e+size/2.0]
            
            for pot in potential_list:
                commands.append(excecutable + ' ' + str(nb_record_per_run) 
                                + ' ' + str(alti_e) + ' ' + str(size) + ' ' + str(pot)
                                + ' ' + str(record_altitudes[0]) + ' ' + str(record_altitudes[1]) + ' ' + str(record_altitudes[2]))

################################################################################
print(len(commands))
print(commands[0])
#############

nb_thread = int(1) # number of threads (cpu) to run

commands2 = commands

command_number = len(commands2)

#print('Number of commands required '+ str(command_number))

pool = Pool(nb_thread) #
for i, returncode in enumerate(pool.imap(partial(call, shell=True), commands2)):
    if returncode != 0:
        print("%d command failed: %d" % (i, returncode))
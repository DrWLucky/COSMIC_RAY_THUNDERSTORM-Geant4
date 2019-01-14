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

scales = [1.5436, 1.9256, 2.4605, 3.2461, 4.4152, 6.0826, 8.3489, 11.4043]

efield_sizes = [2.0] # does not matter if efield_altitudes > 20

#potential_list = [40., 60., 80. ,100., 120., 140., 160.]
record_altitudes = [13.0]

efield_list = [0, 0.25, 0.5, 0.75, 1., 1.25, 1.5] # fraction of RREA threshold

# converting fraction of RREA threshold to potential

tilt_list = np.array([0.]) # does not matter if efield_altitudes > 20
#tilt_list = np.array([0.,25.,45.])

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
        
            potential_list_0 = np.array([200.0, 225.])
            
            for pot in potential_list_0:
                for tilt in tilt_list:
                    commands.append(excecutable + ' ' + str(seedd) + ' ' + str(nb_record_per_run) 
                                    + ' ' + str(alti_e) + ' ' + str(size) + ' ' + str(pot) + ' ' + str(tilt)
                                    + ' ' + str(record_altitudes[0]))
                    seedd+=1

################################################################################
print(len(commands))
print(commands[0])
#############

nb_thread = int(4) # number of threads (cpu) to run

commands2 = commands

command_number = len(commands2)

#print('Number of commands required '+ str(command_number))

pool = Pool(nb_thread) #
for i, returncode in enumerate(pool.imap(partial(call, shell=True), commands2)):
    if returncode != 0:
        print("%d command failed: %d" % (i, returncode))
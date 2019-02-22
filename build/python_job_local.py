import platform
import subprocess as sp
import time
import re
from os import system
from os import walk
import numpy as np
from os import listdir
from mpi4py import MPI
from subprocess import call
from functools import partial
from multiprocessing.dummy import Pool
import numpy as np
import time
import sys
import pandas as pd

computer_name = platform.node()

#### functions definitions

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

################################################################################
### Defining commands to run

#efield_altitudes =  [9.,10.,11.,12.,13.,14.]# kilometers, record altitude

efield_altitudes = [6.0]# kilometers, record altitude

altitudes = [4.,      5.,     6.,     7.,     8.,     9.,     10.,     11.,     12.,     13.,      14.,      15.,      16.]
scales =    [1.5434,  1.7199, 1.9253, 2.1686, 2.4604, 2.8134, 3.2454,  3.7742,  4.4144,  5.1808,   6.0821,   7.1335,   8.3483]

altitudes = np.array(altitudes)
scales = np.array(scales)

efield_sizes = [1.0,2.0] # does not matter if efield_altitudes > 20

#potential_list = [40., 60., 80. ,100., 120., 140., 160.]

# 0.284 MV/m
RREA_thres = 284.0 # MV/km
frac_rrea_thres_list = np.array([0.0, 0.25, 0.5, 0.75, 1., 1.25, 1.5]) # fraction of RREA threshold

if ("iftrom" in computer_name) or ("7370" in computer_name) or ("sarria-pc" in computer_name):
    nb_run = 1
else :
    nb_run = 2000
#potential_list = [200.]

nb_record_per_run = 10000

# defining the commands to be run in parallel

commands=[]
excecutable = './mos_test'

for _ in range(nb_run):
    for size in efield_sizes:
        for alti_e in efield_altitudes:
        
            scale_fact = np.interp(alti_e, altitudes, scales)
            # converting "fraction of RREA" threshold to potential
            potential_list = RREA_thres * frac_rrea_thres_list * size / scale_fact
            
            #print(potential_list)
            
            record_altitudes = [alti_e, alti_e-size/2.0, alti_e+size/2.0]
            
            for pot in potential_list:
                commands.append(excecutable + ' ' + str(nb_record_per_run) 
                                + ' ' + str(alti_e) + ' ' + str(size) + ' ' + str(pot)
                                + ' ' + str(record_altitudes[0]) + ' ' + str(record_altitudes[1]) + ' ' + str(record_altitudes[2]))

################################################################################
#print(len(commands))
#print(commands[0])
#############

#######################################
if ("iftrom" in computer_name) or ("7370" in computer_name) or ("sarria-pc" in computer_name): # local (personal) computer
  
    nb_thread = 2 # number of threads (cpu) to run
    
    # Making an array where each element is the list of command for a given thread

    command_number = len(commands)

    print('Number of commands required '+ str(command_number))

    pool = Pool(nb_thread) # to be always set to 1 for this MPI case
    for i, returncode in enumerate(pool.imap(partial(call, shell=True), commands)):
        if returncode != 0:
            print("%d command failed: %d" % (i, returncode))

#######################################
else: # Cluster (fram)
    # MPI initializations and preliminaries
    comm = MPI.COMM_WORLD   # get MPI communicator object
    size = comm.Get_size()       # total number of processes
    rank = comm.Get_rank()       # rank of this process
    status = MPI.Status()   # get MPI status object

    # Define MPI message tags
    tags = enum('READY', 'DONE', 'EXIT', 'START')

    if rank == 0:
        # Master process executes code below
        tasks = commands
        task_index = 0
        num_workers = size - 1
        closed_workers = 0
        print("Master starting with %d workers" % num_workers)
        while closed_workers < num_workers:
            data = comm.recv(source=MPI.ANY_SOURCE, tag=MPI.ANY_TAG, status=status)
            source = status.Get_source()
            tag = status.Get_tag()
            if tag == tags.READY:
                # Worker is ready, so send it a task
                if task_index < len(tasks):
                    comm.send(tasks[task_index], dest=source, tag=tags.START)
                    print("Sending task %d to worker %d" % (task_index, source))
                    task_index += 1
                else:
                    comm.send(None, dest=source, tag=tags.EXIT)
            elif tag == tags.DONE:
                results = data
                print("Got data from worker %d" % source)
            elif tag == tags.EXIT:
                print("Worker %d exited." % source)
                closed_workers += 1

        print("Master finishing")
    else:
        # Worker processes execute code below
        name = MPI.Get_processor_name()
        print("I am a worker with rank %d on %s." % (rank, name))
        while True:
            comm.send(None, dest=0, tag=tags.READY)
            task = comm.recv(source=0, tag=MPI.ANY_TAG, status=status)
            tag = status.Get_tag()
        
            if tag == tags.START:
                # Do the work here
                task2=[task]
                pool = Pool(1) # to be always set to 1 for this MPI case
                for i, returncode in enumerate(pool.imap(partial(call, shell=True), task2)):
                    if returncode != 0:
                        print("%d command failed: %d" % (i, returncode))
                comm.send(returncode, dest=0, tag=tags.DONE)
            elif tag == tags.EXIT:
                break

    comm.send(None, dest=0, tag=tags.EXIT)
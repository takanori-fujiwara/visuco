Lennard-Jones input file for miniMD

lj             units (lj or metal)
none           data file (none or filename)
lj	       	   force style (lj or eam)
1.0 1.0        LJ parameters (epsilon and sigma; COMD: 0.167 / 2.315)
256 256 256       size of problem
500            timesteps
0.005          timestep size
1.44           initial temperature
0.8442         density
20             reneighboring every this many steps
2.5 0.30       force cutoff and neighbor skin
100            thermo calculation every this many steps (0 = start,end)

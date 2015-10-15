SAGA
========

author: Rafael Alves Batista

email: rafael.alvesbatista [at] physics.ox.ac.uk


SAGA ([S]QLite [A]MR [G]rid [A]pplication) is a simple code to allow [CRPropa 3](https://github.com/CRPropa/CRPropa3) and [GRPropa] (https://github.com/rafaelab/GRPropa) to read magnetohydrodynamical simulations of the cosmic web obtained with the [RAMSES](http://magnet.ens.fr/?ramses-mhd) code.

Reading a full MHD simulation is memory and time consuming. Therefore, we convert the RAMSES output into a SQLite 3 R-Tree. By doing so, it is possible to query the database containing magnetic field strengths and baryon density faster than with a C++ code without the need to load the whole grid into the memory. This approach relies on the efficient data structure of the R-tree, which indexes each cell according to its position, hence optimising the performance of the code when querying the database.

For information on how to install and use the code, see the [wiki](https://github.com/rafaelab/saga/wiki).

This code is still being tested. I do not assume responsability for the results obtained with it. I recommend thorough tests before publishing any scientific results obtained using SAGA.

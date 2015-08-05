SAGA
========

author: Rafael Alves Batista

email: rafael.alves.batista [at] desy.de

first public release: 20/07/2014


SAGA ([S]QLite [A]MR [G]rid [A]pplication) is a simple code to allow [CRPropa 3](https://github.com/CRPropa/CRPropa3) and [GRPropa] (https://github.com/rafaelab/GRPropa) to read magnetohydrodynamical simulations of the cosmic web done using the [RAMSES](http://magnet.ens.fr/?ramses-mhd) code.

Reading a full MHD simulation is memory and time consuming, so here we use convert the RAMSES output into a SQLite 3 R-Tree. By doing this, it will be possible to query the database containing magnetic field strengths and baryon density much faster than with a C++ code. This approach relies on the efficient data structure of the R-tree, which indexes each cell according to its position, hence optimizing the performance of the queries.

For information on how to install and use the code, see the [wiki](https://github.com/rafaelab/saga/wiki).

This code is still being tested. I do not assume responsability for the results obtained with it. I recommend thorough tests before publishing any scientific results obtained using SAGA.

SAGA

author: Rafael Alves Batista
email: rafael.alves.batista [at] desy.de
first public release: 20/07/2014


SAGA ([S]QLite [A]MR [G]rid [A]pplication) is a simple code to allow CRPropa 3 to read 
magnetohydrodynamical simulations of the cosmic web done using the RAMSES code.

Reading a full MHD simulation with CRPropa is memory and time consuming, so here we use 
a simple script to convert the RAMSES output into a SQLite 3 R-Tree. By doing this, 
CRPropa will be able to query the database containing magnetic field strengths and baryon 
density much faster than with a C++ code. This approach relies on the efficient data 
structure of the R-tree, which indexes each cell according to its position, hence 
optimizing the performance of the queries.


CRPropa: https://github.com/CRPropa/CRPropa3

RAMSES: http://magnet.ens.fr/?ramses-mhd


* This code is still being tested. I do not assume responsability for anything. 
I recommend a thorough test of this code before publishing any scientific results
obtained from it.

# Parallel Communication Data in Supercomputers

## About
Communication datasets including routes and message size information between compute nodes/cores. These datasets were collected for the paper:

***A Visual Analytics System for Optimizing Communications in Massively Parallel Applications***    
Takanori Fujiwara, Preeti Malakar, Venkatram Vishwanath, Khairi Reda, Michael E. Papka, and Kwan-Liu Ma    
In Proceedings of IEEE Symposium on Visual Analytics Science and Technology (VAST), 2017.

The system used in the paper above is also available.    
***Visuco***    
 https://github.com/takanori-fujiwara/visuco (under work for cleaning the code)

## Description of datasets
### Directories
route
* has communication route information files including a count of hops, MPI ranks of source and destination, MPI ranks used in a route, and physical coordinates of the participatng MPI ranks.

hopbyte
* has a message size and a number of hops information files including MPI ranks of source and destination.

argument
* has an information about arguments used to run applications for obtaining each data

### File names
In route and hopbyte directories, we use a file naming rule below.

*"Application Name"*\_*"System Name"*\_n*"Number of Compute Nodes Used"*\_c*"Number of Cores Used"*\_*"Scaling Tested"*_*"Directory Name"*
.txt

For example, MiniMD_Mira_n1024_c4_w_hopbyte.txt means hopbyte information file collected by running MiniMD on Mira with 1,024 nodes and 4 cores, and to see weak scaling.

We used w: weak scaling, s1: strong scaling with small size problem, and s2: strong scaling with large size problem as "Scaling Tested".

### File formats
#### Route File
Each line has a format below:
* Hop *"Hop Count"*: [*"Source MPI Rank"*-*"Destination MPI Rank"*] *"Intermediate Rank From"* (*"Its Coordinates"*) -> *"Intermediate Rank To"* (*"Its Coordinates"*)

For example,  
Hop 1: [12-0] 12 (0 1 1 0 0 0) -> 8 (0 1 0 0 0 0)  
Hop 2: [12-0] 8 (0 1 0 0 0 0) -> 0 (0 0 0 0 0 0)  
mean that this is a communication route from MPI rank 12 to 0, which uses 12 to 8 as the first hop and 8 to 0 as the second hop. MPI rank 12 has (0 1 1 0 0 0) as its coordinates and so on.

#### Hopbyte File
Each column separated by space has information below.  
* Source MPI Rank, Destination MPI Rank, Message Size (Byte), and Number of Hops.

## Applications Used
* MiniMD and MiniAMR
    * https://mantevo.org  
* IMB-MPI1
    * https://software.intel.com/en-us/articles/intel-mpi-benchmarks  

## Systems Used
* Theta, Mira, and Vesta
    * https://www.alcf.anl.gov/computing-resources

## How to Cite
Please, cite:
* Takanori Fujiwara, Preeti Malakar, Venkatram Vishwanath, Khairi Reda, Michael E. Papka, and Kwan-Liu Ma, "A Visual Analytics System for Optimizing Communications in Massively Parallel Applications." In Proceedings of IEEE Symposium on Visual Analytics Science and Technology (VAST), 2017.

## Lisence
[![CC-BY](https://licensebuttons.net/l/by/3.0/88x31.png)](https://licensebuttons.net/l/by/3.0/88x31.png)
This work is available under a [CC-BY]( http://creativecommons.org/licenses/by/4.0/) license.   

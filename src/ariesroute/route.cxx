/*
 *  Created by Preeti Malakar
 *  Argonne National Laboratory
 *
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <tuple>
#include <map>
#include <mpi.h>
#include <pmi.h>
#include "route.h" 

const int MAX_DIMS = 5;

/*
 * Ranks per node, Core ID (0...15) 
 */ 
//int ppn, coreID, nodeID;

/*
 * Size of each dimension 
 */ 
int dimSize[MAX_DIMS];	// torus dimension size

/*
 *  Routing order : IN_ORDER for now 
 */
int routingOrder[] = {4, 3, 2, 1, 0};

inline int min (int x, int y) {
  return x<y? x: y;
}

typedef std::tuple <int,int,int,int,int> tupleCoords;
std::map<tupleCoords, int> nodeMap;

struct nidInfo {
  int processor_id, rank, cab_position, cab_row, cage, slot, cpu;
}*nidInfoList;

void Nid2Coords (int *childnid, int *childCoords) {

  childCoords[0] = nidInfoList[*childnid].cab_position;
  childCoords[1] = nidInfoList[*childnid].cab_row;
  childCoords[2] = nidInfoList[*childnid].cage;
  childCoords[3] = nidInfoList[*childnid].slot;
  childCoords[4] = nidInfoList[*childnid].cpu;
}

void Coords2Nid (int *intmdtCoords, int *intmdtNid) {
  auto t = std::make_tuple(intmdtCoords[0], intmdtCoords[1], intmdtCoords[2], intmdtCoords[3], intmdtCoords[4]);
  auto search = nodeMap.find(t);
  if(search != nodeMap.end()) { 
      *intmdtNid = search->second;
  //      std::cout << "Found " << search->second << '\n';
  }
  else { 
      std::cout << "Not found\n";
      printf("%d %d %d %d %d\n", intmdtCoords[0], intmdtCoords[1], intmdtCoords[2], intmdtCoords[3], intmdtCoords[4]);
  }
}

void getRoute(int srcRank, int destRank, char *path) {

	//local variables
  int i, unitHop = 1, rank, nodeid; 
  int nid, srcNid, destNid, rc;
  int srcCoords[6], destCoords[6], coord[6];
	int childCoords[6], intmdtCoords[6];
	char buf[64];

  PMI_BOOL initialized;
  rc = PMI_Initialized(&initialized);
  if (rc!=PMI_SUCCESS)
    PMI_Abort(rc,"PMI_Initialized failed");

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *nidofrank = new int[size]; // total #nodes
  //rankofnid = {-1};

  for (int rank=0; rank<size ; rank++) {
    rc = PMI_Get_nid(rank, &nid);
 //   printf ("rank %d, nid %d ", rank, nid);
    if (rc!=PMI_SUCCESS)
      PMI_Abort(rc,"PMI_Get_nid failed");
    nidofrank[rank] = nid;
  }

  nidInfoList = (struct nidInfo *) malloc (4000 * sizeof (struct nidInfo));

  //read the nodetable file
  FILE *fp = fopen ("nodetable", "r");
  //FILE *fp = fopen ("theta.cnodetable", "r");
  while(fscanf (fp, "%d %d %d %d %d %d %d", &rank, &nodeid, &coord[0], &coord[1], &coord[2], &coord[3], &coord[4]) != EOF) {

   rc = PMI_Get_nid(srcRank, &srcNid);
   rc = PMI_Get_nid(destRank, &destNid);
   
   //if (srcRank == rank) {
   if (srcNid == nodeid) {
     //printf ("src %d %d %d %d %d %d %d\n", rank, nodeid, coord[0], coord[1], coord[2], coord[3], coord[4]);
     srcCoords[0]=coord[0];
     srcCoords[1]=coord[1];
     srcCoords[2]=coord[2];
     srcCoords[3]=coord[3];
     srcCoords[4]=coord[4];
   }
   //else if (destRank == rank) {
   else if (destNid == nodeid) {
     //printf ("dest %d %d %d %d %d %d %d\n", rank, nodeid, coord[0], coord[1], coord[2], coord[3], coord[4]);
     destCoords[0]=coord[0];
     destCoords[1]=coord[1];
     destCoords[2]=coord[2];
     destCoords[3]=coord[3];
     destCoords[4]=coord[4];
   }

   //rank = rankofnid[nodeid];
   nodeMap[std::make_tuple(coord[0], coord[1], coord[2], coord[3], coord[4])] = nodeid;

	 nidInfoList[nodeid].processor_id = nodeid; 
	 //nidInfoList[nodeid].rank = rank; 
   nidInfoList[nodeid].cab_position = coord[0]; 
   nidInfoList[nodeid].cab_row = coord[1]; 
   nidInfoList[nodeid].cage = coord[2]; 
   nidInfoList[nodeid].slot = coord[3]; 
   nidInfoList[nodeid].cpu = coord[4];  
   
	 //printf ("%d node %d %d %d %d %d %d\n", srcRank, nodeid, coord[0], coord[1], coord[2], coord[3], coord[4]);
  }

  fclose(fp);

  srcCoords[5] = 0;
  destCoords[5] = 0;
  intmdtCoords[5] = 0;

#ifdef DEBUG
	printf("Source rank: %d coords = (%u,%u,%u,%u,%u,%u) : %d\n", srcRank, srcCoords[0], srcCoords[1], srcCoords[2], srcCoords[3], srcCoords[4], srcCoords[5], destRank);
#endif

	//Initialize intermediate nodes in original path to the destination node
	for (int dim=0; dim < MAX_DIMS; dim++) 
		intmdtCoords[dim] = srcCoords[dim];
	   
//	intmdtCoords[MAX_DIMS] = destCoords[MAX_DIMS];	//T

	int hopnum = 0;
	int hopDiff, intmdtNid, child, parent;

	child = srcNid;

	for (int dim=0; dim<MAX_DIMS; dim++) {

		int dimID = routingOrder[dim];
		hopDiff = abs(destCoords[dimID] - srcCoords[dimID]);
    if (hopDiff > 1) hopDiff = 1;

#ifdef DEBUG
		printf("%d to %d difference in dim %d = %d\n", srcNid, destNid, dimID, hopDiff);
#endif


		for(int diff=0; diff<hopDiff ;diff++) {

/*
				if(destCoords[dimID] < srcCoords[dimID]) 
					intmdtCoords[dimID] -= unitHop;  
				else intmdtCoords[dimID] += unitHop;
*/

      intmdtCoords[dimID] = destCoords[dimID];			
			++hopnum;

			//get the rank
			Coords2Nid (intmdtCoords, &intmdtNid);
			parent = intmdtNid;

#ifdef DEBUG
			printf ("Enroute %d (%d %d %d %d %d %d) to %d (%d %d %d %d %d %d) Hop %d: in dimension %d Child %d to Parent %d (%d %d %d %d %d %d)\n", \
			srcNid, srcCoords[0], srcCoords[1], srcCoords[2], srcCoords[3], srcCoords[4], srcCoords[5], \
			destNid, destCoords[0], destCoords[1], destCoords[2], destCoords[3], destCoords[4], destCoords[5], \
			hopnum, dimID, child, \
      intmdtNid, intmdtCoords[0], intmdtCoords[1], intmdtCoords[2], intmdtCoords[3], intmdtCoords[4], intmdtCoords[5]);
#endif

//			sprintf(buf, "%d ", child);
//			strcat(path, buf); 

#ifdef DEBUG
			//printf ("Route %d to %d Hop %d\n", srcRank, intmdtRank, hopnum);
			printf ("Route %d to %d Hop %d\n", child, parent, hopnum);
			printf ("%d->%d;\n", hopnum, child, parent);
#endif
			Nid2Coords (&child, childCoords);

			printf ("Hop %d: [%d-%d] %d (%d %d %d %d %d %d) -> %d (%d %d %d %d %d %d)\n", \
      hopnum, srcNid, destNid, child, \
      childCoords[0], childCoords[1], childCoords[2], childCoords[3], childCoords[4],  childCoords[5], parent, \
      intmdtCoords[0], intmdtCoords[1], intmdtCoords[2], intmdtCoords[3], intmdtCoords[4], intmdtCoords[5]); 

			child = parent;
		}
	}   

//	sprintf(buf, "%d", destRank);
//	strcat(path, buf); 

//#ifdef DEBUG
//	printf ("path: %s\n", path);
//#endif

	return;
			
}


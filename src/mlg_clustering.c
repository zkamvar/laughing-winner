/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# This software was authored by Zhian N. Kamvar and Javier F. Tabima, graduate
# students at Oregon State University; and Dr. Nik Grünwald, an employee of
# USDA-ARS.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for educational, research and non-profit purposes, without fee,
# and without a written agreement is hereby granted, provided that the statement
# above is incorporated into the material, giving appropriate attribution to the
# authors.
#
# Permission to incorporate this software into commercial products may be
# obtained by contacting USDA ARS and OREGON STATE UNIVERSITY Office for
# Commercialization and Corporate Development.
#
# The software program and documentation are supplied "as is", without any
# accompanying services from the USDA or the University. USDA ARS or the
# University do not warrant that the operation of the program will be
# uninterrupted or error-free. The end-user understands that the program was
# developed for research purposes and is advised not to rely exclusively on the
# program for any reason.
#
# IN NO EVENT SHALL USDA ARS OR OREGON STATE UNIVERSITY BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
# LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
# EVEN IF THE OREGON STATE UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE. USDA ARS OR OREGON STATE UNIVERSITY SPECIFICALLY DISCLAIMS ANY
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY
# WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
# BASIS, AND USDA ARS AND OREGON STATE UNIVERSITY HAVE NO OBLIGATIONS TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

SEXP farthest_neighbor(SEXP dist, SEXP mlg, SEXP threshold)
{

  // TODO: More descriptive header
  // TODO: Implement wrapper function in R
  // TODO: Make unit tests
  // TODO: Clean up code. Get rid of commented out code.
  // TODO: Input validation
  // TODO: Remove empty genotypes before returning.
  // TODO: Order genotypes by size before returning?

  // Assumptions:
  //  dist is an n by n matrix containing distances between individuals
  //  mlg is a vector of length n containing mlg assignments
  //  threshold is a real
  //  dist as attr "dim" of (n,n)

  int num_individuals; // n above; total number of individuals
  int num_clusters; // Number of clusters; updated upon merge
  int num_mlgs; // Number of clusters already present
  int cur_mlg;  // Temporary number used for tracking mlgs
  double thresh; // Threshold for distance under which mlgs are clones
  double min_cluster_distance; // Used in finding the closest cluster pairing
  double tmp_max;     // Used in finding the closest cluster
  int closest_pair[2]; // Used in finding pair of clusters closest together
  int** cluster_matrix;
  int** cluster_distance_matrix;
  int* cluster_size; // Size of each cluster

  SEXP Rout;
  SEXP Rdim;

  thresh = REAL(threshold)[0];
  Rdim = getAttrib(dist, R_DimSymbol);
  num_individuals = INTEGER(Rdim)[0]; // dist is a square matrix
  PROTECT(Rout = allocVector(INTSXP, num_individuals));  

  // TODO: Limit memory usage by counting initial mlgs from mlg
  num_mlgs = num_individuals;
  // Allocate empty matrix for storing clusters
  cluster_matrix = (int**)malloc(num_mlgs * sizeof(int*));
  cluster_distance_matrix = (int**)malloc(num_mlgs * sizeof(int*));
  if(cluster_matrix == NULL || cluster_distance_matrix == NULL)
  {
    // TODO: Throw error and exit
  }
  for(int i = 0; i < num_mlgs; i++)
  {
    cluster_matrix[i] = (int*)malloc(num_individuals * sizeof(int));
    memset(cluster_matrix[i], -1, sizeof(int)*num_individuals);
    cluster_distance_matrix[i] = (int*)malloc(num_individuals * sizeof(int));
    memset(cluster_distance_matrix[i], -1, sizeof(int)*num_individuals);
    if(cluster_matrix[i] == NULL || cluster_distance_matrix[i] == NULL)
    {
      // TODO: Throw error and exit
    }
  }
  // Allocate memory for storing sizes of each cluster
  // calloc clears the memory allocated to 0s
  cluster_size = (int*)calloc(num_mlgs, sizeof(int));
  if(cluster_size == NULL)
  {
    // TODO: Throw error and exit
  }

  // Fill initial clustering matrix via mlg
  // Steps through mlg.
  // Adds the index of each individual to cluster_matrix
  // Increments the cluster size for each individual added
  // For each new cluster added, increments num_clusters
  num_clusters = 0;
  for(int i = 0; i < num_individuals; i++)
  {
    cur_mlg = REAL(mlg)[i]; // Get the initial cluster of this individual / Casts as int implicitly
    // Insert base 1 index of individual in the next open cell for its cluster  
    cluster_matrix[cur_mlg-1][cluster_size[cur_mlg-1]] = i;
    cluster_size[cur_mlg-1]++;

    // If this is the first individual in this cluster, increment num_clusters
    if(cluster_size[cur_mlg-1] == 1)
    {
      num_clusters++;
    }
  }

  // Main processing loop.
  // Finds the two closest cluster
  // then merges them together if they are within threshold of each other
  // Repeats until no clusters are withing threshold of each other
  //  or only one cluster remains
  min_cluster_distance = -1;
  while(min_cluster_distance < thresh && num_clusters > 1)
  {
    // Fill the distance matrix from cluster to cluster
    // Merge the two with the smallest distance and refill the matrix
    //  until the smallest distance is greater than the threshold 
    min_cluster_distance = -1;
    closest_pair[0] = -1;
    closest_pair[1] = -1;
    // Step through each cluster
    // Upon completion the above three variables will hold:
    //  the min distance between two clusters
    //  the two clusters which are closest together
    for(int i_cluster = 0; i_cluster < num_mlgs; i_cluster++)
    {
      tmp_max = -1;
        
      // Step through each subsequent cluster
      for(int j_cluster = i_cluster+1; j_cluster < num_mlgs; j_cluster++)
      {
        // Step through each individual in the i_cluster cluster until an empty spot is found
        for(int k_individual = 0; k_individual < num_individuals && cluster_matrix[i_cluster][k_individual] != -1; k_individual++)
        {
          int k_individual_val = cluster_matrix[i_cluster][k_individual];
          // Step through each individual in this cluster as well
          for(int l_individual = 0; l_individual < num_individuals && cluster_matrix[j_cluster][l_individual] != -1; l_individual++)
          {
            int l_individual_val = cluster_matrix[j_cluster][l_individual];
            // TODO: Clean up and explain the indexing here
            if(REAL(dist)[k_individual_val*num_individuals+l_individual_val] > tmp_max)
            {
              tmp_max = REAL(dist)[k_individual_val*num_individuals + l_individual_val];
              //printf("tmp_max is now %f\n", tmp_max);
            }
          }
        }
        // Check to see if this cluster could be the closest
        // Make sure not to count empty clusters (tmp_max ~= -1)
        if(tmp_max > -0.5 && (tmp_max < min_cluster_distance || min_cluster_distance < 0))
        {
          //printf("Closest pair is now: %d,%d at %f\n", i_cluster, j_cluster, tmp_max);
          min_cluster_distance = tmp_max;
          closest_pair[0] = i_cluster;
          closest_pair[1] = j_cluster;
        }
      }
    }
    //printf("The closest pair is: %i and %i at %f\n", closest_pair[0], closest_pair[1], min_cluster_distance);

    // Merge the two closest clusters together into the lower numbered cluster
    //  if they are withing the threshold distance from each other
    if(min_cluster_distance < thresh)
    {
      
      for(int i = 0; i < cluster_size[closest_pair[1]]; i++)
      {
        cluster_matrix[closest_pair[0]][cluster_size[closest_pair[0]]] = cluster_matrix[closest_pair[1]][i];
        cluster_matrix[closest_pair[1]][i] = -1;
        cluster_size[closest_pair[0]]++;
      }
      cluster_size[closest_pair[1]] = 0;
      num_clusters--;
    }
  }

  //printf("numClusters: %d\n", num_clusters);
  // Fill return vector
  for(int i = 0; i < num_mlgs; i++)
  {
    for(int j = 0; j < num_individuals && cluster_matrix[i][j] > -0.5; j++)
    {
      //printf("%d ",cluster_matrix[i][j]);
      INTEGER(Rout)[cluster_matrix[i][j]] = i+1;
    }
    //printf("\n");
  }
  

  // Free memory allocated for cluster_matrix
  for(int i = 0; i < num_mlgs; i++)
  { 
    free(cluster_matrix[i]);
    free(cluster_distance_matrix[i]);
  }
  free(cluster_matrix);
  free(cluster_distance_matrix);
  free(cluster_size);
  UNPROTECT(1);
  
  return Rout;
}



#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
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
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#
#==============================================================================#
#' Calculate a distance matrix comparing samples based on the number of alleles
#' that differ in zygosity.
#' 
#' This function does pairwise comparisons between diploid samples in a genlight
#' object. The number representing the distance between two samples is equal to
#' the number of alleles in the samples that do not have the same zygosity.
#' 
#' @rdname bitwise_distance
#'
#' @param x a genlight object. 
#'
#' @param percent \code{logical}. Should the distance be represented from 0 to 1? 
#' Default set to \code{TRUE}. \code{FALSE} will return the distance represented 
#' as integers from 1 to n where n is the number of loci. 
#'  
#' @param mat \code{logical}. Return a matrix object. Default set to  
#' \code{FALSE}, returning a dist object. \code{TRUE} returns a matrix object. 
#'
#' @param missing_match \code{logical}. Determines whether two samples differing
#' by missing data in a location should be counted as matching at that location.
#' Default set to \code{TRUE}, which forces missing data to match with anything. 
#' \code{FALSE} forces missing data to not match with any other information. 
#' 
#' @return Pairwise distances between individuals present in the genlight object.
#' @author Zhian N. Kamvar, Jonah Brooks
#' 
#' @export
#==============================================================================#
bitwise.dist <- function(x, percent=TRUE, mat=FALSE, missing_match=TRUE){
  stopifnot(class(x)[1] == "genlight")
  stopifnot(ploidy(x) == 2)
  ploid     <- 2
  ind.names <- x@ind.names
  inds      <- length(x@gen)
  numPairs   <- length(x@gen[[1]]@snp[[1]])*8

  pairwise_dist <- .Call("bitwise_distance", x, missing_match)
  
  dist.mat <- pairwise_dist
  dim(dist.mat) <- c(inds,inds)
  colnames(dist.mat)            <- ind.names
  rownames(dist.mat)            <- ind.names
  if (percent){
    dist.mat <- dist.mat/(numPairs)
  }
  if (mat == FALSE){
    dist.mat <- as.dist(dist.mat)
  }
  return(dist.mat)
}


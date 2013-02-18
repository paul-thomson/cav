/*
 * test.cpp
 *
 *  Created on: 14 Feb 2013
 *      Author: s0942340
 */

#include "test.h"
/* sscanf example */
/* sscanf example */
#include <stdio.h>

int main2 ()
{
  char sentence []="0 -0.0881862 -0.223678 -0.929536 -1";
  int i;
  float p1;
  float p2;
  float p3;
  int prev;

  sscanf (sentence,"%i %f %f %f %i",&i, &p1, &p2, &p3, &prev);
  printf ("%i %f %f %f %i",i, p1, p2, p3, prev);

  return 0;
}


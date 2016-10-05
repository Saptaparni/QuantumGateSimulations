#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>
#include "alfred_types.h"
#include "sim.h"

/* implicit parameters, manipulated by the routines below */
int Nbits =  -1; /* number of qubits used */
cplx *state = 0; /* state vector */ 
double thres = 0.001; /* numbers 0<=fabs(r)<thres are considered tiny */

#define istiny(x) ( (fabs((x))<thres) ? 1: 0 )

#define sq(x) ( (x)*(x) ) 

/* macros for complex number arithmetic */ 
#define add(a, b) ( (cplx) {(a).re+(b).re, (a).im+(b).im } )

#define mult(a, b) ((cplx) {(a).re*(b).re-(a).im*(b).im, \
  (a).re*(b).im+(a).im*(b).re }) 



/*Multiply a 2 by 2 matrix with a column vector with 2 rows*/
void multiplyMatrixAndVector(int Nbits, mat g,
                             int position1, int position2,
                             cplx* tempVector)
{
     cplx vect1, vect2;
     vect1 = add(mult(g[0],tempVector[position1]),
                 mult(g[1],tempVector[position2]));
     vect2 = add(mult(g[2],tempVector[position1]),
                 mult(g[3],tempVector[position2]));
     tempVector[position1] = vect1;
     tempVector[position2] = vect2;
}


/* manipulate state size */
cplx *init_state(int bits) {
  Nbits = bits;
  return calloc(1<<bits, sizeof(cplx));
}

cplx *resize_state(int bits, cplx *st) { 
  /* enlarge or reduce to *bits* qubits */ 
  int i;
  if(bits <= Nbits) { 
    st = realloc(st, (1<<bits)*sizeof(cplx)); 
  } else { 
    st = realloc(st, (1<<bits)*sizeof(cplx));
    for(i=1<<Nbits; i < (1<<bits); i++)
      st[i] = (cplx){ 0.0, 0.0};
  }
  Nbits = bits;
  return st;
}

/* manipulation of state content */
void clear_state() { 
  memset(state, 0, (1<<Nbits)*sizeof(cplx));
} 

void normalize_state() { 
  double len=0; 
  int i;
  for(i=0; i<(1<<Nbits); i++) 
    len += sq(state[i].re)+sq(state[i].im);
  len = 1.0/sqrt(len);  
  for(i=0; i<(1<<Nbits); i++) {
    state[i].re *= len;
    state[i].im *= len;
  }  
}

/* input: a single bit set at the position of the gate; 
   this bitposition is measured and the state vector  
   is changed according to the postulates of quantum 
   mechanics. 
   NOTE: the state must be normalized to length 1 */

/* complete the missing code */

/* compute the probability to observe 0,
 * determine whether 0 or 1 should be observed in this measurement
 * then modify state to produce the post measurement state
 * of the quantum computer
 */

void measure_state(int pos, cplx *state) {
     int i;
     double ran;
     double prob=0.0;
     double prob1;
     normalize_state();
     /*Assuming that drand48() gives the probability we observed a 0*/
     ran = drand48();
     int position = (log2(pos));
     for(i=0; i<(1<<Nbits); i=i+(2*pos)){
          for (int j=0; j<pos; j++) {
               prob1 = sq(state[i+j].re)+sq(state[i+j].im);
               prob += prob1;
          }
     }
    if (ran <= prob) {
          /*Modify state as if we observed 0: */
          for(i=0; i<(1<<Nbits); i=i+(2*pos)){
               for (int j=pos; j<2*pos; j++) {
                    state[i+j].re = 0.0;
                    state[i+j].im = 0.0;
               }
          }
     }
     else{
          /*Modify state as if we observed 1:*/
          for(i=0; i<(1<<Nbits); i=i+(2*pos)){
               for (int j=0; j<pos; j++) {
                    state[i+j].re = 0.0;
                    state[i+j].im = 0.0;
               }
          }
     }
     normalize_state();
 }



void set_bit(int pos, cplx *state) {
  int i=0;
  int limit = 1<<Nbits;

  for(i = 0; (state[i].re == 0.0) && (state[i].im == 0.0) ; i++);
  /* state[i] is now the first nonzero entry */

  if( !(i & pos) ) { /* bit is not set */
    for(  ; i< limit; i++) { /* swap entries */
      if( i & pos) { continue; } /* skip this index */ 
      state[ i|pos ].re = state[i].re;
      state[ i|pos ].im = state[i].im;
      state[i].re = 0.0;
      state[i].im = 0.0;  
    }
  } 
  print_state();
}

void erase_bit(int pos, cplx *state) {
  int i=0;
  int limit = 1<<Nbits;

  for(i = 0; (state[i].re == 0.0) && (state[i].im == 0.0) ; i++);
  /* state[i] is now the first nonzero entry */
 
  if( i & pos ) { /* bit is set */
    for(  ; i< limit; i++) { /* swap entries */
      if( i & pos ) { 
	state[i-pos].re = state[i].re;
	state[i-pos].im = state[i].im;
	state[i].re = 0.0;
	state[i].im = 0.0;
      }
    }
  } 
}

/* print a single ket in binary format */
void print_label(int d) { 
  int i;
  printf("|");
  for(i=Nbits-1; i>=0; i--) 
    printf((d & 1<<i) ? "1": "0"); 
  printf(")");
}

/* print the complete state, supressing
   tiny numbers */ 

/* sign of a float with exception for the 
   first occurence, where the + sign is 
   supressed */ 
#define sign(x) ( ((x)<0) ? "-" : \
   (first) ? "" : "+" ) 

void print_state(void) {
int i;
int first = 1;
double re, im;
  for(i=0; i< (1<<Nbits); i++) { 
    re = state[i].re; im = state[i].im;
    switch( (istiny(re)<<1 | istiny(im)) ) { 
    case 0: /* print re and im */ 
      /* printf("%s%1.3g", sign(re),fabs(re));
      printf("%si*%.3g",sign(im),fabs(im));
      */ 
      if( !first ) { 
	printf("+");
      };
      printf("(%1.3g%si%.3g)",re,sign(im),fabs(im));
      print_label(i);
      first = 0;
      break;
    case 1: /* print re */
      printf("%s%0.3g",sign(re),fabs(re)); 
      print_label(i);
      first = 0;
      break;
    case 2: /* print im */ 
      printf("%si*%1.3g",sign(im),fabs(im)); 
      print_label(i);
      first = 0;
      break;
    case 3: /* print nothing */  
      ;
    }
  }
  printf("\n");
}

/* convert the label of a ket in binary format into
   an integer d, the address of the state */
int convert_label(char *str) { 
int i, d=0;
  if(strlen(str) != Nbits) { 
     fprintf(stderr,"length of qubit label is wrong\n");
     exit(1);
  } else { 
    for(i=0; i<Nbits; i++) { 
      if (str[Nbits-1-i]=='1') { d += (1<<i); } 
    }
  }
  return d;
}


/* Applying a conditioned g-gate to a state vector */ 
void applygate(int Nbits, int ocnd, int icnd, int gpos, 	       
               mat g, cplx *state  ) { 
     int i,j;
     cplx* tempVector = calloc(1<<Nbits, sizeof(cplx));
     /*Store the value of the state in a temporary vector
      and make changes to this vector and copy its value to 
      the state vector in the end */
     for (i=0; i< 1<<Nbits; i++) {
          tempVector[i].re = state[i].re;
          tempVector[i].im = state[i].im;
     }
     /* Check if the gate is conditioned*/
     int isConditioned = ((ocnd == 0 && icnd == 0 )? 0:1);
     /* If the i and o conditions are not set. The matrix is
     applied to the entire state vector based on the
     target bit only
     The matrix is applied to two positions in the state
     vector at a time. this was done after finding a careful
     generalization */
     for (i=0; i< 1<<Nbits; i = i+2*gpos) {
          for (j=0; j<gpos; j++) {
               int k = i+j;
               /* If the i and o conditions are set. The matrix is
                applied to selected elements of the state vector
                based on the target bit and the i and o conditions 
                using the conditionsMet varuable */
               int conditionsMet = ((k & icnd) == icnd && //Is icnd met?
                                   (k & ocnd) == 0 &&     //Is ocnd met?
                                   (k & gpos) == 0);      //Is the gpos bit 0?
               /*Check if the gate is unconditioned or the 
                conditions are met for this element*/
               if (isConditioned == 0 || conditionsMet) {
                    multiplyMatrixAndVector( Nbits, g, k, k+gpos, tempVector);
               }
          }
     }
    for (int i=0; i< 1<<Nbits; i++){
          state[i].re = tempVector[i].re;
          state[i].im = tempVector[i].im;
     }
     normalize_state();
}






//printf("\n in multiplyMatrixAndVector: \n position1 = %d, position2 = %d,matrix : [%f+i%f %f+i%f \t %f+i%f %f+i%f\n Temp vector = [ ",position1,position2, g[0].re,g[0].im,g[1].re,g[1].im,g[2].re,g[2].im,g[3].re,g[3].im);
/*for (int i=0; i< 1<<Nbits; i++) {
 //
 printf(" %f + i%f \t",tempVector[i].re, tempVector[i].im);
 tempVector[i].re = 1.0;
 tempVector[i].im = 2.0;
 }
 printf("]\n ");*/



//printf("\n*********************************Start first Multiply***********************************\n ");
//multiplyMatrixAndVector( Nbits, g, 0, 1, tempVector);
//printf("\n*********************************Start second Multiply***********************************\n ");
//multiplyMatrixAndVector( Nbits, g, 4, 5, tempVector);
//printf("\n in applygate: TempVector = \n[ ");
/*for (int i=0; i< 1<<Nbits; i++) {
 //
 printf(" %f + i%f \t\n",tempVector[i].re, tempVector[i].im);
 }
 printf("]\n ");*/

//printf("\n in multiplyMatrixAndVector: \n position1 = %d, position2 = %d,matrix : [%f+i%f %f+i%f \t %f+i%f %f+i%f\n Temp vector = [ ",position1,position2, g[0].re,g[0].im,g[1].re,g[1].im,g[2].re,g[2].im,g[3].re,g[3].im);



//for (int i=0; i< 1<<Nbits; i++) {
//
// printf(" tempVector[%d] =  %f + i%f \t",position1,tempVector[position1].re, tempVector[position1].im);
//printf(" tempVector[%d]  = %f + i%f \t",position2,tempVector[position2].re, tempVector[position2].im);
//tempVector[i].re = 1.0;
// tempVector[i].im = 2.0;
//}
//printf("]\n ");*/



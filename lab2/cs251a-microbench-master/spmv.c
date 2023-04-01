#include <stdio.h>
#include "spmvArr.h"

void spmv(float val[NNZ], int cols[NNZ], int rowDelim[N_ROWS+1], float vec[N_COLS], float (*out)[N_ROWS]){
  int i, j;
  float sum, Si;

  for(i = 0; i < N_ROWS; i++){
    sum = 0; Si = 0;
    int tmp_begin = rowDelim[i];
    int tmp_end = rowDelim[i+1];
    for (j = tmp_begin; j < tmp_end; j++){
        Si = val[j] * vec[cols[j]];
        sum = sum + Si;
    }
    (*out)[i] += sum;
  }
}

 
int main(int argc, char* argv[]) {
  float out[N_ROWS];
  for(int i = 0; i < 3; ++i) {
    spmv(val,cols,row_delim,vec,&out);
  }

  if(argc>=2) {
    printf("random_elem:%f, %s\n", out[1+argc],argv[0]);
  }
}

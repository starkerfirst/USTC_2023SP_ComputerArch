#include <stdio.h>

#define row_size 128
#define N row_size*row_size
#define block_size 8

//Matrix multiply:  prod = m1 x m2 
float m1[N];
float m2[N];
float prod[N];

void mm() {
    //source machsuite
    int i, k, j, jj, kk;
    int i_row, k_row;
   
    for (jj = 0; jj < row_size; jj += block_size){
        for (kk = 0; kk < row_size; kk += block_size){
            for ( i = 0; i < row_size; ++i){
                for (k = 0; k < block_size; ++k){
                    i_row = i * row_size;
                    k_row = (k  + kk) * row_size;
                    for (j = 0; j < block_size; ++j){
                        prod[i_row + j + jj] += m1[i_row + k + kk] * m2[k_row + j + jj];
                    }
                }
            }
        }
    }
}

int main(int argc, char**argv) {
  mm();  

  if(argc>=2) {
    printf("random result:%f, %s\n", prod[argc],argv[0]);
  }

}


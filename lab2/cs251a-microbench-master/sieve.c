#include<stdio.h>
#include<stdlib.h>

int sieve(int n)  { 
    //source: https://www.geeksforgeeks.org/sieve-of-eratosthenes/
    char *notprime = (char*)malloc(sizeof(char) * n+1); 
  
    for (int p=2; p*p<=n; p++) { 
        // If prime[p] is not changed, then it is a prime 
        if (notprime[p] == 0) { 
            // Update all multiples of p 
            for (int i=p*2; i<=n; i += p) {
                notprime[i] = 1; 
            }
        }
    }

    int total=0;
    for(int p = 2; p < n; ++p) {
      total+=!notprime[p];
    }
    return total;
}

int main(int argc, char**argv) {
  int total = sieve(1000000);

  if(argc==2) {
    printf("sieve:%s %d\n", argv[0], total);
  }

  return 0;
}

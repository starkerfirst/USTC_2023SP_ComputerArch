#include <stdio.h>
#include "randArr.h"

#define ASIZE sizeof(randArr)/sizeof(int)

int temp[ASIZE];

void merge(int numbers[], int temp[], int left, int mid, int right) {
  int i, left_end, num_elements, tmp_pos;
 
  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;
 
  while ((left <= left_end) && (mid <= right)) {
    if (numbers[left] <= numbers[mid]) {
      temp[tmp_pos++] = numbers[left++];
    } else {
      temp[tmp_pos++] = numbers[mid++];
    }
  }
 
  while (left <= left_end) {
    temp[tmp_pos++] = numbers[left++];
  }
  
  while (mid <= right) {
    temp[tmp_pos++] = numbers[mid++];
  }
 
  for (i=0; i <= num_elements; i++) {
    numbers[right] = temp[right];
    right = right - 1;
  }
}

void m_sort(int numbers[], int temp[], int left, int right) {
  int mid;
  if (right > left) {
    mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);
    merge(numbers, temp, left, mid+1, right);
  }
}

void mergeSort(int numbers[], int temp[], int array_size) {
  m_sort(numbers, temp, 0, array_size - 1);
}
 
int main(int argc, char* argv[]) {
  mergeSort(randArr, temp, ASIZE);
 
  if(argc>=2) {
    printf("random_elem:%d, %s\n", randArr[1+argc],argv[0]);
  }
}

// bottom_up_mergesort.c

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Hàm helper để tìm giá trị nhỏ hơn
int min(int a, int b) { return (a < b) ? a : b; }

/**
 * @brief Trộn hai mảng con đã được sắp xếp (Giữ nguyên như các ví dụ trước).
 */
void merge(int arr[], int l, int m, int r) {
  int n1 = m - l + 1;
  int n2 = r - m;

  int *L = (int *)malloc(n1 * sizeof(int));
  int *R = (int *)malloc(n2 * sizeof(int));

  for (int i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  int i = 0, j = 0, k = l;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k++] = L[i++];
    } else {
      arr[k++] = R[j++];
    }
  }

  while (i < n1)
    arr[k++] = L[i++];
  while (j < n2)
    arr[k++] = R[j++];

  free(L);
  free(R);
}

/**
 * @brief Thuật toán Merge Sort song song từ dưới lên (lặp, không đệ quy).
 * @param arr Mảng cần sắp xếp.
 * @param n Kích thước của mảng.
 */
void bottom_up_mergeSort(int arr[], int n) {
  // Vòng lặp ngoài: Tăng dần kích thước của các đoạn con được trộn (1, 2,
  // 4, 8...)
  for (int current_size = 1; current_size < n;
       current_size = 2 * current_size) {

// Vòng lặp trong: Thực hiện các thao tác trộn cho kích thước hiện tại.
// Các thao tác trộn trong vòng lặp này là độc lập với nhau -> có thể song song
// hóa!
#pragma omp parallel for
    for (int left_start = 0; left_start < n - 1;
         left_start += 2 * current_size) {
      int mid = min(left_start + current_size - 1, n - 1);
      int right_end = min(left_start + 2 * current_size - 1, n - 1);

      // Chỉ gọi merge nếu có một đoạn bên phải để trộn
      if (mid < right_end) {
        merge(arr, left_start, mid, right_end);
      }
    }
  }
}

int main() {
  const int ARRAY_SIZE = 20000000; // 20 triệu phần tử
  int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));

  if (arr == NULL) {
    printf("Không thể cấp phát bộ nhớ!\n");
    return 1;
  }

  srand(time(0));
  printf("Đang tạo %d phần tử dữ liệu ngẫu nhiên...\n", ARRAY_SIZE);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = rand() % 100000;
  }

  printf("Bắt đầu sắp xếp song song (Bottom-up)...\n");

  double start_time = omp_get_wtime();

  // Gọi hàm sắp xếp từ dưới lên
  bottom_up_mergeSort(arr, ARRAY_SIZE);

  double end_time = omp_get_wtime();

  printf("------------------------------------------\n");
  printf("Hoàn thành! Thời gian sắp xếp: %f giây.\n", end_time - start_time);
  printf("------------------------------------------\n");

  free(arr);
  return 0;
}

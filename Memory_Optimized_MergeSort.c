// memory_optimized_mergesort.c

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Cần cho memcpy
#include <time.h>

#define THRESHOLD 1024 // Vẫn sử dụng kỹ thuật lai để có hiệu năng tốt nhất

/**
 * @brief Hàm trộn dữ liệu từ mảng nguồn (source) sang mảng đích (dest).
 */
void merge_optimized(int *source, int *dest, int l, int m, int r) {
  int i = l;     // Con trỏ cho nửa trái
  int j = m + 1; // Con trỏ cho nửa phải
  int k = l;     // Con trỏ cho mảng đích

  while (i <= m && j <= r) {
    if (source[i] <= source[j]) {
      dest[k++] = source[i++];
    } else {
      dest[k++] = source[j++];
    }
  }
  while (i <= m)
    dest[k++] = source[i++];
  while (j <= r)
    dest[k++] = source[j++];
}

void insertionSort(int arr[], int left, int right) {
  for (int i = left + 1; i <= right; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= left && arr[j] > key) {
      arr[j + 1] = arr[j--];
    }
    arr[j + 1] = key;
  }
}

/**
 * @brief Hàm sắp xếp đệ quy, hoán đổi vai trò của nguồn và đích.
 * @param source Mảng chứa dữ liệu chưa sắp xếp ở cấp này.
 * @param dest Mảng để ghi dữ liệu đã sắp xếp vào.
 * @return Trả về true nếu kết quả cuối cùng nằm trong mảng `dest`, false nếu
 * nằm trong `source`.
 */
void optimized_sort_recursive(int *source, int *dest, int l, int r) {
  if (r - l + 1 <= THRESHOLD) {
    // Với mảng nhỏ, sắp xếp trực tiếp trên mảng nguồn
    insertionSort(source, l, r);
    return;
  }

  int m = l + (r - l) / 2;

// Các tác vụ con sẽ sắp xếp trên cùng một mảng nguồn
#pragma omp task
  optimized_sort_recursive(source, dest, l, m);
#pragma omp task
  optimized_sort_recursive(source, dest, m + 1, r);

#pragma omp taskwait

  // Trộn hai nửa đã được sắp xếp từ `source` vào `dest`
  merge_optimized(source, dest, l, m, r);

// Sau khi trộn, dữ liệu đã sắp xếp nằm trong `dest`.
// Sao chép nó trở lại `source` để cấp đệ quy cao hơn sử dụng.
// Điều này đơn giản hóa logic so với việc theo dõi độ sâu.
#pragma omp parallel for
  for (int i = l; i <= r; i++) {
    source[i] = dest[i];
  }
}

int main() {
  const int ARRAY_SIZE = 20000000;
  int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
  int *buffer = (int *)malloc(ARRAY_SIZE *
                              sizeof(int)); // Cấp phát buffer một lần duy nhất

  if (arr == NULL || buffer == NULL) {
    printf("Không thể cấp phát bộ nhớ!\n");
    return 1;
  }

  srand(time(0));
  printf("Đang tạo %d phần tử dữ liệu ngẫu nhiên...\n", ARRAY_SIZE);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = rand() % 100000;
  }

  printf("Bắt đầu sắp xếp với tối ưu hóa bộ nhớ...\n");

  double start_time = omp_get_wtime();

#pragma omp parallel
  {
#pragma omp single
    {
      // Bắt đầu sắp xếp, arr là nguồn, buffer là đích
      optimized_sort_recursive(arr, buffer, 0, ARRAY_SIZE - 1);
    }
  }

  double end_time = omp_get_wtime();

  printf("------------------------------------------\n");
  printf("Hoàn thành! Thời gian sắp xếp: %f giây.\n", end_time - start_time);
  printf("------------------------------------------\n");

  free(arr);
  free(buffer);
  return 0;
}

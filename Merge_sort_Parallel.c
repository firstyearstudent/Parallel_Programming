// parallel_mergesort.c

#include <omp.h>   // Thư viện OpenMP cho lập trình song song
#include <stdio.h> // Dùng cho việc in ra màn hình (printf)
#include <stdlib.h> // Dùng cho việc cấp phát bộ nhớ (malloc), số ngẫu nhiên (rand, srand)
#include <time.h> // Dùng để lấy "hạt giống" cho số ngẫu nhiên (time)

/**
 * @brief Trộn hai mảng con đã được sắp xếp.
 * * @param arr Mảng chính.
 * @param l Chỉ số bắt đầu của mảng con thứ nhất.
 * @param m Chỉ số kết thúc của mảng con thứ nhất.
 * @param r Chỉ số kết thúc của mảng con thứ hai.
 */
void merge(int arr[], int l, int m, int r) {
  int n1 = m - l + 1;
  int n2 = r - m;

  // Tạo các mảng tạm để chứa dữ liệu
  int *L = (int *)malloc(n1 * sizeof(int));
  int *R = (int *)malloc(n2 * sizeof(int));

  // Sao chép dữ liệu vào các mảng tạm L[] và R[]
  for (int i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  // Trộn các mảng tạm trở lại vào mảng chính arr[l..r]
  int i = 0; // Chỉ số ban đầu của mảng con thứ nhất
  int j = 0; // Chỉ số ban đầu của mảng con thứ hai
  int k = l; // Chỉ số ban đầu của mảng con đã trộn
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k++] = L[i++];
    } else {
      arr[k++] = R[j++];
    }
  }

  // Sao chép các phần tử còn lại của L[], nếu có
  while (i < n1) {
    arr[k++] = L[i++];
  }

  // Sao chép các phần tử còn lại của R[], nếu có
  while (j < n2) {
    arr[k++] = R[j++];
  }

  // Giải phóng bộ nhớ đã cấp phát
  free(L);
  free(R);
}

/**
 * @brief Thuật toán Merge Sort song song sử dụng OpenMP Tasks.
 * * @param arr Mảng cần sắp xếp.
 * @param l Chỉ số bắt đầu.
 * @param r Chỉ số kết thúc.
 */
void parallel_mergeSort(int arr[], int l, int r) {
  // Điều kiện dừng đệ quy
  if (l < r) {
    int m = l + (r - l) / 2;

// Tạo một tác vụ (task) để sắp xếp nửa bên trái
#pragma omp task
    {
      parallel_mergeSort(arr, l, m);
    }

// Tạo một tác vụ khác để sắp xếp nửa bên phải
#pragma omp task
    {
      parallel_mergeSort(arr, m + 1, r);
    }

// Đợi cả hai tác vụ trên hoàn thành trước khi trộn
#pragma omp taskwait

    // Trộn hai nửa đã sắp xếp
    merge(arr, l, m, r);
  }
}

int main() {
  // Kích thước mảng, đủ lớn để quan sát được quá trình chạy song song
  const int ARRAY_SIZE = 20000000; // 20 triệu phần tử

  // Cấp phát động bộ nhớ cho mảng
  int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
  if (arr == NULL) {
    printf("Không thể cấp phát bộ nhớ!\n");
    return 1;
  }

  // Khởi tạo "hạt giống" cho bộ sinh số ngẫu nhiên
  srand(time(0));
  printf("Đang tạo %d phần tử dữ liệu ngẫu nhiên...\n", ARRAY_SIZE);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = rand() % 100000; // Các số ngẫu nhiên từ 0 đến 99999
  }

  printf("Bắt đầu sắp xếp song song...\n");

  // Bắt đầu đếm thời gian
  double start_time = omp_get_wtime();

// Bắt đầu vùng song song của OpenMP
// Chỉ một luồng (single) sẽ bắt đầu gọi hàm sắp xếp,
// hàm này sẽ tự tạo các tác vụ con cho các luồng khác xử lý.
#pragma omp parallel
  {
#pragma omp single
    parallel_mergeSort(arr, 0, ARRAY_SIZE - 1);
  }

  // Kết thúc đếm thời gian
  double end_time = omp_get_wtime();

  printf("------------------------------------------\n");
  printf("Hoàn thành! Thời gian sắp xếp: %f giây.\n", end_time - start_time);
  printf("------------------------------------------\n");

  // Giải phóng bộ nhớ đã cấp phát cho mảng
  free(arr);

  return 0;
}

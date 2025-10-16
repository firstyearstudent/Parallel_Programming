// parallel_merge.c

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THRESHOLD 1024 // Ngưỡng để chuyển sang sắp xếp/trộn tuần tự

// --- Các hàm tuần tự (dùng cho trường hợp cơ sở) ---

void sequential_merge(int *source, int *dest, int l, int m, int r) {
  int i = l, j = m + 1, k = l;
  while (i <= m && j <= r) {
    if (source[i] <= source[j])
      dest[k++] = source[i++];
    else
      dest[k++] = source[j++];
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

// --- Các hàm song song (phần chính của thuật toán) ---

// Hàm tìm kiếm nhị phân: Tìm vị trí của phần tử đầu tiên > key
int binary_search(int key, int *arr, int low, int high) {
  int l = low, r = high + 1;
  while (l < r) {
    int mid = l + (r - l) / 2;
    if (arr[mid] <= key)
      l = mid + 1;
    else
      r = mid;
  }
  return l;
}

// Hàm trộn song song (phần phức tạp nhất)
void parallel_merge(int *source, int *dest, int l1, int r1, int l2, int r2) {
  int n1 = r1 - l1 + 1;
  int n2 = r2 - l2 + 1;

  // Nếu một trong hai mảng con quá nhỏ, dùng hàm trộn tuần tự
  if (n1 + n2 <= THRESHOLD) {
    sequential_merge(source, dest, l1, r1, r2); // Chú ý: r1 là điểm giữa
    return;
  }

  // Đảm bảo mảng 1 luôn lớn hơn hoặc bằng mảng 2
  if (n1 < n2) {
    parallel_merge(source, dest, l2, r2, l1, r1);
    return;
  }

  // 1. Lấy phần tử giữa của mảng lớn hơn (mảng 1)
  int m1 = l1 + (n1) / 2;
  int pivot = source[m1];

  // 2. Dùng binary search để tìm điểm chia trong mảng 2
  int m2 = binary_search(pivot, source, l2, r2);

  // 3. Tính vị trí đích cho pivot
  int dest_mid = l1 + (m1 - l1) + (m2 - l2);
  dest[dest_mid] = pivot;

// 4. Tạo 2 tác vụ con đệ quy cho 2 bài toán trộn nhỏ hơn
#pragma omp task
  {
    // Trộn các phần nhỏ hơn pivot
    parallel_merge(source, dest, l1, m1 - 1, l2, m2 - 1);
  }
#pragma omp task
  {
    // Trộn các phần lớn hơn pivot
    parallel_merge(source, dest, m1 + 1, r1, m2, r2);
  }
#pragma omp taskwait
}

// Hàm sắp xếp chính, quản lý việc đệ quy và hoán đổi buffer
void parallel_sort_recursive(int *source, int *buffer, int l, int r) {
  if (r - l + 1 <= THRESHOLD) {
    insertionSort(source, l, r);
    return;
  }

  int m = l + (r - l) / 2;

#pragma omp task
  parallel_sort_recursive(source, buffer, l, m);
#pragma omp task
  parallel_sort_recursive(source, buffer, m + 1, r);

#pragma omp taskwait

  // Gọi hàm trộn song song. Kết quả từ `source` sẽ được ghi vào `buffer`.
  parallel_merge(source, buffer, l, m, m + 1, r);

// Sau khi trộn, dữ liệu đã sắp xếp nằm trong buffer.
// Sao chép nó trở lại mảng nguồn để cấp đệ quy cao hơn có thể sử dụng.
// Đây là một bước quan trọng để đảm bảo tính đúng đắn.
#pragma omp parallel for
  for (int i = l; i <= r; ++i) {
    source[i] = buffer[i];
  }
}

int main() {
  const int ARRAY_SIZE = 20000000;
  int *arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
  int *buffer = (int *)malloc(ARRAY_SIZE * sizeof(int)); // Mảng phụ

  if (arr == NULL || buffer == NULL) {
    printf("Không thể cấp phát bộ nhớ!\n");
    return 1;
  }

  srand(time(0));
  printf("Đang tạo %d phần tử dữ liệu ngẫu nhiên...\n", ARRAY_SIZE);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = rand() % 100000;
  }

  printf("Bắt đầu sắp xếp với Parallel Merge...\n");

  double start_time = omp_get_wtime();

#pragma omp parallel
  {
#pragma omp single
    {
      parallel_sort_recursive(arr, buffer, 0, ARRAY_SIZE - 1);
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

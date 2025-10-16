
# Parallel_Programming — Parallel MergeSort Implementations

## Các file chính
- `Merge_sort_Parallel.c` — Top-down (Đệ quy)
  - Ý tưởng: Song song hóa trực tiếp tư duy "chia để trị" bằng cách tạo #pragma omp task cho nửa trái và nửa phải.
  - Hoạt động: Mỗi lần chia mảng sẽ khởi tạo 2 task đệ quy. Sau khi cả hai kết thúc (`#pragma omp taskwait`), luồng chính thực hiện merge tuần tự.
  - Mục đích: Minh họa song song hóa đệ quy cơ bản.

- `Bottom_up_MergeSort.c` — Bottom-up (Lặp)
  - Ý tưởng: Loại bỏ đệ quy, dùng vòng lặp với kích thước đoạn con tăng dần (1,2,4,...).
  - Hoạt động: Trong mỗi lượt, các merge giữa cặp đoạn con độc lập → dùng `#pragma omp parallel for` để song song hoá vòng lặp merge.
  - Mục đích: Thể hiện cách cấu trúc lại thuật toán để phù hợp hơn với song song hoá vòng lặp.

- `Memory_optimized_MergeSort.c` — Tối ưu hóa bộ nhớ
  - Ý tưởng: Tránh malloc nhiều lần và copy dư thừa bằng cách cấp phát một buffer tạm toàn cục và hoán đổi vai trò nguồn ↔ đích ở mỗi cấp độ đệ quy.
  - Hoạt động: Buffer có kích thước bằng mảng gốc được cấp một lần, loại bỏ malloc trong vòng lặp chính, cải thiện locality/cache.
  - Mục đích: Giảm độ trễ bộ nhớ, tăng throughput.

- `Parallel_Merge.c` — Song song hóa thao tác Merge
  - Ý tưởng: Parallelize toàn bộ thao tác merge bằng cách viết merge theo kiểu đệ quy song song.
  - Hoạt động: Dùng binary search để chia bài toán merge hai mảng lớn thành hai bài toán merge nhỏ hơn, sau đó dùng `#pragma omp task` cho mỗi nửa.
  - Mục đích: Loại bỏ nút thắt cuối cùng — chính hàm merge chạy tuần tự — đạt tối ưu song song cao nhất.

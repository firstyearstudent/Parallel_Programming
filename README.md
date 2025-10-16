
# Parallel_Programming — Parallel MergeSort Implementations

Kho lưu trữ này chứa nhiều biến thể của thuật toán Merge Sort được triển khai bằng C và OpenMP, nhằm minh họa các kĩ thuật song song hóa khác nhau — từ cách song song hóa đệ quy cơ bản đến tối ưu hoá bộ nhớ và song song hóa chính hàm merge.

## Mục tiêu
- So sánh cách tiếp cận song song khác nhau để cài đặt Merge Sort.
- Minh họa trade-off giữa chi phí tạo tác vụ và lợi ích song song.
- Trình bày một số kỹ thuật tối ưu hoá (ngưỡng chuyển sang sắp xếp tuần tự, buffer dùng chung, merge song song).

## Các file chính
- `parallel_mergesort.c` — Top-down (Đệ quy)
  - Ý tưởng: Song song hóa trực tiếp tư duy "chia để trị" bằng cách tạo #pragma omp task cho nửa trái và nửa phải.
  - Hoạt động: Mỗi lần chia mảng sẽ khởi tạo 2 task đệ quy. Sau khi cả hai kết thúc (`#pragma omp taskwait`), luồng chính thực hiện merge tuần tự.
  - Mục đích: Minh họa song song hóa đệ quy cơ bản.

- `bottom_up_mergesort.c` — Bottom-up (Lặp)
  - Ý tưởng: Loại bỏ đệ quy, dùng vòng lặp với kích thước đoạn con tăng dần (1,2,4,...).
  - Hoạt động: Trong mỗi lượt, các merge giữa cặp đoạn con độc lập → dùng `#pragma omp parallel for` để song song hoá vòng lặp merge.
  - Mục đích: Thể hiện cách cấu trúc lại thuật toán để phù hợp hơn với song song hoá vòng lặp.

- `memory_optimized_mergesort.c` — Tối ưu hóa bộ nhớ
  - Ý tưởng: Tránh malloc nhiều lần và copy dư thừa bằng cách cấp phát một buffer tạm toàn cục và hoán đổi vai trò nguồn ↔ đích ở mỗi cấp độ đệ quy.
  - Hoạt động: Buffer có kích thước bằng mảng gốc được cấp một lần, loại bỏ malloc trong vòng lặp chính, cải thiện locality/cache.
  - Mục đích: Giảm độ trễ bộ nhớ, tăng throughput.

- `parallel_merge.c` — Song song hóa thao tác Merge
  - Ý tưởng: Parallelize toàn bộ thao tác merge bằng cách viết merge theo kiểu đệ quy song song.
  - Hoạt động: Dùng binary search để chia bài toán merge hai mảng lớn thành hai bài toán merge nhỏ hơn, sau đó dùng `#pragma omp task` cho mỗi nửa.
  - Mục đích: Loại bỏ nút thắt cuối cùng — chính hàm merge chạy tuần tự — đạt tối ưu song song cao nhất.

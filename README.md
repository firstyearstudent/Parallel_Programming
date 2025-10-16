# Parallel_Programming
```markdown
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

- `hybrid_mergesort.c` — Phương pháp lai (Hybrid)
  - Ý tưởng: Dùng ngưỡng (THRESHOLD) để với các mảng con nhỏ chuyển sang Insertion Sort tuần tự.
  - Hoạt động: Tiếp cận từ Top-down nhưng nếu độ dài mảng <= THRESHOLD thì chạy Insertion Sort tuần tự thay vì tạo task.
  - Mục đích: Giảm chi phí quản lý task khi mảng con quá nhỏ — một tối ưu thực tế hay dùng trong thư viện hiệu năng.

- `memory_optimized_mergesort.c` — Tối ưu hóa bộ nhớ
  - Ý tưởng: Tránh malloc nhiều lần và copy dư thừa bằng cách cấp phát một buffer tạm toàn cục và hoán đổi vai trò nguồn ↔ đích ở mỗi cấp độ đệ quy.
  - Hoạt động: Buffer có kích thước bằng mảng gốc được cấp một lần, loại bỏ malloc trong vòng lặp chính, cải thiện locality/cache.
  - Mục đích: Giảm độ trễ bộ nhớ, tăng throughput.

- `parallel_merge.c` — Song song hóa thao tác Merge
  - Ý tưởng: Parallelize toàn bộ thao tác merge bằng cách viết merge theo kiểu đệ quy song song.
  - Hoạt động: Dùng binary search để chia bài toán merge hai mảng lớn thành hai bài toán merge nhỏ hơn, sau đó dùng `#pragma omp task` cho mỗi nửa.
  - Mục đích: Loại bỏ nút thắt cuối cùng — chính hàm merge chạy tuần tự — đạt tối ưu song song cao nhất.

## Biên dịch (các ví dụ)
Yêu cầu: Trình biên dịch hỗ trợ OpenMP (ví dụ gcc hoặc clang).

Ví dụ các lệnh biên dịch:
- Top-down:
  - gcc -O3 -fopenmp -march=native -o parallel_mergesort parallel_mergesort.c
- Bottom-up:
  - gcc -O3 -fopenmp -march=native -o bottom_up_mergesort bottom_up_mergesort.c
- Hybrid:
  - gcc -O3 -fopenmp -march=native -o hybrid_mergesort hybrid_mergesort.c
- Memory-optimized:
  - gcc -O3 -fopenmp -march=native -o memory_optimized_mergesort memory_optimized_mergesort.c
- Parallel merge:
  - gcc -O3 -fopenmp -march=native -o parallel_merge parallel_merge.c

Chú ý:
- Tùy file nguồn có thể cần macro hoặc define (ví dụ THRESHOLD) — kiểm tra phần đỉnh file để biết các tùy chọn có sẵn.
- Cờ -O3 và -march=native thường giúp đo hiệu năng tốt hơn khi benchmark.

## Chạy chương trình
- Thiết lập số luồng:
  - export OMP_NUM_THREADS=4
- Ví dụ:
  - ./parallel_mergesort 1000000
  - (tùy chương trình có thể nhận thêm đối số như seed, kích thước mảng, hoặc in thời gian)

Nếu chương trình của bạn có giao diện khác (ví dụ nhận các tham số cụ thể), hãy kiểm tra phần comment đầu file nguồn.

## Gợi ý benchmark và kiểm tra
- Dùng arrays có kích thước lớn (1e6, 5e6, ...) để thấy sự khác biệt hiệu năng.
- Chạy với nhiều giá trị OMP_NUM_THREADS (1,2,4,8,...) để thấy scalability.
- So sánh thời gian với thuật toán tuần tự (std mergesort hoặc qsort) để đánh giá speedup.
- Khi benchmark, tắt các tác động ngoại cảnh (caching OS, CPU frequency scaling) nếu có thể để kết quả ổn định.

## Lưu ý triển khai & tối ưu
- Chi phí tạo task không miễn phí: hybrid với ngưỡng thường là lựa chọn tốt.
- Tối ưu hóa bộ nhớ (single buffer, hoán đổi nguồn/đích) giúp giảm cache misses và malloc overhead.
- Parallel merge phức tạp nhưng cần thiết nếu merge tuần tự là nút thắt.

## Đóng góp
- Bạn có thể thêm:
  - Makefile / script build tự động
  - Script benchmark (bash, Python)
  - Unit tests (kiểm tra sắp xếp đúng) và test harness
  - Phiên bản sử dụng pthreads hoặc MPI để so sánh

## License
- Mặc định: MIT (hoặc thay đổi theo ý bạn). Vui lòng thêm file LICENSE nếu muốn.

## Thông tin liên hệ
- Chủ repo: firstyearstudent
```

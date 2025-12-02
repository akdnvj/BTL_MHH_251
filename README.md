# Symbolic and Algebraic Reasoning in Petri Nets (CO2011) ✨

![Version](https://img.shields.io/badge/Version-0.0.2-blue)
![Status](https://img.shields.io/badge/Status-In%20Progress-yellow) 
![Language](https://img.shields.io/badge/Language-Python%20%7C%20C%2B%2B%20%7C%20Java-red)

## 📜 Giới Thiệu (Overview)

[cite_start]Dự án này là bài tập lớn môn Mô hình Toán học (CO2011), tập trung vào việc phân tích mô hình mạng Petri (Petri Nets) bằng các kỹ thuật **Biểu tượng (Symbolic)** và **Đại số (Algebraic)** để khắc phục vấn đề bùng nổ không gian trạng thái[cite: 16]. [cite_start]Chúng tôi tích hợp việc sử dụng **Binary Decision Diagrams (BDD)** để biểu diễn tập trạng thái khả thi (reachable markings) [cite: 25] [cite_start]và **Integer Linear Programming (ILP)** để kiểm tra các thuộc tính formal như deadlock và tối ưu hóa[cite: 26, 40].

* **Mục đích:**
    * [cite_start]Nắm vững nền tảng toán học của Petri Nets[cite: 31].
    * [cite_start]Thực hành các kỹ thuật mô hình hóa tính toán (BDD, ILP) được sử dụng rộng rãi trong Verification và AI[cite: 31].
* [cite_start]**Mô hình:** Dự án giả định các mô hình kiểm thử là **1-safe Petri nets**[cite: 69].

## 🚀 Các Nhiệm Vụ Đã Thực Hiện

Dự án này bao gồm việc triển khai các chức năng phân tích mạng Petri sau:

1.  [cite_start]**Đọc/Phân tích PNML:** Triển khai trình phân tích để đọc 1-safe Petri net từ tệp **PNML** tiêu chuẩn[cite: 34].
2.  [cite_start]**Tính toán Reachability Tường minh:** Triển khai thuật toán **BFS/DFS** cơ bản để liệt kê tường minh tất cả các trạng thái khả thi[cite: 36].
3.  [cite_start]**Tính toán Reachability bằng BDD (40%):** Mã hóa trạng thái bằng **BDD** và xây dựng tập trạng thái khả thi một cách biểu tượng[cite: 37, 38].
4.  [cite_start]**Phát hiện Deadlock bằng ILP + BDD (20%):** Kết hợp công thức **ILP** và **BDD** để tìm kiếm (và xuất) một trạng thái deadlock khả thi (dead marking that is reachable)[cite: 40, 41, 42].
5.  [cite_start]**Tối ưu hóa trên Reachable Markings (20%):** Tìm trạng thái khả thi tối ưu hóa hàm mục tiêu tuyến tính **$\max c^{\top}M$**[cite: 44, 45].

## 🛠️ Công Nghệ và Thư viện

Ngôn ngữ lập trình được sử dụng là [Ngôn ngữ bạn chọn: Python / C++ / Java].

* **Ngôn ngữ:** [Ghi rõ phiên bản, ví dụ: Python 3.10]
* **Thư viện BDD:** [Ví dụ: PyEDA / CUDD (cho C++)]
* **Thư viện ILP/Optimization:** [Ví dụ: Gurobi / PuLP / GLPK]
* **Công cụ khác:** Git, GitHub

## 💻 Hướng Dẫn Cài Đặt và Chạy Dự Án

### 1. Yêu Cầu Tiên Quyết (Prerequisites)

* [Ví dụ: Đã cài đặt Python 3.10+]
* [Ví dụ: Đã cài đặt thư viện BDD và ILP (Gurobi/PuLP) theo hướng dẫn.]

### 2. Các Bước Thực Hiện

1.  **Clone Repository:**
    ```bash
    git clone [https://github.com/your-username/ten-du-an.git](https://github.com/your-username/ten-du-an.git)
    ```
2.  **Di chuyển vào thư mục dự án:**
    ```bash
    cd ten-du-an
    ```
3.  **Cài đặt các thư viện (nếu dùng Python):**
    ```bash
    pip install -r requirements.txt
    ```
4.  **Chạy Chương trình:**
    * **Để chạy toàn bộ các tác vụ trên mô hình kiểm thử:**
        ```bash
        [Lệnh chạy chính, ví dụ: python main.py <PNML_file>]
        ```
    * [cite_start]*(Tham khảo file `README_Instructions` chi tiết đi kèm trong thư mục Source Code để biết thêm chi tiết về cú pháp lệnh chạy)*[cite: 75].

## 📄 Nội dung File Báo Cáo (`Report.pdf`)

[cite_start]File báo cáo (`$\le15$ pages` [cite: 76]) bao gồm các phần sau:

* [cite_start]**Nền tảng lý thuyết:** Mô tả lý thuyết cho từng phương pháp (tường minh, biểu tượng, ILP-based)[cite: 48].
* [cite_start]**Thiết kế và Cấu trúc dữ liệu:** Mô tả thiết kế triển khai và các cấu trúc dữ liệu được sử dụng[cite: 54].
* [cite_start]**Kết quả và Thảo luận hiệu suất:** Trình bày kết quả thực nghiệm và thảo luận về hiệu suất[cite: 55].
* [cite_start]**Thách thức và Cải tiến:** Đề cập đến những thách thức gặp phải và các cải tiến khả thi[cite: 56].
* [cite_start]*Lưu ý: Khuyến khích sử dụng Hình ảnh và Bảng biểu để minh họa kết quả.* [cite: 57]

## 🧑‍💻 Thông Tin Nhóm và Tác Giả

[cite_start]Bài tập này được thực hiện bởi nhóm [3-5 sinh viên][cite: 59].

| Tên Sinh Viên | Mã Số Sinh Viên | Vai trò |
| :--- | :--- | :--- |
| [Tên Thành viên 1] | [MSSV] | [Trưởng nhóm / Thành viên] |
| [Tên Thành viên 2] | [MSSV] | [Thành viên] |
| [Tên Thành viên 3] | [MSSV] | [Thành viên] |
| ... | ... | ... |

**Trưởng nhóm (Người nộp bài):** [Tên Trưởng nhóm]
**Email liên hệ:** [email_cua_truong_nhom@example.com]

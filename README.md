# Symbolic and Algebraic Reasoning in Petri Nets (CO2011) ✨

## 📜 Giới Thiệu (Overview)

Dự án này là bài tập lớn môn Mô hình Toán học (CO2011), tập trung vào việc phân tích mô hình mạng Petri (Petri Nets) bằng các kỹ thuật **Biểu tượng (Symbolic)** và **Đại số (Algebraic)** để khắc phục vấn đề bùng nổ không gian trạng thái.

* **Mục đích:**
    * Nắm vững nền tảng toán học của Petri Nets.
    * Thực hành các kỹ thuật mô hình hóa tính toán (BDD, ILP) được sử dụng rộng rãi trong Verification và AI.
* **Mô hình:** Dự án giả định các mô hình kiểm thử là **1-safe Petri nets**.

## 🚀 Các Nhiệm Vụ Đã Thực Hiện

Dự án này bao gồm việc triển khai các chức năng phân tích mạng Petri sau:

1.  **Đọc/Phân tích PNML:** Triển khai trình phân tích để đọc 1-safe Petri net từ tệp **PNML** tiêu chuẩn.
2.  **Tính toán Reachability Tường minh:** Triển khai thuật toán **BFS/DFS** cơ bản để liệt kê tường minh tất cả các trạng thái khả thi.
3.  **Tính toán Reachability bằng BDD:** Mã hóa trạng thái bằng **BDD** và xây dựng tập trạng thái khả thi một cách biểu tượng.
4.  **Phát hiện Deadlock bằng ILP + BDD:** Kết hợp công thức **ILP** và **BDD** để tìm kiếm (và xuất) một trạng thái deadlock khả thi (dead marking that is reachable).
5.  **Tối ưu hóa trên Reachable Markings:** Tìm trạng thái khả thi tối ưu hóa hàm mục tiêu tuyến tính **$\max c^{\top}M$**.

## 🛠️ Công Nghệ và Thư viện

Ngôn ngữ lập trình được sử dụng là C++.

* **Thư viện BDD:** CUDD
* **Thư viện ILP:** Gurobi/OR-Tools
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

## 🧑‍💻 Thông Tin Nhóm và Tác Giả

Bài tập này được thực hiện bởi nhóm 5 sinh viên lớp L02.

| Tên Sinh Viên | Mã Số Sinh Viên | 
| :--- | :--- | 
| Nguyễn Hoàng Anh | 2310099 | 
| Trịnh Thanh Bình] | 2310313 | 
| Trần Nhật Anh  | 2310150 | 
| Trang Hiểu Nghĩa | 2312279 | 
| Đoàn Đức Huy | 2210501 | 
| :... | :... | 

**Email liên hệ:** anh.nguyennghe@hcmut.edu.vn

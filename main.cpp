// main.cpp
#include <iostream>
#include <vector>
#include <iomanip> // Cần để định dạng output
#include "BDDManager.h"

int main() {
    // 1. Khởi tạo Manager (tự động load test.pnml, xây dựng BDD và tính Explicit)
    // Quá trình này đã đo thời gian cho cả hai phương pháp.
    BDDManager manager("test.pnml");

    // 2. In ra báo cáo hiệu suất và số trạng thái
    manager.printPerformanceReport();

    // In ra tất cả các trạng thái đến được 
    manager.printReachableMarkings();

    // 3. Thực hiện kiểm tra (ví dụ)
    std::cout << "\nKiểm tra một số trạng thái cụ thể:\n";
    
    // Lấy thứ tự các Place: (Giả sử là p1, p2, p3, p4, p5)
    
    // Trạng thái ban đầu (p1=1, còn lại 0)
    std::vector<int> m1 = {1, 0, 0, 0, 0};
    // Trạng thái sau t1 (p1 mất, p2=1, p3=1)
    std::vector<int> m2 = {0, 1, 1, 0, 0};
    // Trạng thái kết thúc (p4=1, p5=1)
    std::vector<int> m_end = {0, 0, 0, 1, 1};
    // Trạng thái vô lý
    std::vector<int> m3 = {1, 1, 1, 1, 1};

    std::cout << "Check Marking [1,0,0,0,0] (Initial): " << (manager.checkReachability(m1) ? "FOUND" : "NOT FOUND") << "\n";
    std::cout << "Check Marking [0,1,1,0,0] (Mid-state): " << (manager.checkReachability(m2) ? "FOUND" : "NOT FOUND") << "\n";
    std::cout << "Check Marking [0,0,0,1,1] (Final state): " << (manager.checkReachability(m_end) ? "FOUND" : "NOT FOUND") << "\n";
    std::cout << "Check Marking [1,1,1,1,1] (Impossible): " << (manager.checkReachability(m3) ? "FOUND" : "NOT FOUND") << "\n";


    //HoangAnh

    const std::vector<std::string>& places = manager.getPlaceOrder();
    size_t n = places.size();

    std::vector<int> c_weights(n);
    for (size_t i = 0; i < n; ++i) {
        std::cout << "Nhap trong so c[" << i << "]: ";
        std::cin >> c_weights[i]; 
    }
    
    // Gọi hàm tối ưu hóa
    manager.findOptimalMarking(c_weights);

    return 0;
}



// // main.cpp
// #include <iostream>
// #include <vector>
// #include "BDDManager.h"

// int main() {
//     // 1. Khởi tạo Manager (tự động load test.pnml và xây dựng BDD)
//     // Đảm bảo bạn đã có file test.pnml cùng thư mục
//     BDDManager manager("test.pnml");

//     // Lấy thứ tự các Place để biết vị trí index trong vector
//     std::cout << "Thu tu Place: ";
//     for (const auto& name : manager.getPlaceOrder()) {
//         std::cout << name << " ";
//     }
//     std::cout << "\n--------------------------------\n";

//     // 2. Định nghĩa các Marking cần kiểm tra
//     // Ví dụ dựa trên test.pnml của bạn: 5 place (p1, p2, p3, p4, p5)
    
//     // Test 1: Trạng thái ban đầu (p1=1, còn lại 0) -> Nên trả về TRUE
//     std::vector<int> m1 = {1, 0, 0, 0, 0};
    
//     // Test 2: Sau khi transition t1 (Split) bắn (p1 mất, p2 và p3 có token) -> Nên trả về TRUE
//     std::vector<int> m2 = {0, 1, 1, 0, 0};

//     // Test 3: Một trạng thái vô lý (tất cả đều có token) -> Nên trả về FALSE
//     std::vector<int> m3 = {1, 1, 1, 1, 1};

//     // 3. Thực hiện kiểm tra
//     std::cout << "Check Marking [1,0,0,0,0]: " << (manager.checkReachability(m1) ? "FOUND" : "NOT FOUND") << "\n";
//     std::cout << "Check Marking [0,1,1,0,0]: " << (manager.checkReachability(m2) ? "FOUND" : "NOT FOUND") << "\n";
//     std::cout << "Check Marking [1,1,1,1,1]: " << (manager.checkReachability(m3) ? "FOUND" : "NOT FOUND") << "\n";

//     return 0;
// }
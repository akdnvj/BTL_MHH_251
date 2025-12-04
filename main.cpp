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



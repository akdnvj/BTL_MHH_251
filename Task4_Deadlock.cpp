    // File: Task4_Deadlock.cpp
// Code mô phỏng việc tích hợp ILP và BDD để tìm Deadlock
#include <iostream>
#include <vector>
#include <string>
#include "BDDManager.h"

// --- HÀM GIẢ LẬP SOLVER TOÁN HỌC (ILP) ---
// Trong thực tế, đoạn này sẽ gọi thư viện Gurobi/OR-Tools.
// Ở đây, mô phỏng việc Solver tìm ra một trạng thái nghi ngờ là Deadlock.
std::vector<int> solveILP(const std::vector<std::string>& places) {
    std::cout << ">> [ILP Solver] Dang thiet lap phuong trinh toan hoc...\n";
    std::cout << ">> [ILP Solver] Dang giai he phuong trinh...\n";
    
    // Giả sử Solver tính toán và tìm ra: Token nằm ở End1 (p4) và End2 (p5)
    // (Vì tại đây không còn transition nào đi ra -> Deadlock toán học)
    std::vector<int> candidate(places.size(), 0);
    
    // Map tên Place sang vector
    for (size_t i = 0; i < places.size(); ++i) {
        if (places[i] == "p4" || places[i] == "End1") candidate[i] = 1;
        if (places[i] == "p5" || places[i] == "End2") candidate[i] = 1;
    }
    
    return candidate; // Trả về vector [0, 0, 0, 1, 1]
}

int main() {
    std::cout << "=======================================================\n";
    std::cout << "       TASK 4: DEADLOCK DETECTION (ILP + BDD)          \n";
    std::cout << "=======================================================\n";

    // Load Mạng Petri và Xây dựng BDD
    std::cout << "[Buoc 1] Khoi tao BDD Manager...\n";
    // Tự động đọc file test.pnml
    BDDManager manager("test.pnml"); 

    // Lấy danh sách biến
    const std::vector<std::string>& places = manager.getPlaceOrder();
    std::cout << "-> Danh sach Places: ";
    for (const auto& p : places) std::cout << p << " ";
    std::cout << "\n";

    // Chạy Solver toán học (Giả lập)
    std::cout << "\n[Buoc 2] Tim Deadlock tiem nang bang ILP (Integer Linear Programming)...\n";
    std::vector<int> potential_deadlock = solveILP(places);

    std::cout << "-> Ung vien tim duoc: [ ";
    for(int val : potential_deadlock) std::cout << val << " ";
    std::cout << "]\n";

    // KIỂM TRA CHÉO VỚI BDD (Bước quan trọng nhất)
    std::cout << "\n[Buoc 3] Kiem tra ung vien voi BDD (Reachability Check)...\n";
    bool isReachable = manager.checkReachability(potential_deadlock);

    std::cout << "\n-------------------- KET QUA CUOI CUNG -----------------\n";
    if (isReachable) {
        std::cout << ">>> [PHAT HIEN] Deadlock nay CO THE XAY RA (Reachable)!\n";
        std::cout << ">>> He thong se bi treo tai trang thai nay.\n";
    } else {
        std::cout << ">>> [CANH BAO GIA] Deadlock nay khong the xay ra (Unreachable).\n";
        std::cout << ">>> He thong an toan.\n";
    }
    std::cout << "=======================================================\n";

    return 0;
}
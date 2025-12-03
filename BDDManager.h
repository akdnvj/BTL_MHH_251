// File: BDDManager.h 
//g++ main.cpp BDDManager.cpp -o petri -std=c++17 -ltinyxml2 -lcudd
//./petri test.pnml
#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <functional>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <iomanip>

#include "tinyxml2.h"
#include <cudd.h>

using namespace tinyxml2;

// Định nghĩa alias để khớp với main.cpp cũ nhưng được quản lý trong class
using CuddMgr = DdManager*;
using Marking = std::vector<int>;

// Cấu trúc Node ô dùng (Wrapper cho DdNode của thư viện CUDD)
struct BDDNode {
    DdNode* node; // Con trỏ thực tế tới node CUDD
};

struct ReachabilityResults {//luu ket qua reachability
    double symbolic_time_ms = 0.0;
    size_t symbolic_states = 0;
    size_t symbolic_bdd_nodes = 0;
    double explicit_time_ms = 0.0;
    size_t explicit_states = 0;
};

// =========================================================
// Hashing cho Marking (cần cho Explicit Reachability)
// =========================================================
struct VectorHash {
    size_t operator()(const Marking& m) const {
        std::hash<int> hasher;
        size_t seed = m.size();
        for (int i : m) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class BDDManager {
private:
    // --- Các biến thành viên riêng tư ---
    CuddMgr mgr;                // Quản lý bộ nhớ CUDD
    BDDNode* root;              // Node gốc lưu tập trạng thái đến được (Reachable Set)
    
    // Lưu trữ ánh xạ biến để kiểm tra sau này
    std::vector<std::string> ordered_place_ids;
    std::vector<DdNode*> vars;  // Biến hiện tại (x)
    ReachabilityResults results; 
    
    // --- Hàm nội bộ ---
    // Hàm này chuyển đổi vector marking thành BDD cube (tích các biến)
    DdNode* markingToCube(const std::vector<int>& marking);

    // Hàm đệ quy nội bộ (dùng Cudd_Eval để kiểm tra một path cụ thể)
    bool internalCheck(BDDNode* node, const std::vector<int>& inputs);

    //in ra BDD
    ReachabilityResults calculateExplicitReachability(const std::string& pnml_file, const std::vector<std::string>& place_order);

    //HoangAnh
    int bestValue = INT_MIN;
    Marking bestM;
    std::vector<int> c_weights;
    size_t n_vars;

    // Hàm đệ quy DFS cho tối ưu hóa (Duyệt BDD)
    void dfs_optimization(DdNode* u, Marking& M);





public:
    // --- VÙNG GIAO TIẾP ---
    
    // Hàm khởi tạo: Tự động load file PNML mặc định hoặc file chỉ định, 
    // khởi tạo CUDD và tính toán tập Reachability ngay lập tức.
    BDDManager(const std::string& pnml_file = "test.pnml");

    // Destructor để giải phóng bộ nhớ CUDD
    ~BDDManager();

    // Hàm check nhận vào một marking cụ thể (vector int) và trả về True/False 
    // xem nó có nằm trong tập trạng thái đến được không.
    bool checkReachability(const std::vector<int>& marking);
    
    // Helper: Lấy danh sách tên các Place theo thứ tự (để người dùng biết thứ tự vector)
    const std::vector<std::string>& getPlaceOrder() const;

    //compare performance with the explicit approach (time and memory)
    void printPerformanceReport() const;

    // Print all reachable markings
    void printReachableMarkings() const;

    //HoangAnh
    void findOptimalMarking(const std::vector<int>& c_weights);
};





















// // File: BDDManager.h 
// #pragma once
// #include <map>
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <queue>
// #include <tuple>
// #include <algorithm>
// #include <sstream>
// #include <functional>
// #include <memory>
// #include <cstdlib>
// #include <cstring>

// #include "tinyxml2.h"
// #include <cudd.h>

// using namespace tinyxml2;

// // Định nghĩa alias để khớp với main.cpp cũ nhưng được quản lý trong class
// using CuddMgr = DdManager*;
// using Marking = std::vector<int>;

// // Cấu trúc Node ô dùng (Wrapper cho DdNode của thư viện CUDD)
// struct BDDNode {
//     DdNode* node; // Con trỏ thực tế tới node CUDD
// };

// class BDDManager {
// private:
//     // --- Các biến thành viên riêng tư ---
//     CuddMgr mgr;                // Quản lý bộ nhớ CUDD
//     BDDNode* root;              // Node gốc lưu tập trạng thái đến được (Reachable Set)
    
//     // Lưu trữ ánh xạ biến để kiểm tra sau này
//     std::vector<std::string> ordered_place_ids;
//     std::vector<DdNode*> vars;  // Biến hiện tại (x)
    
//     // --- Hàm nội bộ ---
//     // Hàm này chuyển đổi vector marking thành BDD cube (tích các biến)
//     DdNode* markingToCube(const std::vector<int>& marking);

//     // Hàm đệ quy nội bộ (dùng Cudd_Eval để kiểm tra một path cụ thể)
//     bool internalCheck(BDDNode* node, const std::vector<int>& inputs);

// public:
//     // --- VÙNG GIAO TIẾP ---
    
//     // Hàm khởi tạo: Tự động load file PNML mặc định hoặc file chỉ định, 
//     // khởi tạo CUDD và tính toán tập Reachability ngay lập tức.
//     BDDManager(const std::string& pnml_file = "test.pnml");

//     // Destructor để giải phóng bộ nhớ CUDD
//     ~BDDManager();

//     // Hàm check nhận vào một marking cụ thể (vector int) và trả về True/False 
//     // xem nó có nằm trong tập trạng thái đến được không.
//     bool checkReachability(const std::vector<int>& marking);
    
//     // Helper: Lấy danh sách tên các Place theo thứ tự (để người dùng biết thứ tự vector)
//     const std::vector<std::string>& getPlaceOrder() const;
// };
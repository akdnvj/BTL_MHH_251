// File: BDDManager.cpp
#include "BDDManager.h"
#include <cmath> // Dùng cho std::round

// =========================================================
// Helper Class: PetriNet Logic (được giữ lại)
// =========================================================

struct Place { std::string id; std::string name; };
struct Transition { std::string id; std::string name; };

class PetriNetHelper {
public:
    std::unordered_map<std::string, Place> places;
    std::unordered_map<std::string, Transition> transitions;
    std::unordered_map<std::string, std::unordered_set<std::string>> pre;
    std::unordered_map<std::string, std::unordered_set<std::string>> post;
    std::unordered_map<std::string, int> initial_marking;
    std::vector<std::string> ordered_place_ids;
    std::unordered_map<std::string, int> place_to_index;

    bool load_pnml(const std::string& filename) {
        // ... (Logic load_pnml không thay đổi) ...
        XMLDocument doc;
        XMLError loadResult = doc.LoadFile(filename.c_str());
        if (loadResult != XML_SUCCESS) {
            std::cerr << "Lỗi: không thể mở PNML: " << filename << "\n";
            return false;
        }
        XMLElement* root = doc.FirstChildElement("pnml");
        if (!root) root = doc.RootElement();
        XMLElement* net = root->FirstChildElement("net");
        XMLElement* page = net->FirstChildElement("page");

        for (XMLElement* child = page->FirstChildElement(); child; child = child->NextSiblingElement()) {
            std::string name = child->Name();
            if (name == "place") {
                const char* id = child->Attribute("id");
                std::string sid = id ? id : "";
                places[sid] = Place{sid, sid};
                int mark = 0;
                XMLElement* im = child->FirstChildElement("initialMarking");
                if (im) {
                    XMLElement* t = im->FirstChildElement("text");
                    if (t && t->GetText()) try { mark = std::stoi(t->GetText()); } catch(...) { mark = 1; }
                }
                initial_marking[sid] = mark;
                ordered_place_ids.push_back(sid);
            } else if (name == "transition") {
                const char* id = child->Attribute("id");
                std::string tid = id ? id : "";
                transitions[tid] = Transition{tid, tid};
            }
        }
        for (size_t i = 0; i < ordered_place_ids.size(); ++i) place_to_index[ordered_place_ids[i]] = (int)i;

        for (XMLElement* child = page->FirstChildElement("arc"); child; child = child->NextSiblingElement("arc")) {
            const char* src = child->Attribute("source");
            const char* tgt = child->Attribute("target");
            if (src && tgt) {
                std::string s = src, t = tgt;
                if (places.count(s) && transitions.count(t)) pre[t].insert(s);
                else if (transitions.count(s) && places.count(t)) post[s].insert(t);
            }
        }
        return true;
    }

    Marking get_initial_marking_vector() const {
        Marking m(ordered_place_ids.size());
        for (size_t i = 0; i < ordered_place_ids.size(); ++i)
            m[i] = initial_marking.at(ordered_place_ids[i]);
        return m;
    }

    DdNode* build_transition_relation(CuddMgr mgr, const std::vector<DdNode*>& vars, const std::vector<DdNode*>& primed_vars) {
        // ... (Logic build_transition_relation không thay đổi) ...
        int n_vars = (int)vars.size();
        DdNode* R = Cudd_Not(Cudd_ReadOne(mgr)); Cudd_Ref(R); 

        for (const auto& t_pair : transitions) {
            const std::string& t_id = t_pair.first;
            DdNode* T = Cudd_ReadOne(mgr); Cudd_Ref(T); 

            // Pre-conditions
            if (pre.count(t_id)) {
                for (const std::string& p : pre.at(t_id)) {
                    DdNode* tmp = Cudd_bddAnd(mgr, T, vars[place_to_index.at(p)]); Cudd_Ref(tmp);
                    Cudd_RecursiveDeref(mgr, T); T = tmp;
                }
            }
            // Conflict check (Post but not Pre)
            if (post.count(t_id)) {
                for (const std::string& p : post.at(t_id)) {
                    if (!pre.count(t_id) || pre.at(t_id).find(p) == pre.at(t_id).end()) {
                        DdNode* lit = Cudd_Not(vars[place_to_index.at(p)]);
                        DdNode* tmp = Cudd_bddAnd(mgr, T, lit); Cudd_Ref(tmp);
                        Cudd_RecursiveDeref(mgr, T); T = tmp;
                    }
                }
            }
            // Frame conditions (variables not changing)
            DdNode* Frame = Cudd_ReadOne(mgr); Cudd_Ref(Frame);
            for (int i = 0; i < n_vars; ++i) {
                const std::string& p = ordered_place_ids[i];
                bool is_pre = (pre.count(t_id) && pre.at(t_id).count(p));
                bool is_post = (post.count(t_id) && post.at(t_id).count(p));
                
                DdNode* change = nullptr;
                
                if (is_pre && !is_post) { // 1. Consume (Pre-only): p' = 0
                    change = Cudd_Not(primed_vars[i]); Cudd_Ref(change);
                } else if (!is_pre && is_post) { // 2. Produce (Post-only): p' = 1
                    change = primed_vars[i]; Cudd_Ref(change);
                } else { // 3 & 4. Self-Loop (is_pre && is_post) HOẶC Frame (neither): p' = p
                    // p <-> p' (p' = p V p' = !p)
                    DdNode* c1 = Cudd_bddAnd(mgr, vars[i], primed_vars[i]); Cudd_Ref(c1);
                    DdNode* c2 = Cudd_bddAnd(mgr, Cudd_Not(vars[i]), Cudd_Not(primed_vars[i])); Cudd_Ref(c2);
                    change = Cudd_bddOr(mgr, c1, c2); Cudd_Ref(change);
                    Cudd_RecursiveDeref(mgr, c1); Cudd_RecursiveDeref(mgr, c2); 
                    // Cần loại bỏ dòng thừa: Frame = Cudd_Not(Frame); 
                }
                
                DdNode* tmpF = Cudd_bddAnd(mgr, Frame, change); Cudd_Ref(tmpF);
                Cudd_RecursiveDeref(mgr, Frame); Cudd_RecursiveDeref(mgr, change); Frame = tmpF;
            }
            
            DdNode* tmpTF = Cudd_bddAnd(mgr, T, Frame); Cudd_Ref(tmpTF);
            Cudd_RecursiveDeref(mgr, T); Cudd_RecursiveDeref(mgr, Frame); T = tmpTF;

            DdNode* tmpR = Cudd_bddOr(mgr, R, T); Cudd_Ref(tmpR);
            Cudd_RecursiveDeref(mgr, R); Cudd_RecursiveDeref(mgr, T); R = tmpR;
        }
        return R;
    }
};

// =========================================================
// Implementation of BDDManager
// =========================================================

// NEW: Hàm tính toán Explicit Reachability
ReachabilityResults BDDManager::calculateExplicitReachability(
    const std::string& pnml_file, 
    const std::vector<std::string>& place_order) 
{
    PetriNetHelper pn;
    if (!pn.load_pnml(pnml_file)) return {0.0, 0, 0, 0.0, 0};

    auto start_time = std::chrono::high_resolution_clock::now();

    // BFS implementation
    Marking initial = pn.get_initial_marking_vector();
    std::unordered_set<Marking, VectorHash> visited;
    std::queue<Marking> q;

    q.push(initial);
    visited.insert(initial);

    while (!q.empty()) {
        Marking current = q.front();
        q.pop();

        for (const auto& t_pair : pn.transitions) {
            const std::string& t_id = t_pair.first;
            
            // Check enablement (0/1 assumption)
            bool enabled = true;
            if (pn.pre.count(t_id)) {
                for (const std::string& p : pn.pre.at(t_id)) {
                    if (current[pn.place_to_index.at(p)] != 1) {
                        enabled = false;
                        break;
                    }
                }
            }
            if (!enabled) continue;

            // Compute next marking (0/1 assumption)
            Marking next = current;
            for (size_t i = 0; i < place_order.size(); ++i) {
                const std::string& p = place_order[i];
                bool is_pre = (pn.pre.count(t_id) && pn.pre.at(t_id).count(p));
                bool is_post = (pn.post.count(t_id) && pn.post.at(t_id).count(p));

                if (is_pre && !is_post) { // Lose token (1 -> 0)
                    next[i] = 0; 
                } else if (!is_pre && is_post) { // Gain token (0 -> 1)
                    next[i] = 1;
                } 
                // is_pre && is_post: keeps 1 (no change for 0/1 net)
                // !is_pre && !is_post: keeps value
            }

            if (visited.find(next) == visited.end()) {
                visited.insert(next);
                q.push(next);
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    ReachabilityResults res;
    res.explicit_time_ms = duration_ms; 
    res.explicit_states = visited.size();

    return res;
}


BDDManager::BDDManager(const std::string& pnml_file) {
    // 1. Tải PNML và Tính Explicit Reachability để so sánh
    PetriNetHelper pn;
    if (!pn.load_pnml(pnml_file)) {
        std::cerr << "Lỗi: không thể load PNML. Dừng.\n";
        mgr = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        root = new BDDNode{ Cudd_Not(Cudd_ReadOne(mgr)) };
        Cudd_Ref(root->node);
        return;
    }
    
    this->ordered_place_ids = pn.ordered_place_ids;
    results = calculateExplicitReachability(pnml_file, this->ordered_place_ids);


    // 2. Khởi tạo CUDD và tính Symbolic Reachability
    auto symbolic_start_time = std::chrono::high_resolution_clock::now();
    mgr = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    root = new BDDNode{ nullptr };
    
    // ... (Symbolic Reachability Logic remains the same) ...
    int nvars = (int)ordered_place_ids.size();
    
    std::vector<DdNode*> primed_vars(nvars);
    for (int i = 0; i < 2 * nvars; ++i) Cudd_bddIthVar(mgr, i); 
    
    vars.resize(nvars);
    for (int i = 0; i < nvars; ++i) {
        vars[i] = Cudd_bddIthVar(mgr, i);
        primed_vars[i] = Cudd_bddIthVar(mgr, i + nvars);
    }

    Marking initM = pn.get_initial_marking_vector();
    DdNode* S = Cudd_ReadOne(mgr); Cudd_Ref(S);
    for (int i = 0; i < nvars; ++i) {
        DdNode* lit = (initM[i] == 1) ? vars[i] : Cudd_Not(vars[i]);
        DdNode* tmp = Cudd_bddAnd(mgr, S, lit); Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgr, S); S = tmp;
    }

    DdNode* R = pn.build_transition_relation(mgr, vars, primed_vars);

    DdNode* current_vars_cube = Cudd_ReadOne(mgr); Cudd_Ref(current_vars_cube);
    for (auto v : vars) { 
        DdNode* tmp = Cudd_bddAnd(mgr, current_vars_cube, v); Cudd_Ref(tmp); 
        Cudd_RecursiveDeref(mgr, current_vars_cube); current_vars_cube = tmp; 
    }

    std::vector<DdNode*> compose_vec(2 * nvars);
    for (int i = 0; i < nvars; ++i) {
        compose_vec[i] = vars[i]; 
        compose_vec[nvars + i] = vars[i]; 
    }

    while (true) {
        DdNode* conj = Cudd_bddAnd(mgr, S, R); Cudd_Ref(conj);
        DdNode* img_p = Cudd_bddExistAbstract(mgr, conj, current_vars_cube); Cudd_Ref(img_p);
        DdNode* img = Cudd_bddVectorCompose(mgr, img_p, compose_vec.data()); Cudd_Ref(img);
        
        DdNode* S_new = Cudd_bddOr(mgr, S, img); Cudd_Ref(S_new);
        
        Cudd_RecursiveDeref(mgr, conj);
        Cudd_RecursiveDeref(mgr, img_p);
        Cudd_RecursiveDeref(mgr, img);

        if (S_new == S) {
            Cudd_RecursiveDeref(mgr, S_new);
            break;
        } else {
            Cudd_RecursiveDeref(mgr, S);
            S = S_new;
        }
    }

    Cudd_RecursiveDeref(mgr, R);
    Cudd_RecursiveDeref(mgr, current_vars_cube);

    root->node = S;
    
    // 3. Kết thúc thời gian và tính toán kết quả Symbolic
    auto symbolic_end_time = std::chrono::high_resolution_clock::now();
    results.symbolic_time_ms = std::chrono::duration<double, std::milli>(symbolic_end_time - symbolic_start_time).count();
    
    // Tính tổng số trạng thái đến được: Cudd_CountPathsToNonZero
    double state_count_double = Cudd_CountPathsToNonZero(root->node);
    results.symbolic_states = (size_t)std::round(state_count_double);
    results.symbolic_bdd_nodes = Cudd_DagSize(root->node);

    std::cout << "BDDManager initialized. Symbolic reachable set built.\n";
}

BDDManager::~BDDManager() {
    if (root) {
        if (root->node) Cudd_RecursiveDeref(mgr, root->node);
        delete root;
    }
    if (mgr) Cudd_Quit(mgr);
}

// ... (Các hàm markingToCube, internalCheck, checkReachability không đổi) ...
DdNode* BDDManager::markingToCube(const std::vector<int>& marking) {
    if (marking.size() != vars.size()) return nullptr;
    
    DdNode* cube = Cudd_ReadOne(mgr); Cudd_Ref(cube);
    for (size_t i = 0; i < vars.size(); ++i) {
        DdNode* lit = (marking[i] == 1) ? vars[i] : Cudd_Not(vars[i]);
        DdNode* tmp = Cudd_bddAnd(mgr, cube, lit); Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgr, cube);
        cube = tmp;
    }
    return cube;
}

bool BDDManager::internalCheck(BDDNode* node, const std::vector<int>& inputs) {
    if (!node || !node->node) return false;

    int n = Cudd_ReadSize(mgr);
    int* input_array = new int[n];
    memset(input_array, 0, sizeof(int) * n);

    for (size_t i = 0; i < inputs.size() && i < vars.size(); ++i) {
        input_array[i] = inputs[i]; 
    }

    DdNode* res = Cudd_Eval(mgr, node->node, input_array);
    bool isReachable = !Cudd_IsComplement(res);

    delete[] input_array;
    return isReachable;
}

bool BDDManager::checkReachability(const std::vector<int>& marking) {
    if (marking.size() != ordered_place_ids.size()) {
        return false;
    }

    DdNode* mCube = markingToCube(marking);
    DdNode* intersection = Cudd_bddAnd(mgr, root->node, mCube); Cudd_Ref(intersection);
    
    bool reachable = (intersection != Cudd_Not(Cudd_ReadOne(mgr)));
    
    Cudd_RecursiveDeref(mgr, intersection);
    Cudd_RecursiveDeref(mgr, mCube);

    return reachable;
}

const std::vector<std::string>& BDDManager::getPlaceOrder() const {
    return ordered_place_ids;
}

// NEW: Hàm in ra báo cáo hiệu suất
void BDDManager::printPerformanceReport() const {
    std::cout << "\n=======================================================\n";
    std::cout << "           BÁO CÁO HIỆU SUẤT TÍNH REACHABILITY         \n";
    std::cout << "=======================================================\n";
    std::cout << "Số lượng biến (Place): " << ordered_place_ids.size() << "\n";
    std::cout << "Thu tu Place: ";
    for (const auto& name : ordered_place_ids) {
        std::cout << name << " ";
    }
    std::cout << "\n-------------------------------------------------------\n";
    
    // Symbolic BDD Results
    std::cout << "PHƯƠNG PHÁP SYMBOLIC (BDD):\n";
    std::cout << "  - BDD Size (Số Node):  " << results.symbolic_bdd_nodes << "\n";
    std::cout << "  - Tổng trạng thái đến: " << results.symbolic_states << "\n";
    std::cout << "  - Thời gian tính toán: " << std::fixed << std::setprecision(3) << results.symbolic_time_ms << " ms\n";

    std::cout << "-------------------------------------------------------\n";

    // Explicit Results
    std::cout << "PHƯƠNG PHÁP TƯỜNG MINH (EXPLICIT/BFS):\n";
    std::cout << "  - Tổng trạng thái đến: " << results.explicit_states << "\n";
    std::cout << "  - Thời gian tính toán: " << std::fixed << std::setprecision(3) << results.explicit_time_ms << " ms\n";

    std::cout << "-------------------------------------------------------\n";
    
    // Comparison
    std::cout << "SO SÁNH HIỆU SUẤT:\n";
    if (results.symbolic_time_ms > 0 && results.explicit_time_ms > 0) {
        double time_ratio = results.explicit_time_ms / results.symbolic_time_ms;
        if (time_ratio > 1.0) {
            std::cout << "  - Symbolic nhanh hơn Explicit: " << std::fixed << std::setprecision(2) << time_ratio << " lần\n";
        } else {
            std::cout << "  - Explicit nhanh hơn Symbolic: " << std::fixed << std::setprecision(2) << (1.0/time_ratio) << " lần\n";
        }
    }
    std::cout << "=======================================================\n";
}

// NEW: Hàm in ra tất cả các marking đến được
void BDDManager::printReachableMarkings() const {
    size_t n_vars = ordered_place_ids.size();

    DdNode* S = root->node;
    if (S == Cudd_Not(Cudd_ReadOne(mgr))) {
        std::cout << "Tập trạng thái đến được là rỗng (Empty Reachable Set).\n";
        return;
    }

    // Sao chép BDD gốc để thực hiện phép trừ lặp
    DdNode* temp_S = S;
    Cudd_Ref(temp_S); 

    size_t count = 0;
    // const_cast<DdNode**> là cần thiết vì hàm CUDD không được khai báo là const
    DdNode** vars_ptr = const_cast<DdNode**>(vars.data()); 

    // Lặp cho đến khi tập trạng thái còn lại là rỗng (hằng số 0)
    while (temp_S != Cudd_Not(Cudd_ReadOne(mgr))) {
        // 1. Chọn một minterm (cube) bằng hàm Cudd_bddPickOneMinterm
        DdNode* cube = Cudd_bddPickOneMinterm(mgr, temp_S, vars_ptr, (int)n_vars);
        Cudd_Ref(cube); 
        
        // 2. Trích xuất minterm BDD thành vector Marking an toàn
        Marking current_marking((int)n_vars, 0);
        int* minterm_array = (int*)malloc(sizeof(int) * n_vars);
        
        // Sử dụng Cudd_BddToCubeArray để chuyển BDD cube thành mảng [0, 1, 2]
        int success_cube = Cudd_BddToCubeArray(mgr, cube, minterm_array);

        if (success_cube == 0) {
             std::cerr << "Lỗi: Cudd_BddToCubeArray thất bại.\n";
             free(minterm_array);
             Cudd_RecursiveDeref(mgr, cube);
             Cudd_RecursiveDeref(mgr, temp_S);
             return;
        }
        
        for(size_t i=0; i < n_vars; ++i) {
            // minterm_array[i] là 1 (TRUE) hoặc 0 (FALSE).
            if (minterm_array[i] == 1) current_marking[i] = 1;
            else current_marking[i] = 0; 
        }
        free(minterm_array);
        
        // 3. In Marking
        std::cout << "Marking " << ++count << ": [";
        for (size_t i = 0; i < n_vars; ++i) {
            std::cout << current_marking[i] << (i < n_vars - 1 ? ", " : "");
        }
        std::cout << "]\n";
        
        // 4. Trừ minterm vừa tìm được khỏi tập trạng thái
        DdNode* not_cube = Cudd_Not(cube);
        Cudd_Ref(not_cube);
        
        DdNode* new_temp_S = Cudd_bddAnd(mgr, temp_S, not_cube);
        Cudd_Ref(new_temp_S);
        
        // Giải phóng tham chiếu cũ
        Cudd_RecursiveDeref(mgr, temp_S);
        Cudd_RecursiveDeref(mgr, cube); 
        Cudd_RecursiveDeref(mgr, not_cube);

        temp_S = new_temp_S; // Cập nhật temp_S
    }
    
    // Giải phóng tham chiếu cuối cùng
    Cudd_RecursiveDeref(mgr, temp_S);

    std::cout << "-------------------------------------------------------\n";
    std::cout << "Tổng cộng: " << count << " trạng thái đã được in ra.\n";
    std::cout << "=======================================================\n";
}

//HoangAnh

void BDDManager::dfs_optimization(DdNode* u, Marking& M) {
    if (u == Cudd_Not(Cudd_ReadOne(mgr))) { 
        return;
    } 
    
    if (u == Cudd_ReadOne(mgr)) { 
        int val = 0;
        for (size_t i = 0; i < n_vars; i++)
            val += c_weights[i] * M[i];

        if (val > bestValue) {
            bestValue = val;
            bestM = M;
        }
        return;
    }

    //Lấy biến và chỉ số tại nút hiện tại
    int v = Cudd_NodeReadIndex(u); 

    //Lấy các nút con
    DdNode* low_child = Cudd_ReadLogicZero(mgr); 
    DdNode* high_child = Cudd_ReadLogicZero(mgr);
    
    //CUDD không lưu var index trên node, mà dùng level/index. Ta dùng Cudd_Regular/Cudd_T
    DdNode* T = Cudd_Regular(u); 
    if (T == Cudd_ReadOne(mgr)) return; 

    low_child = Cudd_E(T); // Child when var = 0
    high_child = Cudd_T(T); // Child when var = 1

    if (Cudd_IsComplement(u)) {
        if (low_child != Cudd_Not(Cudd_ReadOne(mgr))) low_child = Cudd_Not(low_child);
        if (high_child != Cudd_Not(Cudd_ReadOne(mgr))) high_child = Cudd_Not(high_child);
    }

    //Duyệt nhánh x[v] = 0 (low)
    M[v] = 0;
    dfs_optimization(low_child, M);

    //Duyệt nhánh x[v] = 1 (high)
    M[v] = 1;
    dfs_optimization(high_child, M);
}


void BDDManager::findOptimalMarking(const std::vector<int>& c_weights_in) {
    if (!root || root->node == Cudd_Not(Cudd_ReadOne(mgr))) {
        std::cout << "Không có trạng thái đến được. Không thể tối ưu hóa.\n";
        return;
    }


    this->c_weights = c_weights_in;
    this->n_vars = ordered_place_ids.size();
    
    if (this->c_weights.size() != this->n_vars) {
        std::cerr << "Lỗi: Số lượng trọng số không khớp với số lượng Place.\n";
        return;
    }

    // Khởi tạo các giá trị tối ưu
    bestValue = INT_MIN;
    bestM.assign(n_vars, 0); 
    Marking currentM(n_vars, 0);

    // Gọi hàm DFS đệ quy
    dfs_optimization(root->node, currentM);

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    // In kết quả
    std::cout << "=======================================================\n";
    std::cout << "Hàm mục tiêu: max( ";
    for(size_t i=0; i<n_vars; ++i) {
        std::cout << c_weights[i] << "*" << ordered_place_ids[i] << (i < n_vars-1 ? " + " : "");
    }
    std::cout << " )\n";

    if (bestValue == INT_MIN) {
        std::cout << "Không tìm thấy trạng thái đến được nào.\n";
    } else {
        std::cout << "Gia tri toi uu: " << bestValue << "\n";
        std::cout << "Marking toi uu: [";
        for (size_t i = 0; i < n_vars; i++) {
            std::cout << bestM[i] << (i < n_vars - 1 ? ", " : "");
        }
        std::cout << "]\n";
    }
    std::cout << "=======================================================\n";
}

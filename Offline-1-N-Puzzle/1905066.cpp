#include<bits/stdc++.h>
#include<memory>

using namespace std;

#define BLANK_VALUE 0
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define MANHATTAN 0
#define HAMMING 1
#define NodePtr shared_ptr<Node>
#define SIZE_LIMIT 10000000

int n;

struct Node{
    int blank_pos_x, blank_pos_y;
    vector<vector<int>> board;
    int distance_from_start_node, hamming_distance = -1, manhattan_distance = -1;
    NodePtr parent_node = nullptr;
    int last_move = -1;

    Node(){
        // make the board n*n;
        board.resize(n, vector<int>(n));
        distance_from_start_node = 0;
    }

    void set_value(int x, int y, int value){
        board[x][y] = value;
        if(value == BLANK_VALUE){
            blank_pos_x = x;
            blank_pos_y = y;
        }
    }

    bool operator==(Node &node){
        return board == node.board;
    }

    // ---------------------------- getters ----------------------------
    int get_value(int x, int y){
        return board[x][y];
    }

    int get_distance_from_start_node(){
        return distance_from_start_node;
    }

    vector<vector<int>> get_board(){
        return board;
    }

    int get_hamming_distance(){
        if(hamming_distance != -1) return hamming_distance;

        hamming_distance = 0;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                if(board[i][j] != BLANK_VALUE && board[i][j] != i*n + j + 1) hamming_distance++;
            }
        }
        return hamming_distance;
    }
    
    int get_manhattan_distance(){
        if(manhattan_distance != -1) return manhattan_distance;

        manhattan_distance = 0;
        int value, x, y;
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                value = board[i][j];
                if(value != BLANK_VALUE){
                    x = (value-1)/n;
                    y = (value-1)%n;
                    manhattan_distance += abs(x-i) + abs(y-j);
                }
            }
        }
        return manhattan_distance;
    }

    
    // ---------------------------- print board ----------------------------
    void print_board(){
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }
};


struct Solution{
    int expanded_nodes = 0;
    int explored_nodes = 0;
    vector<NodePtr> path;

    Solution(NodePtr node, int expanded_nodes = 0, int explored_nodes = 1){
        save_path(node);
        this->expanded_nodes = expanded_nodes;
        this->explored_nodes = explored_nodes;
    }

    void save_path(NodePtr node){
        if(node == nullptr) return;
        save_path(node->parent_node);
        path.push_back(node);
    }

    void print_path(){
        for(auto node: path){
            node->print_board();
            cout<<endl;
        }
    }

    int get_minimum_moves(){
        return path.size()-1;
    }

};


// returns a new node after moving the blank. If not a valid move, returns nullptr
NodePtr move_blank(NodePtr node, int move){
    int x = node->blank_pos_x;
    int y = node->blank_pos_y;

    if(x == 0 && move == UP) return nullptr;
    if(x == n-1 && move == DOWN) return nullptr;
    if(y == 0 && move == LEFT) return nullptr;
    if(y == n-1 && move == RIGHT) return nullptr;

    if(node->last_move == UP && move == DOWN) return nullptr;
    if(node->last_move == DOWN && move == UP) return nullptr;
    if(node->last_move == LEFT && move == RIGHT) return nullptr;
    if(node->last_move == RIGHT && move == LEFT) return nullptr;

    NodePtr new_node = make_shared<Node>(*node);

    
    if(move == LEFT) swap(new_node->board[x][y], new_node->board[x][y-1]);
    else if(move == RIGHT) swap(new_node->board[x][y], new_node->board[x][y+1]);
    else if(move == UP) swap(new_node->board[x][y], new_node->board[x-1][y]);
    else if(move == DOWN) swap(new_node->board[x][y], new_node->board[x+1][y]);

    new_node->blank_pos_x = x + (move == DOWN) - (move == UP);
    new_node->blank_pos_y = y + (move == RIGHT) - (move == LEFT);

    new_node->distance_from_start_node = node->distance_from_start_node + 1;
    new_node->hamming_distance = -1;
    new_node->manhattan_distance = -1;
    new_node->parent_node = node;
    new_node->last_move = move;

    new_node->get_manhattan_distance();
    new_node->get_hamming_distance();

    return new_node;
}


// ---------------------------- use merge sort to count inversions ----------------------------
int merge(vector<int> &vec, int start, int mid, int end){
    int inversions = 0;
    int n1 = mid - start + 1;
    int n2 = end - mid;


    vector<int> left(n1), right(n2);
    for(int i = 0 ; i<n1; i++) left[i] = vec[start + i];
    for(int i = 0 ; i<n2; i++) right[i] = vec[mid + 1 + i];

    int i = 0, j = 0;

    for(int k = start; k<=end; k++){
        if(i>=n1) vec[k] = right[j++];
        else if(j>=n2) {
            vec[k] = left[i++];
            
        }
        else if(left[i]<=right[j]) vec[k] = left[i++];
        else{
            vec[k] = right[j++];
            inversions += n1 - i;
        }
    }

    return inversions;
}


int mergeSort(vector<int> &vec, int start, int end){
    int inversions = 0;
    if(start < end){
        int mid  = (start + end) / 2;
        inversions += mergeSort(vec, start, mid);
        inversions += mergeSort(vec, mid+1, end);
        inversions += merge(vec, start, mid, end);
    }
    return inversions;
}


bool is_solvable(NodePtr node){
    vector<int> vec;
    int value;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            value = node->get_value(i, j);
            if(value != BLANK_VALUE) vec.push_back(node->get_value(i, j));
        }
    }

   
    int inversions = mergeSort(vec, 0, vec.size()-1);

    if(n%2 == 1 && inversions%2==0) return true;

    if(n%2 == 0){
        int blank_row = n - node->blank_pos_x;
        if(blank_row%2 == 0 && inversions%2 == 1) return true;
        if(blank_row%2 == 1 && inversions%2 == 0) return true;
    }
    return false;
}


bool compare_hamming(NodePtr node1, NodePtr node2){
    int dist1 = node1->get_distance_from_start_node() + node1->get_hamming_distance();
    int dist2 = node2->get_distance_from_start_node() + node2->get_hamming_distance();
    if(dist1 == dist2) return node1->distance_from_start_node > node2->distance_from_start_node;
    return dist1 > dist2;
}

bool compare_manhattan(NodePtr node1, NodePtr node2){
    int dist1 = node1->get_distance_from_start_node() + node1->get_manhattan_distance();
    int dist2 = node2->get_distance_from_start_node() + node2->get_manhattan_distance();
    if(dist1 == dist2) return node1->distance_from_start_node > node2->distance_from_start_node;
    return dist1 > dist2;
}


Solution solve(NodePtr &start_node, int heuristic = MANHATTAN){
    
    auto cmp = compare_manhattan;
    if(heuristic == HAMMING) cmp = compare_hamming;
    int expanded_nodes = 0;
    int explored_nodes = 1;
    NodePtr node;
    
    if(start_node->get_hamming_distance() == 0)
        return Solution(start_node, expanded_nodes, explored_nodes);
    
    priority_queue<NodePtr, vector<NodePtr>, function<bool(NodePtr, NodePtr)>> pq(cmp);
    map<vector<vector<int>>, bool> visited;
    
    visited[start_node->get_board()] = true;
    pq.push(start_node);

    while(!pq.empty()){
        node = pq.top();
        pq.pop();
       
        auto up = move_blank(node, UP);
        auto down = move_blank(node, DOWN);
        auto left = move_blank(node, LEFT);
        auto right = move_blank(node, RIGHT);

        expanded_nodes++;
        
        // check if valid move, check if visited, check if goal reached
        // push to pq if valid
        // update visited
        if(up && visited.find(up->get_board()) == visited.end()) {
            pq.push(up);
            explored_nodes++;
            if(visited.size() < SIZE_LIMIT) visited[up->get_board()] = true;
            if(up->get_hamming_distance() == 0) {node = up; break;}
        }

        if(down && visited.find(down->get_board()) == visited.end()) {
            pq.push(down);
            explored_nodes++;
            if(visited.size() < SIZE_LIMIT) visited[down->get_board()] = true;
            if(down->get_hamming_distance() == 0) {node = down; break;}
        }

        if(left && visited.find(left->get_board()) == visited.end()) {
            pq.push(left);
            explored_nodes++;
            if(visited.size() < SIZE_LIMIT) visited[left->get_board()] = true;
            if(left->get_hamming_distance() == 0) {node = left; break;}
        }

        if(right && visited.find(right->get_board()) == visited.end()) {
            pq.push(right);
            explored_nodes++;
            if(visited.size() < SIZE_LIMIT) visited[right->get_board()] = true;
            if(right->get_hamming_distance() == 0) {node = right; break;}
        } 
    }

    return Solution(node, expanded_nodes, explored_nodes);
}


int main(){

    freopen("318_offline_1/in09.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    
    cin >> n;
    
    
    NodePtr start_node = make_shared<Node>();
   
    int value;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            cin >> value;
            start_node->set_value(i, j, value);
        }
    }


    if(is_solvable(start_node)){
        //Solution solution = solve(start_node, HAMMING);
        Solution solution2 = solve(start_node, MANHATTAN);

        // cout<<"For Hamming Distance"<<endl;
        // cout<<"Number of expanded nodes: "<<solution.expanded_nodes<<endl;
        // cout<<"Number of explored nodes: "<<solution.explored_nodes<<endl;
        // cout<<"---------------------------------"<<endl;
       
        cout<<"For Manhattan Distance:"<<endl;
        cout<<"Number of expanded nodes: "<<solution2.expanded_nodes<<endl;
        cout<<"Number of explored nodes: "<<solution2.explored_nodes<<endl;
        cout<<"---------------------------------"<<endl;


        cout<<"Minimum number of moves: "<<solution2.get_minimum_moves()<<endl;
        solution2.print_path();
    }

    else{
        cout<<"Unsolvable puzzle"<<endl;
    }
    
    return 0;
}
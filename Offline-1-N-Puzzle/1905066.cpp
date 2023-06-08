#include<bits/stdc++.h>

using namespace std;

#define BLANK_VALUE 0
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define MANHATTAN 0
#define HAMMING 1

int n;

struct Node{
    int blank_pos_x, blank_pos_y;
    vector<vector<int>> board;
    int distance_from_start_node, hamming_distance = -1, manhattan_distance = -1;
    Node* parent_node = NULL;
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

    int get_hamming_distance();
    int get_manhattan_distance();

    
    // ---------------------------- print board ----------------------------
    void print(){
        for(int i=0; i<n; i++){
            for(int j=0; j<n; j++){
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }
};


int calculate_hamming_distance(Node *node){
    int distance = 0;
    int value;
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            value = node->get_value(i, j);
            if(value != BLANK_VALUE && value != i*n + j + 1) distance++;
        }
    }
    return distance;
}

int calculate_manhattan_distance(Node *node){
    int distance = 0;
    int value, x, y;
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            value = node->get_value(i, j);
            if(value != BLANK_VALUE){
                x = (value-1)/n;
                y = (value-1)%n;
                distance += abs(x-i) + abs(y-j);
            }
        }
    }
    return distance;
}

int Node::get_hamming_distance(){
    if(hamming_distance == -1) hamming_distance = calculate_hamming_distance(this);
    return hamming_distance;
}

int Node::get_manhattan_distance(){
    if(manhattan_distance == -1) manhattan_distance = calculate_manhattan_distance(this);
    return manhattan_distance;
}


Node* move_blank(Node* node, int move){
    int x = node->blank_pos_x;
    int y = node->blank_pos_y;

    if(x == 0 && move == UP) return NULL;
    if(x == n-1 && move == DOWN) return NULL;
    if(y == 0 && move == LEFT) return NULL;
    if(y == n-1 && move == RIGHT) return NULL;

    if(node->last_move == UP && move == DOWN) return NULL;
    if(node->last_move == DOWN && move == UP) return NULL;
    if(node->last_move == LEFT && move == RIGHT) return NULL;
    if(node->last_move == RIGHT && move == LEFT) return NULL;

    Node *new_node = new Node(*node);
    
    if(move == LEFT) swap(new_node->board[x][y], new_node->board[x][y-1]);
    else if(move == RIGHT) swap(new_node->board[x][y], new_node->board[x][y+1]);
    else if(move == UP) swap(new_node->board[x][y], new_node->board[x-1][y]);
    else if(move == DOWN) swap(new_node->board[x][y], new_node->board[x+1][y]);

    new_node->blank_pos_x = x + (move == DOWN) - (move == UP);
    new_node->blank_pos_y = y + (move == RIGHT) - (move == LEFT);

    new_node->distance_from_start_node = node->distance_from_start_node + 1;
    new_node->hamming_distance = calculate_hamming_distance(new_node);
    new_node->manhattan_distance = calculate_manhattan_distance(new_node);
    new_node->parent_node = node;
    new_node->last_move = move;

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
            inversions++;
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


bool is_solvable(Node *node){
    vector<int> vec;
    int value;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            value = node->get_value(i, j);
            if(value != BLANK_VALUE) vec.push_back(node->get_value(i, j));
        }
    }

    int inversions = mergeSort(vec, 0, n*n-2);

    if(n%2 == 1 && inversions%2==0) return true;

    if(n%2 == 0){
        int blank_row = n - node->blank_pos_x;
        if(blank_row%2 == 0 && inversions%2 == 1) return true;
        if(blank_row%2 == 1 && inversions%2 == 0) return true;
    }
    return false;
}


bool compare_hamming(Node* node1, Node* node2){
    return node1->get_distance_from_start_node() + node1->get_hamming_distance() > node2->get_distance_from_start_node() + node2->get_hamming_distance();
}

bool compare_manhattan(Node* node1, Node* node2){
    return node1->get_distance_from_start_node() + node1->get_manhattan_distance() > node2->get_distance_from_start_node() + node2->get_manhattan_distance();
}

void solve(Node* start_node, int heuristic = MANHATTAN){

    auto cmp = compare_manhattan;
    if(heuristic == HAMMING) cmp = compare_hamming;
    priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)>> pq(cmp);

    Node* up = move_blank(start_node, UP);
    Node* down = move_blank(start_node, DOWN);
    Node* left = move_blank(start_node, LEFT);
    Node* right = move_blank(start_node, RIGHT);

    if(up != NULL) pq.push(up);
    if(down != NULL) pq.push(down);
    if(left != NULL) pq.push(left);
    if(right != NULL) pq.push(right);

    int cnt = 0;

    while(!pq.empty()){
        Node* node = pq.top();
        pq.pop();
        
        // if goal reached
        if(node->get_hamming_distance() == 0){
            cout<<"Solved"<<endl;
            break;
        }

        up = move_blank(node, UP);
        down = move_blank(node, DOWN);
        left = move_blank(node, LEFT);
        right = move_blank(node, RIGHT);

        if(up != NULL) pq.push(up);
        if(down != NULL) pq.push(down);
        if(left != NULL) pq.push(left);
        if(right != NULL) pq.push(right);
    }
}

int main(){

    freopen("input.txt", "r", stdin);
    
    cin >> n;
    
    
    Node *start_node = new Node;
   
    int value;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            cin >> value;
            start_node->set_value(i, j, value);
        }
    }


    if(is_solvable(start_node)){
        solve(start_node, HAMMING);
    }

    else{
        cout<<"Unsolvable puzzle"<<endl;
    }
    
    return 0;
}
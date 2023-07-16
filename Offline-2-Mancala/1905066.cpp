#include<bits/stdc++.h>
#include "defs.h"
#include "node.h"
#include "heuristic.h"

using namespace std;

std::function<double(NodePtr, int, double[])> heuristics[] = {heuristic1, heuristic1, heuristic2, heuristic3, heuristic4};
double w1[10], w2[10];

// minimax with alpha beta pruning
// return {score, move}
pair<double,int> minmax(NodePtr node, double alpha, double beta, int depth, int player, function<double(NodePtr, int, double[])> evaluate, double w[]){
    int game_over = node->gameOver();
    if(game_over == PLAYER1) return {INF, 0};
    if(game_over == PLAYER2) return {-INF, 0};

    //if (depth==0) return {evaluate(node, node->prev_player, w), 0};

    if(depth == 0) {
        //return {evaluate(node, player, w), 0}; // leaf node
        if(player == PLAYER1) return {evaluate(node, player, w), 0}; 
        return {-evaluate(node, player, w), 0};
    }


    int best_move = -1;
    double best;

    if(player == PLAYER1){
        best = -INF;
        for(int i = 6; i >=1; i--){
            if(node->isValidMove(player, i)){
                NodePtr next = node->move(player, i);
                double score = minmax(next, alpha, beta, depth - 1, next->next_player, evaluate, w).first;
                if(score >= best){
                    best = score;
                    best_move = i;
                }
                alpha = max(alpha, best);
                if(alpha >= beta) break;
            }
        }
    }
    else{
        best = INF;
        for(int i = 6; i >=1; i--){
            if(node->isValidMove(player, i)){
                NodePtr next = node->move(player, i);
                double score = minmax(next, alpha, beta, depth - 1, next->next_player, evaluate, w).first;
                if(score <= best){
                    best = score;
                    best_move = i;
                }
                beta = min(beta, best);
                if(alpha >= beta) break;
            }
        }
    }

    return {best, best_move};
}


int gameCvC(int h1, int h2, double w1[], double w2[], int depth1, int depth2){
    NodePtr node = make_shared<Node>();
    node->board = {0,4,4,4,4,4,4,0,4,4,4,4,4,4};

    // node->print_board();

    int player = PLAYER1, winner, cnt = 0;
    pair<double,int> best;
    while ((winner = node->gameOver()) == 0)
    {
        if(player == PLAYER1) best = minmax(node, -INF, INF, depth1, player, heuristics[h1], w1);
        
        else best = minmax(node, -INF, INF, depth2, player, heuristics[h2], w2);
            

        //cout<<"Player "<<player<<"'s turn : "<<best.second<<endl;
        
        node = node->move(player, best.second);
        
        //node->print_board();
        //cout<<endl;

        player = node->next_player;
        cnt++;
    }

    //cout<<"Total moves : "<<cnt<<endl;
    //node->print_board();
    return winner;
}

int gameCvH(int h, double w[], int depth){
    NodePtr node = make_shared<Node>();
    node->board = {0,4,4,4,4,4,4,0,4,4,4,4,4,4};
    node->print_board();

    int player = PLAYER1, winner, pos;
    while ((winner = node->gameOver()) == 0)
    {
        cout<<"Player "<<player<<"'s turn : ";

        if(player == PLAYER1){
            cin>>pos;
            if(!node->isValidMove(player, pos)){
                cout<<"Invalid move"<<endl;
                continue;
            }
            node = node->move(player, pos);
            player = node->next_player;
        }
        
        else{
            pair<int,int> best = minmax(node, -INF, INF, depth, player, heuristics[h], w);
            cout<<"best move : "<<best.second<<endl;
            node = node->move(player, best.second);
            player = node->next_player;
        }
        

        node->print_board();
        
        cout<<endl;
    }


    return winner;
}


void test(int h1, int h2, int depth){
    int cnt1 = 0, cnt2 = 0;
    for(int i = 0; i < 10; i++){

        // random weights
        for(int i = 0; i < 5; i++){
        w1[i] = (30 + (rand() % 100)) / 100.0;
        w2[i] = (30 + (rand() % 100)) / 100.0;
        }

        int winner = gameCvC(h1, h2, w1, w2, depth, depth);
        cout<<"Winner : "<<winner<<endl;

        if(winner == PLAYER1) cnt1++;
        else if(winner == PLAYER2) cnt2++;
        
    }

    for(int i = 0; i < 10; i++){

        // random weights
        for(int i = 0; i < 5; i++){
        w1[i] = (30 + (rand() % 100)) / 100.0;
        w2[i] = (30 + (rand() % 100)) / 100.0;
        }

        int winner = gameCvC(h2, h1, w1, w2, depth, depth);
        cout<<"Winner : "<<3-winner<<endl;

        if(winner == PLAYER1) cnt2++;
        else if(winner == PLAYER2) cnt1++;
        
    }

    cout<<h1<<","<<h2<<","<<cnt1<<endl;
    cout<<h2<<","<<h1<<","<<cnt2<<endl;

}

int main(){
    //freopen("output.txt", "w", stdout);

    //test(4,1,12);
    w1[0] = 0.5;
    w1[1] = 0.3;
    w1[2] = 0.8;
    w1[3] = 0.7; 
    gameCvH(4, w1, 10);
   
    return 0;
}
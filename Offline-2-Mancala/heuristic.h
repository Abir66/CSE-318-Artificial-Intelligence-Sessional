#ifndef heuristic_h
#define heuristic_h

#include "defs.h"
#include "node.h"

using namespace std;

// heuristic-1: 
// (stones_in_my_storage - stones_in_opponents_storage)
double heuristic1(NodePtr node, int player, double w[]){
    // need to normalize
    if(player == PLAYER1) return (node->board[7] - node->board[0]) / 72.0;
    else return (node->board[0] - node->board[7]) / 72.0;
}

// heuristic-2: 
// W1 * (stones_in_my_storage – stones_in_opponents_storage) + W2 * (stones_on_my_side – stones_on_opponents_side)
double heuristic2(NodePtr node, int player, double w[]){
    double h0 = heuristic1(node, player, w);
    double h1 = 0;
    for(int i = 1; i <= 6; i++){
        if(player == PLAYER1) h1 += node->board[i] - node->board[i + 7];
        else h1 += node->board[i + 7] - node->board[i];
    }
    return h0 + w[1] * h1 / 24.0;
}

// heuristic-3:
// W1 * (stones_in_my_storage - stones_in_opponents_storage) + W2 * (stones_on_my_side - stones_on_opponents_side) + W3 * (additional_move_earned)
double heuristic3(NodePtr node, int player, double w[]){
    double h1 = heuristic2(node, player, w);
    double h2 = 0;
    h2 = player == PLAYER1 ? node->p1_extra_move : node->p2_extra_move;

    return h1 + w[2] * h2 / 2.0;
    
}

// heuristic-4: 
// W1 * (stones_in_my_storage - stones_in_opponents_storage) + W2 * (stones_on_my_side - stones_on_opponents_side) + W3 * (additional_move_earned) + W4 * (stones_captured)
double heuristic4(NodePtr node, int player, double w[]){
    double h2 = heuristic3(node, player, w);
    double h3 = 0;
    h3 = player == PLAYER1 ? node->p1_captured : node->p2_captured;

    return h2 + w[3] * h3 / 24.0;
}


#endif  /* heuristic_h */
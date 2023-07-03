#ifndef NODE_H
#define NODE_H

#include <bits/stdc++.h>
#include "defs.h"

using namespace std;

struct Node{

    vector<int> board = vector<int>(14);
    int next_player;
    int p1_extra_move = 0;
    int p2_extra_move = 0;
    int p1_captured = 0;
    int p2_captured = 0;


    bool isValidMove(int player, int pos){
        pos = player == PLAYER1 ? pos : pos + 7;
        return board[pos] > 0;
    }

    // return 0 if game is not over
    // return 1 if player 1 wins
    // return 2 if player 2 wins
    int gameOver(){
        bool player1 = true, player2 = true;
        for(int i = 1; i <= 6; i++){
            if(board[i] > 0) player1 = false;
            if(board[i + 7] > 0) player2 = false;
        }
        if(player1 || player2) {
            if(board[7] > board[0]) return 1;
            else return 2;
        }
        return 0;
    }

    NodePtr move(int player, int pos){
        NodePtr next = make_shared<Node>(*this);
        // next->board = board;
        // next->p1_extra_move = p1_extra_move;
        // next->p2_extra_move = p2_extra_move;
        pos = player == PLAYER1 ? pos : pos + 7;
        int stones = next->board[pos];
        int captured = 0;
        
        next->board[pos] = 0;
        
        while(stones > 0){
            pos = (pos + 1) % 14;
            if(player == PLAYER1 && pos == 0) continue;
            if(player == PLAYER2 && pos == 7) continue;
            next->board[pos]++;
            stones--;

            // if last stone was on empty pit
            if(stones == 0 && next->board[pos] == 1 && pos != 0 && pos != 7 && next->board[14 - pos] > 0){
                if(player == PLAYER1 && pos < 7){
                    captured = next->board[pos] + next->board[14 - pos];
                    next->board[7] += captured;
                    next->board[14 - pos] = 0;
                    next->board[pos] = 0;
                    next->p1_captured += captured;
                }
                if(player == PLAYER2 && pos > 7){
                    captured = next->board[pos] + next->board[14 - pos];
                    next->board[0] += captured;
                    next->board[14 - pos] = 0;
                    next->board[pos] = 0;
                    next->p2_captured += captured;
                }
            }

            // else{
            //     p1_captured = 0;
            //     p2_captured = 0;
            // }
        }

        if(player == PLAYER1 && pos == 7) {
            next->next_player = PLAYER1;
            next->p1_extra_move++;
            next->p2_extra_move = 0;
        }
        else if(player == PLAYER2 && pos == 0) {
            next->next_player = PLAYER2; 
            next->p2_extra_move++;
            next->p1_extra_move = 0;
        }
        else {
            next->next_player = player == PLAYER1 ? PLAYER2 : PLAYER1;
            next->p1_extra_move = 0;
            next->p2_extra_move = 0;
        }

        return next;
    }

    // formatted output. each number 3 digit space
    void print_board(){
        cout<<endl;
        cout<< "---------------------------------------------------------------------------------"<<endl;
        cout<< "|         |          |         |         |         |         |         |         |"<<endl;
        cout<< "|         | ";
        for(int i = 13; i > 7; i--) cout << setw(5) << board[i] << "    |";
        cout <<"         |"<<endl;
        cout << setw(0);
        cout<< "|         |          |         |         |         |         |         |         |"<<endl;
        cout<< "|         --------------------------------------------------------------         |"<<endl;
        cout<< "|         |                                                            |         |"<<endl;
        cout <<"|"<< setw(5) << board[0]<< setw(0)<<"    |<- Player 2              MANCALA               Player 1  ->|"<< setw(5) << board[7]<< setw(0) << "     |"<<endl;;
        cout<< "|         |                                                            |         |"<<endl;
        cout<< "|         --------------------------------------------------------------         |"<<endl;
        cout<< "|         |          |         |         |         |         |         |         |"<<endl;
        cout<< "|         | ";
        for(int i = 1; i <= 6; i++) cout << setw(5) << board[i] <<"    |";
        cout <<"         |"<<endl;
        cout<< "|         |          |         |         |         |         |         |         |"<<endl;
        cout<< "---------------------------------------------------------------------------------"<<endl;
        cout<< "                1          2         3         4         5         6"<<endl;
        cout<<endl;

        cout << setw(0);
    }
};


#endif // NODE_H
#ifndef SURAKARTA_AI_H_
#define SURAKARTA_AI_H_

#include <vector>
#include <memory>
#include <utility>
#include <string>
#include <cstdlib>
#include <ctime>

#include "chessboard.h"

// greedy + MCTS?
// TODO: finish this part

class AI {
public:
    AI(ChessBoard &board, ChessType chess) : board_(board), chess_(chess) {
        moving_status_ = chess == ChessType::Red ?
                GameStatus::RedMoving : GameStatus::BlackMoving;
        // randomize
        std::srand(std::time(nullptr));
        search_depth_ = 3;
    }
    ~AI() {}

    std::string NextStep();

    void set_search_depth(int search_depth) {
        if (search_depth <= 0) return;
        search_depth_ = search_depth;
    }
    int search_depth() const { return search_depth_; }

private:
    struct TreeNode {
        using TreePtr = std::unique_ptr<TreeNode>;
        struct TriTuple {
            int evalue;
            ChessBoard board;
            TreePtr child;
        };
        std::vector<TriTuple> node_set;
    };

    Action Simulate();

    ChessBoard &board_;
    ChessType chess_;
    GameStatus moving_status_;
    int search_depth_;
    //
};

#endif // SURAKARTA_AI_H_

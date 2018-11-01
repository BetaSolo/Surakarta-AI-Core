#ifndef SURAKARTA_CHESSBOARD_H_
#define SURAKARTA_CHESSBOARD_H_

#include <chrono>
#include <utility>
#include <functional>
#include <unordered_set>

enum class GameStatus : char {
    Paused, RedMoving, BlackMoving, RedWon, BlackWon
};

enum class Direction : char {
    Up = 1, Down = 2, Left = 4, Right = 8,
    LeftUp = 5, LeftDown = 6, RightUp = 9, RightDown = 10
};

enum class ChessType : char {
    Null, Red, Black
};

using ChessPos = std::pair<int, int>;

struct PosHash {
    std::size_t operator()(const ChessPos &pos) const {
        auto first_hash = std::hash<int>{}(pos.first);
        auto second_hash = std::hash<int>{}(pos.second);
        return first_hash ^ (second_hash << 1);
    }
};

using PosList = std::unordered_set<ChessPos, PosHash>;

struct Action {
    bool is_move;
    ChessPos current, target;
    ChessType chess;
};

class ChessBoard {
public:
    ChessBoard() { InitBoard(); }
    ~ChessBoard() {}

    void NewGame();
    bool CanMove(ChessPos pos, Direction dir, ChessType chess);
    PosList CanEat(ChessPos pos, ChessType chess);
    bool PlayerMove(ChessPos pos, Direction dir, ChessType chess);
    bool PlayerEat(ChessPos pos, ChessPos eat, ChessType chess);
    bool ApplyAction(const Action &action);

    ChessType chess_status(ChessPos pos) const {
        return board_[pos.second][pos.first];
    }
    GameStatus status() const { return status_; }

    // void Debug() {
    //     const int chess_arr[6][6] = {
    //         {0, 0, 0, 0, 0, 0},
    //         {0, 0, 0, 0, 0, 2},
    //         {0, 0, 0, 1, 0, 0},
    //         {0, 0, 0, 0, 1, 0},
    //         {0, 0, 0, 0, 0, 0},
    //         {1, 0, 0, 0, 0, 1}
    //     };
    //     for (int i = 0; i < 6; ++i) {
    //         for (int j = 0; j < 6; ++j) {
    //             board_[i][j] = static_cast<ChessType>(chess_arr[i][j]);
    //         }
    //     }
    //     status_ = GameStatus::RedMoving;
    // }

private:
    void InitBoard();
    bool CheckValidMove(ChessPos pos, ChessType chess);
    ChessPos GetEatPos(ChessPos pos,
            Direction dir, ChessType chess, int arc_count);
    void RefreshStatus();

    ChessType board_[6][6];
    GameStatus status_;
};

#endif // SURAKARTA_CHESSBOARD_H_

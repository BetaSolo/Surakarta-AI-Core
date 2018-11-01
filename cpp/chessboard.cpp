#include "chessboard.h"

#include <algorithm>

namespace {

inline bool GetOffset(int &x, int &y, Direction dir) {
    const auto up_value = static_cast<char>(Direction::Up);
    const auto down_value = static_cast<char>(Direction::Down);
    const auto left_value = static_cast<char>(Direction::Left);
    const auto right_value = static_cast<char>(Direction::Right);
    auto dir_value = static_cast<char>(dir);
    if ((dir_value & up_value)) {
        y -= 1;
    }
    else if ((dir_value & down_value)) {
        y += 1;
    }
    if ((dir_value & left_value)) {
        x -= 1;
    }
    else if ((dir_value & right_value)) {
        x += 1;
    }
    return x >= 0 && x <= 5 && y >= 0 && y <= 5;
}

} // namespace

void ChessBoard::InitBoard() {
    for (int i = 0; i < 6; ++i) {
        auto &row = board_[i];
        for (auto &&chess : row) {
            if (i <= 1) {
                chess = ChessType::Black;
            }
            else if (i <= 3) {
                chess = ChessType::Null;
            }
            else {
                chess = ChessType::Red;
            }
        }
    }
    status_ = GameStatus::Paused;
}

bool ChessBoard::CheckValidMove(ChessPos pos, ChessType chess) {
    auto x = pos.first, y = pos.second;
    // invalid chess type
    if (chess == ChessType::Null) return false;
    // out of range
    if (x < 0 || x > 5 || y < 0 || y > 5) return false;
    // not in correct status
    if (status_ == GameStatus::Paused || status_ == GameStatus::RedWon
            || status_ == GameStatus::BlackWon) return false;
    // not in correct turn
    if ((chess == ChessType::Black && status_ == GameStatus::RedMoving)
            || (chess == ChessType::Red
            && status_ == GameStatus::BlackMoving)) return false;
    // timeout
    // TODO
    // no chess was moved
    if (board_[y][x] != chess) return false;
    return true;
}

// dfs & tail recursion
ChessPos ChessBoard::GetEatPos(ChessPos pos,
        Direction dir, ChessType chess, int arc_count) {
    auto x = pos.first, y = pos.second;
    // invalid chess type
    if (chess == ChessType::Null) return {-1, -1};
    // in corner
    if ((x == 0 && y == 0) || (x == 0 && y == 5)
            || (x == 5 && y == 0) || (x == 5 && y == 5)) return {-1, -1};
    // recursion
    if (!GetOffset(x, y, dir)) {
        // passing an arc
        static const ChessPos pos_arr[] = {
            {1, -1}, {2, -1}, {2, 6}, {1, 6},
            {4, -1}, {3, -1}, {3, 6}, {4, 6},
            {-1, 1}, {-1, 2}, {6, 2}, {6, 1},
            {-1, 4}, {-1, 3}, {6, 3}, {6, 4}
        };
        auto x_dir = y <= 2 ? Direction::Down : Direction::Up;
        auto y_dir = x <= 2 ? Direction::Right : Direction::Left;
        if (x == -1) {
            return GetEatPos(pos_arr[y - 1], x_dir, chess, arc_count + 1);
        }
        else if (x == 6) {
            return GetEatPos(pos_arr[y + 3], x_dir, chess, arc_count + 1);
        }
        else if (y == -1) {
            return GetEatPos(pos_arr[x + 7], y_dir, chess, arc_count + 1);
        }
        else {   // y == 6
            return GetEatPos(pos_arr[x + 11], y_dir, chess, arc_count + 1);
        }
    }
    else {
        if (board_[y][x] == chess) {
            return {-1, -1};
        }
        else if (board_[y][x] == ChessType::Null) {
            return GetEatPos({x, y}, dir, chess, arc_count);
        }
        else {
            return arc_count ?
                    std::make_pair(x, y) : std::make_pair(-1, -1);
        }
    }
}

void ChessBoard::RefreshStatus() {
    int red_count = 0, black_count = 0;
    // count the number of red & black chess
    for (const auto &row : board_) {
        for (const auto &i : row) {
            if (i == ChessType::Red) {
                ++red_count;
            }
            else if (i == ChessType::Black) {
                ++black_count;
            }
        }
    }
    // update status
    if (red_count == 0) {
        status_ = GameStatus::BlackWon;
    }
    else if (black_count == 0) {
        status_ = GameStatus::RedWon;
    }
    else if (status_ == GameStatus::RedMoving) {
        status_ = GameStatus::BlackMoving;
    }
    else if (status_ == GameStatus::BlackMoving) {
        status_ = GameStatus::RedMoving;
    }
}

void ChessBoard::NewGame() {
    InitBoard();
    status_ = GameStatus::RedMoving;
}

bool ChessBoard::CanMove(ChessPos pos, Direction dir, ChessType chess) {
    if (!CheckValidMove(pos, chess)) return false;
    auto x = pos.first, y = pos.second;
    // invalid move direction
    int new_x = x, new_y = y;
    if (!GetOffset(new_x, new_y, dir)) return false;
    // chess was blocked by another chess
    if (board_[new_y][new_x] != ChessType::Null) return false;
    return true;
}

PosList ChessBoard::CanEat(ChessPos pos, ChessType chess) {
    if (!CheckValidMove(pos, chess)) return {};
    auto x = pos.first, y = pos.second;
    // in corner
    if ((x == 0 && y == 0) || (x == 0 && y == 5)
            || (x == 5 && y == 0) || (x == 5 && y == 5)) return {};
    // get position list
    PosList pos_list;
    auto left = GetEatPos(pos, Direction::Left, chess, 0);
    auto right = GetEatPos(pos, Direction::Right, chess, 0);
    auto up = GetEatPos(pos, Direction::Up, chess, 0);
    auto down = GetEatPos(pos, Direction::Down, chess, 0);
    if (left.first != -1) pos_list.insert(left);
    if (right.first != -1) pos_list.insert(right);
    if (up.first != -1) pos_list.insert(up);
    if (down.first != -1) pos_list.insert(down);
    return std::move(pos_list);
}


bool ChessBoard::PlayerMove(ChessPos pos, Direction dir, ChessType chess) {
    if (!CheckValidMove(pos, chess)) return false;
    auto x = pos.first, y = pos.second;
    // invalid move direction
    int new_x = x, new_y = y;
    if (!GetOffset(new_x, new_y, dir)) return false;
    // chess was blocked by another chess
    if (board_[new_y][new_x] != ChessType::Null) return false;
    // move to specific position
    board_[new_y][new_x] = board_[y][x];
    board_[y][x] = ChessType::Null;
    // update status
    RefreshStatus();
    return true;
}

bool ChessBoard::PlayerEat(ChessPos pos, ChessPos eat, ChessType chess) {
    // get the position list
    auto pos_list = CanEat(pos, chess);
    // find the position of chess that will be eaten
    auto it = std::find(pos_list.begin(), pos_list.end(), eat);
    if (it == pos_list.end()) return false;
    // eat chess
    board_[it->second][it->first] = chess;
    board_[pos.second][pos.first] = ChessType::Null;
    // update status
    RefreshStatus();
    return true;
}

bool ChessBoard::ApplyAction(const Action &action) {
    if (action.is_move) {
        int dir = 0;
        const auto up = static_cast<char>(Direction::Up);
        const auto down = static_cast<char>(Direction::Down);
        const auto left = static_cast<char>(Direction::Left);
        const auto right = static_cast<char>(Direction::Right);
        if (action.target.first - action.current.first == -1) dir |= left;
        if (action.target.first - action.current.first == 1) dir |= right;
        if (action.target.second - action.current.second == -1) dir |= up;
        if (action.target.second - action.current.second == 1) dir |= down;
        auto d = static_cast<Direction>(dir);
        return PlayerMove(action.current, d, action.chess);
    }
    else {
        return PlayerEat(action.current, action.target, action.chess);
    }
}

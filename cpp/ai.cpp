#include "ai.h"

#include <queue>
#include <sstream>

namespace {

int black_evalue[6][6] = {
    {50, 100, 100, 100, 100, 50},
    {100, 120, 150, 150, 120, 100},
    {100, 150, 120, 120, 150, 100},
    {110, 160, 130, 130, 160, 110},
    {110, 130, 160, 160, 130, 110},
    {50, 100, 100, 100, 100, 50}
};

int red_evalue[6][6] = {
    {50, 100, 100, 100, 100, 50},
    {110, 130, 160, 160, 130, 110},
    {110, 160, 130, 130, 160, 110},
    {100, 150, 120, 120, 150, 100},
    {100, 120, 150, 150, 120, 100},
    {50, 100, 100, 100, 100, 50}
};

const int chess_evalue = 300;

int GetEvalue(const ChessBoard &board, ChessType chess) {
    int red_pos = 0, black_pos = 0, red_count = 0, black_count = 0;
    for (int y = 0; y < 6; ++y) {
        for (int x = 0; x < 6; ++x) {
            if (board.chess_status({x, y}) == ChessType::Red) {
                red_pos += red_evalue[y][x];
                ++red_count;
            }
            else if (board.chess_status({x, y}) == ChessType::Black) {
                black_pos += black_evalue[y][x];
                ++black_count;
            }
        }
    }
    int red_final = red_count * chess_evalue + red_pos;
    int black_final = black_count * chess_evalue + black_pos;
    return chess == ChessType::Red ?
            red_final - black_final : black_final - red_final;
}

std::string ConvertActionToString(const Action &action) {
    std::ostringstream oss;
    if (action.is_move) {
        oss << "m " << action.current.first << ' ' << action.current.second;
        oss << ' ';
        if (action.target.first - action.current.first == -1) oss << 'l';
        if (action.target.first - action.current.first == 1) oss << 'r';
        if (action.target.second - action.current.second == -1) oss << 'u';
        if (action.target.second - action.current.second == 1) oss << 'd';
    }
    else {
        oss << "e " << action.current.first << ' ' << action.current.second;
        oss << ' ' << action.target.first << ' ' << action.target.second;
    }
    return oss.str();
}

} // namespace

Action AI::Simulate() {
    auto cur_chess = board_.status() == GameStatus::RedMoving ?
            ChessType::Red : ChessType::Black;
    using ActionEvalue = std::pair<int, Action>;
    struct Compare {
        bool operator()(const ActionEvalue &lhs, const ActionEvalue &rhs) {
            return lhs.first < rhs.first;
        }
    };
    std::priority_queue<ActionEvalue, std::vector<ActionEvalue>, Compare> q;
    for (int y = 0; y < 6; ++y) {
        for (int x = 0; x < 6; ++x) {
            ChessPos cur_pos = {x, y};
            if (board_.chess_status(cur_pos) == cur_chess) {
                auto eat_list = board_.CanEat(cur_pos, cur_chess);
                Action cur_act;
                cur_act.chess = cur_chess;
                for (const auto &p : eat_list) {
                    cur_act.is_move = false;
                    cur_act.current = cur_pos;
                    cur_act.target = p;
                    auto temp_board = board_;
                    temp_board.ApplyAction(cur_act);
                    auto evalue = GetEvalue(temp_board, cur_chess) * 2;
                    q.push({evalue, cur_act});
                }
                for (int i = -1; i < 2; ++i) for (int j = -1; j < 2; ++j) {
                    if (i || j) {
                        cur_act.is_move = true;
                        cur_act.current = cur_pos;
                        cur_act.target.first = cur_pos.first + j;
                        cur_act.target.second = cur_pos.second + i;
                        auto temp_board = board_;
                        auto ret = temp_board.ApplyAction(cur_act);
                        if (ret) {
                            auto evalue = GetEvalue(temp_board, cur_chess);
                            q.push({evalue, cur_act});
                        }
                    }
                }
            }
        }
    }
    return q.top().second;
}

std::string AI::NextStep() {
    if (board_.status() != moving_status_) return "";
    auto action = Simulate();
    board_.ApplyAction(action);
    return ConvertActionToString(action);
}

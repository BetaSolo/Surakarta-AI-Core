#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "chessboard.h"
#include "ai.h"

void PrintBoard(const ChessBoard &board) {
    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
            if (x == 0) {
                if (y == 0) {
                    cout << "  ";
                }
                else {
                    cout << (y - 1) << ' ';
                }
            }
            else if (y == 0) {
                cout << (x - 1) << ' ';
            }
            else {
                auto status = board.chess_status({x - 1, y - 1});
                cout << (status == ChessType::Black ?
                        'B' : status == ChessType::Null ?
                        '-' : 'R') << ' ';
            }
        }
        cout << endl;
    }
    const char *status_str[] = {
        "paused", "red's turn", "black's turn", "red won", "black won"
    };
    cout << "status: " << status_str[static_cast<int>(board.status())];
    cout << endl;
}

void ParseCommand(ChessBoard &board, istream &stream) {
    ChessType cur_chess;
    if (board.status() == GameStatus::RedMoving) {
        cur_chess = ChessType::Red;
    }
    else if (board.status() == GameStatus::BlackMoving) {
        cur_chess = ChessType::Black;
    }
    else {
        return;
    }
    string temp;
    stream >> temp;
    int x, y;
    stream >> x >> y;
    if (temp.front() == 'm') {
        stream >> temp;
        int dir = 0;
        if (temp.find('l') != temp.npos) {
            dir |= static_cast<int>(Direction::Left);
        }
        if (temp.find('r') != temp.npos) {
            dir |= static_cast<int>(Direction::Right);
        }
        if (temp.find('u') != temp.npos) {
            dir |= static_cast<int>(Direction::Up);
        }
        if (temp.find('d') != temp.npos) {
            dir |= static_cast<int>(Direction::Down);
        }
        board.PlayerMove({x, y}, static_cast<Direction>(dir), cur_chess);
    }
    else if (temp.front() == 'e') {
        ChessPos cur_pos = {x, y};
        stream >> x >> y;
        board.PlayerEat(cur_pos, {x, y}, cur_chess);
    }
    else if (temp.front() == 'l') {
        auto pos_list = board.CanEat({x, y}, cur_chess);
        if (pos_list.empty()) {
            cout << "can not eat any chess" << endl;
        }
        else {
            for (const auto &pos : pos_list) {
                cout << '(' << pos.first << ", " << pos.second << ") ";
            }
            cout << endl;
        }
    }
    else {
        cout << "invalid command" << endl;
    }
}

int main(int argc, const char *argv[]) {
    ChessBoard board;
    AI ai(board, ChessType::Black);
    board.NewGame();
    while (!cin.eof()) {
        // print the chess board
        PrintBoard(board);
        cout << endl;
        if (board.status() == GameStatus::RedMoving) {
            // apply command
            if (argc >= 2) {
                char temp;
                cin.get(temp);
                ifstream ifs(argv[1]);
                ParseCommand(board, ifs);
            }
            else {
                ParseCommand(board, cin);
            }
        }
        else if (board.status() == GameStatus::BlackMoving) {
            // AI move
            auto step = ai.NextStep();
            cout << step << endl;
            if (argc >= 2) {
                ofstream ofs(argv[1]);
                ofs << step;
            }
        }
        else {
            return 0;
        }
        cout << endl;
    }
    return 0;
}

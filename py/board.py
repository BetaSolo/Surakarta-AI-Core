from status import Chess, GameStatus, Direction


class Action(object):
    def __init__(self, x, y, eat_pos=None, direction=None):
        self.x = x
        self.y = y
        if direction is not None:
            self.is_move = True
            self.direction = direction
            self.to_x = None
            self.to_y = None
        else:
            self.is_move = False
            self.direction = None
            self.to_x = eat_pos[0]
            self.to_y = eat_pos[1]

    def __str__(self):
        if self.direction is not None:
            s = 'm %d %d ' % (self.x, self.y)
            dir_dict = {
                Direction.Up: 'u',
                Direction.Down: 'd',
                Direction.Left: 'l',
                Direction.Right: 'r',
                Direction.LeftUp: 'lu',
                Direction.LeftDown: 'ld',
                Direction.RightUp: 'ru',
                Direction.RightDown: 'rd'
            }
            s += dir_dict[self.direction]
        else:
            s = 'e %d %d %d %d' % (self.x, self.y, self.to_x, self.to_y)
        return s

    def __eq__(self, obj):
        if obj is None:
            return False
        else:
            return self.x == obj.x and self.y == obj.y \
                and self.is_move == obj.is_move \
                and self.direction == obj.direction \
                and self.to_x == obj.to_x and self.to_y == obj.to_y


class Board(object):
    '''
    A class of chess board for Surakarta game.
    '''

    def __init__(self):
        self.__board = [([Chess.Null] * 6) for i in range(6)]
        self.__status = GameStatus.RedMoving
        self.new_game()

    def __str__(self):
        s = ''
        for i in self.__board:
            for j in i:
                if j == Chess.Null:
                    s += '- '
                elif j == Chess.Red:
                    s += 'R '
                else:
                    s += 'B '
            s += '\n'
        return s.rstrip('\n')

    def __check_movable(self, x, y):
        '''
        Check if chess on (x, y) can move.
        '''
        chess = self.get_chess(x, y)
        if chess == Chess.Null:
            return False
        if self.__status in [GameStatus.RedWon, GameStatus.BlackWon]:
            return False
        if chess == Chess.Black and self.__status == GameStatus.RedMoving:
            return False
        if chess == Chess.Red and self.__status == GameStatus.BlackMoving:
            return False
        return True

    def __get_eat_pos(self, x, y, direction, chess, arc_count):
        '''
        Get the position of chess that can be eaten by chess on (x, y).
        '''
        if chess == Chess.Null:
            return None, None
        if (x, y) in [(0, 0), (0, 5), (5, 0), (5, 5)]:
            return None, None
        success, x, y = self.__get_target_pos(x, y, direction)
        if not success:
            pos_list = [
                (1, -1), (2, -1), (2, 6), (1, 6),
                (4, -1), (3, -1), (3, 6), (4, 6),
                (-1, 1), (-1, 2), (6, 2), (6, 1),
                (-1, 4), (-1, 3), (6, 3), (6, 4)
            ]
            x_dir = Direction.Down if y <= 2 else Direction.Up
            y_dir = Direction.Right if x <= 2 else Direction.Left
            if x == -1:
                return self.__get_eat_pos(pos_list[y - 1][0],
                                          pos_list[y - 1][1], x_dir, chess, arc_count + 1)
            elif x == 6:
                return self.__get_eat_pos(pos_list[y + 3][0],
                                          pos_list[y + 3][1], x_dir, chess, arc_count + 1)
            elif y == -1:
                return self.__get_eat_pos(pos_list[x + 7][0],
                                          pos_list[x + 7][1], y_dir, chess, arc_count + 1)
            else:  # y == 6
                return self.__get_eat_pos(pos_list[x + 11][0],
                                          pos_list[x + 11][1], y_dir, chess, arc_count + 1)
        else:
            new_chess = self.get_chess(x, y)
            if new_chess == chess:
                return None, None
            elif new_chess == Chess.Null:
                return self.__get_eat_pos(x, y, direction, chess, arc_count)
            else:
                return (x, y) if arc_count else (None, None)

    def __update_status(self):
        '''
        Update the status of current game.
        '''
        red, black = 0, 0
        for i in self.__board:
            for j in i:
                if j == Chess.Red:
                    red += 1
                elif j == Chess.Black:
                    black += 1
        if red == 0:
            self.__status = GameStatus.BlackWon
        elif black == 0:
            self.__status = GameStatus.RedWon
        elif self.__status == GameStatus.RedMoving:
            self.__status = GameStatus.BlackMoving
        elif self.__status == GameStatus.BlackMoving:
            self.__status = GameStatus.RedMoving

    @staticmethod
    def __get_target_pos(x, y, direction):
        '''
        Get the target position of giving position move along the direction.
        '''
        if direction & Direction.Up:
            y -= 1
        elif direction & Direction.Down:
            y += 1
        if direction & Direction.Left:
            x -= 1
        elif direction & Direction.Right:
            x += 1
        success = x in range(6) and y in range(6)
        return success, x, y

    @property
    def status(self):
        '''
        Return the status of current game.
        '''
        return self.__status

    @property
    def won(self):
        '''
        Return whether the red or black has already won.
        '''
        return self.__status == GameStatus.RedWon \
            or self.__status == GameStatus.BlackWon

    @property
    def board_size(self):
        '''
        Return the size of board.
        '''
        return len(self.__board)

    def new_game(self):
        '''
        Reset the whole board and start a new game.
        '''
        for i in range(6):
            if i < 2:
                for j in range(6):
                    self.__board[i][j] = Chess.Black
            elif i < 4:
                for j in range(6):
                    self.__board[i][j] = Chess.Null
            else:
                for j in range(6):
                    self.__board[i][j] = Chess.Red
        self.__status = GameStatus.RedMoving

    def get_chess(self, x, y):
        '''
        Get the status of specific chess on board.
        '''
        if x not in range(6) or y not in range(6):
            return Chess.Null
        return self.__board[y][x]

    def can_move(self, x, y, direction):
        '''
        Check if chess on (x, y) can move with giving direction.
        '''
        if not self.__check_movable(x, y):
            return False
        success, x, y = self.__get_target_pos(x, y, direction)
        if not success:
            return False
        if self.get_chess(x, y) != Chess.Null:
            return False
        return True

    def get_can_move(self, x, y):
        '''
        Get the list of direction that chess on (x, y) can move along.
        '''
        dir_list = []
        for i in Direction:
            if self.can_move(x, y, i):
                dir_list.append(i)
        return dir_list

    def get_can_eat(self, x, y):
        '''
        Get the list of chess pieces that chess on (x, y) can eat.
        '''
        if not self.__check_movable(x, y):
            return []
        chess_list = []
        chess = self.get_chess(x, y)
        left = self.__get_eat_pos(x, y, Direction.Left, chess, 0)
        right = self.__get_eat_pos(x, y, Direction.Right, chess, 0)
        up = self.__get_eat_pos(x, y, Direction.Up, chess, 0)
        down = self.__get_eat_pos(x, y, Direction.Down, chess, 0)
        if left[0] is not None:
            chess_list.append(left)
        if right[0] is not None:
            chess_list.append(right)
        if up[0] is not None:
            chess_list.append(up)
        if down[0] is not None:
            chess_list.append(down)
        return chess_list

    def player_move(self, x, y, direction):
        '''
        Let chess on (x, y) move along the direction.
        '''
        if not self.__check_movable(x, y):
            return False
        success, nx, ny = self.__get_target_pos(x, y, direction)
        if not success:
            return False
        if self.get_chess(nx, ny) != Chess.Null:
            return False
        self.__board[ny][nx] = self.__board[y][x]
        self.__board[y][x] = Chess.Null
        self.__update_status()
        return True

    def player_eat(self, x, y, eat_x, eat_y):
        chess_list = self.get_can_eat(x, y)
        if (eat_x, eat_y) not in chess_list:
            return False
        chess = self.get_chess(x, y)
        self.__board[eat_y][eat_x] = chess
        self.__board[y][x] = Chess.Null
        self.__update_status()
        return True

    def apply_action(self, action):
        '''
        Apply an action to board.
        '''
        if action.is_move:
            return self.player_move(action.x, action.y, action.direction)
        else:
            return self.player_eat(action.x, action.y,
                                   action.to_x, action.to_y)


# some test
if __name__ == '__main__':
    board = Board()
    print('current board')
    print(board)
    print('current status:', board.status)

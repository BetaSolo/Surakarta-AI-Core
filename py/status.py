from enum import IntEnum, unique


@unique
class Chess(IntEnum):
    Null = 0
    Red = 1
    Black = 2


@unique
class GameStatus(IntEnum):
    # Paused = 0
    RedMoving = 1
    BlackMoving = 2
    RedWon = 3
    BlackWon = 4


@unique
class Direction(IntEnum):
    Up = 1
    Down = 2
    Left = 4
    Right = 8
    LeftUp = 5
    LeftDown = 6
    RightUp = 9
    RightDown = 10


def get_won_status(moving_status):
    if moving_status == GameStatus.RedMoving:
        return GameStatus.RedWon
    elif moving_status == GameStatus.BlackMoving:
        return GameStatus.BlackWon
    else:
        return None


def get_opposite(status):
    div, mod = (status - 1) // 2, (status - 1) % 2
    return GameStatus(div * 2 + int(not mod) + 1)


def get_chess(status):
    if status == GameStatus.RedMoving:
        return Chess.Red
    elif status == GameStatus.BlackMoving:
        return Chess.Black
    else:
        return None


# some test
if __name__ == '__main__':
    for i in GameStatus:
        print(get_opposite(i))

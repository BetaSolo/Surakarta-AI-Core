import math
import random
import simulator
import status
from sys import float_info
from copy import deepcopy
from board import Board
from status import GameStatus, Chess


class Node(object):
    def __init__(self, status: Board, action=None, parent=None):
        self.__status = status
        self.__action = action
        self.__total_game = 0
        self.__won_game = 0
        self.__parent = parent
        self.__children = []

    @staticmethod
    def __find_best_node(node_list):
        best_rate, best_node = -1, None
        for i in node_list:
            if i.__total_game and i.__won_game / i.__total_game > best_rate:
                best_rate = i.__won_game / i.__total_game
                best_node = i
        return best_node

    @property
    def action(self):
        return self.__action

    def search(self, depth=20, breadth=3):
        target = status.get_won_status(self.__status.status)
        for _ in range(depth):
            node = self
            while node.__children:
                node = node.select()
            if node is self or node.__total_game:
                child = node.expand(breadth)
            else:
                child = node
            if child:
                value = child.playout(target)
                child.update(value)
            else:
                node.update(None)
        return self.__find_best_node(self.__children).__action

    def print_tree(self, tab=0):
        tab_str = '| ' * (tab - 1) + ('+-' if tab else '')
        print(tab_str + 'won/total: %d/%d' %
              (self.__won_game, self.__total_game),
              end=', ')
        print('action:', str(self.__action), end=', ')
        print('status:', str(self.__status.status))
        for i in self.__children:
            i.print_tree(tab + 1)

    def select(self):
        selected = None
        best_value = -1.0
        for i in self.__children:
            uct = i.__won_game / (i.__total_game + float_info.epsilon)
            uct += math.sqrt(math.log(self.__total_game + 1)
                             / (i.__total_game + float_info.epsilon))
            uct += random.random() * float_info.epsilon
            if uct > best_value:
                selected = i
                best_value = uct
        return selected

    def expand(self, breadth=3):
        if self.__status.won:
            return None
        else:
            actions = []
            action = simulator.random_action(self.__status)
            for _ in range(breadth):
                try_count = 0
                while action in actions:
                    action = simulator.random_action(self.__status)
                    try_count += 1
                    if try_count > breadth * 2:
                        return random.choice(self.__children)
                next_status = deepcopy(self.__status)
                next_status.apply_action(action)
                actions.append(action)
                self.__children.append(Node(next_status, action, self))
            return random.choice(self.__children)

    def playout(self, target_status):
        return simulator.simulate(self.__status, target_status)

    def update(self, value):
        if value is None:
            value = 1
        node = self
        while node:
            node.__total_game += 1
            node.__won_game += value
            node = node.__parent


# some test
if __name__ == '__main__':
    board = Board()
    while True:
        node = Node(board)
        depth = 20 if board.status == GameStatus.RedMoving else 10
        board.apply_action(node.search(depth=depth, breadth=int(depth/4)))
        print(board)
        # node.print_tree()
        try:
            input()
        except KeyboardInterrupt:
            break
        except EOFError:
            break

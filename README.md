# Surakarta AI Core

Some implementations of AI for playing Surakarta chess.

## Directories

* `cpp`: A C++ implementation using evaluation function and greedy strategy.
* `py`: A Python implementation using MCTS.

## How To Use

### C++ Version

Before using, we should compile source files under directory `cpp` to executable first:

```
$ mkdir cpp/build
$ g++ cpp/ai.cpp cpp/chessboard.cpp cpp/main.cpp -o cpp/build/ai -std=c++14
```

Then we can run `cpp/build/ai` directly, program will print current chessboard in console and wait for user input:

```
  0 1 2 3 4 5
0 B B B B B B
1 B B B B B B
2 - - - - - -
3 - - - - - -
4 R R R R R R
5 R R R R R R
status: red's turn

```

The following command can be accepted:

* `m x y direction`: move the chess at (x, y) in the specified direction, direction can be `l`, `r`, `u`, `d` or all possible combinations of them.
* `e x1 y1 x2 y2`: eat the chess at (x2, y2) using chess at (x1, y1)
* `l x y`: display all the chesses that the specified chess can eat.

### Python Version

Run python version AI using the following command line:

```
$ python3 py/ui.py
```

Also we can see the help information using:

```
$ python3 py/ui.py -h
```

The Python version of AI works just like the C++ version, we can use commands in the same format to play games with AI. For more detailed parameter settings, please refer to the help information.

### AI vs. AI

C++ version AI can read user input from text file, and output AI's action to the file:

```
$ cpp/build/ai input.txt
```

Python version can do the same thing:

```
$ python3 py/ui.py -o input.txt
```

## Copyright and License

Copyright (C) 2010-2018 MaxXing. License MIT.

#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>

#pragma comment (lib, "gdiplus.lib")
#define WM_AI_DONE (WM_USER + 1)

using namespace Gdiplus;
using namespace std;

enum class Side { NONE, RED, BLACK };
enum class Type { NONE, KING, GUARD, BISHOP, KNIGHT, ROOK, CANNON, PAWN };
enum class GameState { MENU, PLAYING, GAMEOVER };

struct Piece {
    Type type = Type::NONE;
    Side side = Side::NONE;
};

class ChessModel {
public:
    Piece board[10][9];
    Side turn = Side::RED;
    bool isGameOver = false;
    Side winner = Side::NONE;
    int aiDepth = 3;
    bool isVsAI = true;
    atomic<bool> isAiThinking{ false };

    void reset();
    bool canMove(int sx, int sy, int ex, int ey, bool checkCheck = true);
    void performMove(int sx, int sy, int ex, int ey);
    void aiThreadFunc(HWND hwnd);
    bool isCheck(Side side);
    bool isCheckmate(Side side);

private:
    int evaluate();
    int minimax(int depth, bool isMax, int alpha, int beta);
};

class ChessView {
public:
    void render(HDC hdc, ChessModel& model, GameState state, int selX, int selY, bool blinkState);
private:
    void drawBoard(Graphics& g, ChessModel& model, int selX, int selY, bool blinkState);
    void drawMenu(Graphics& g);
    void drawGameOver(Graphics& g, ChessModel& model);
};
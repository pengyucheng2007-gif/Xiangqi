#include "ChessApp.h"

void ChessModel::reset() {
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++) board[y][x] = { Type::NONE, Side::NONE };
    Type lay[] = { Type::ROOK, Type::KNIGHT, Type::BISHOP, Type::GUARD, Type::KING, Type::GUARD, Type::BISHOP, Type::KNIGHT, Type::ROOK };
    for (int i = 0; i < 9; i++) { board[0][i] = { lay[i], Side::BLACK }; board[9][i] = { lay[i], Side::RED }; }
    board[2][1] = board[2][7] = { Type::CANNON, Side::BLACK }; board[7][1] = board[7][7] = { Type::CANNON, Side::RED };
    for (int i = 0; i < 9; i += 2) { board[3][i] = { Type::PAWN, Side::BLACK }; board[6][i] = { Type::PAWN, Side::RED }; }
    turn = Side::RED; isGameOver = false; winner = Side::NONE; isAiThinking = false;
}

bool ChessModel::canMove(int sx, int sy, int ex, int ey, bool checkCheck) {
    if (ex < 0 || ex > 8 || ey < 0 || ey > 9) return false;
    Piece sP = board[sy][sx], eP = board[ey][ex];
    if (sP.side == eP.side) return false;
    int dx = abs(ex - sx), dy = abs(ey - sy);
    bool ok = false;

    switch (sP.type) {
    case Type::KING: ok = (dx + dy == 1) && (ex >= 3 && ex <= 5) && (sP.side == Side::RED ? ey >= 7 : ey <= 2); break;
    case Type::GUARD: ok = (dx == 1 && dy == 1) && (ex >= 3 && ex <= 5) && (sP.side == Side::RED ? ey >= 7 : ey <= 2); break;
    case Type::BISHOP:
        if (dx == 2 && dy == 2 && (sP.side == Side::RED ? ey >= 5 : ey <= 4))
            if (board[(sy + ey) / 2][(sx + ex) / 2].type == Type::NONE) ok = true; // 塞象眼
        break;
    case Type::KNIGHT:
        if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1)) { // 蹩馬腿
            if (dy == 2) ok = (board[sy + (ey > sy ? 1 : -1)][sx].type == Type::NONE);
            else ok = (board[sy][sx + (ex > sx ? 1 : -1)].type == Type::NONE);
        } break;
    case Type::ROOK:
    case Type::CANNON: {
        if (dx != 0 && dy != 0) break;
        int cnt = 0;
        if (dx == 0) { for (int i = min(sy, ey) + 1; i < max(sy, ey); ++i) if (board[i][sx].type != Type::NONE) cnt++; }
        else { for (int i = min(sx, ex) + 1; i < max(sx, ex); ++i) if (board[sy][i].type != Type::NONE) cnt++; }
        ok = (sP.type == Type::ROOK) ? (cnt == 0) : (eP.type == Type::NONE ? cnt == 0 : cnt == 1);
    } break;
    case Type::PAWN:
        if (sP.side == Side::RED) ok = (ey < sy || (sy <= 4 && dx > 0)) && (dx + dy == 1);
        else ok = (ey > sy || (sy >= 5 && dx > 0)) && (dx + dy == 1);
        break;
    }

    if (ok && checkCheck) {
        Piece tmp = board[ey][ex]; board[ey][ex] = board[sy][sx]; board[sy][sx] = { Type::NONE, Side::NONE };
        bool safe = !isCheck(sP.side); // 自殺步判定
        // 王不見王判定
        int rx = -1, ry = -1, bx = -1, by = -1;
        for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++) {
            if (board[y][x].type == Type::KING) { if (board[y][x].side == Side::RED) { rx = x; ry = y; } else { bx = x; by = y; } }
        }
        if (rx == bx && rx != -1) {
            bool blocked = false;
            for (int i = min(ry, by) + 1; i < max(ry, by); i++) if (board[i][rx].type != Type::NONE) blocked = true;
            if (!blocked) safe = false;
        }
        board[sy][sx] = board[ey][ex]; board[ey][ex] = tmp;
        return safe;
    }
    return ok;
}

bool ChessModel::isCheck(Side side) {
    int kx = -1, ky = -1;
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++)
        if (board[y][x].type == Type::KING && board[y][x].side == side) { kx = x; ky = y; break; }
    if (kx == -1) return false;
    Side enemy = (side == Side::RED ? Side::BLACK : Side::RED);
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++)
        if (board[y][x].side == enemy && canMove(x, y, kx, ky, false)) return true;
    return false;
}

bool ChessModel::isCheckmate(Side side) {
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++)
        if (board[y][x].side == side)
            for (int ey = 0; ey < 10; ey++) for (int ex = 0; ex < 9; ex++)
                if (canMove(x, y, ex, ey, true)) return false;
    return true; // 欠行者負 (困斃) 亦在此邏輯中
}

void ChessModel::performMove(int sx, int sy, int ex, int ey) {
    board[ey][ex] = board[sy][sx]; board[sy][sx] = { Type::NONE, Side::NONE };
    turn = (turn == Side::RED) ? Side::BLACK : Side::RED;
    if (isCheckmate(turn)) { isGameOver = true; winner = (turn == Side::RED ? Side::BLACK : Side::RED); }
}

int ChessModel::evaluate() {
    int s = 0;
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++) {
        if (board[y][x].side == Side::NONE) continue;
        int v = 0;
        switch (board[y][x].type) {
        case Type::KING: v = 10000; break; case Type::ROOK: v = 900; break;
        case Type::CANNON: v = 450; break; case Type::KNIGHT: v = 400; break;
        case Type::BISHOP: v = 200; break; case Type::GUARD: v = 200; break;
        case Type::PAWN: v = 100; break;
        }
        s += (board[y][x].side == Side::BLACK) ? v : -v;
    }
    return s;
}

int ChessModel::minimax(int d, bool isMax, int a, int b) {
    if (d <= 0 || isGameOver) return evaluate();
    if (isMax) {
        int res = -999999;
        for (int sy = 0; sy < 10; sy++) for (int sx = 0; sx < 9; sx++) {
            if (board[sy][sx].side == Side::BLACK)
                for (int ey = 0; ey < 10; ey++) for (int ex = 0; ex < 9; ex++)
                    if (canMove(sx, sy, ex, ey, true)) {
                        Piece tmp = board[ey][ex]; board[ey][ex] = board[sy][sx]; board[sy][sx] = { Type::NONE, Side::NONE };
                        res = (max)(res, minimax(d - 1, false, a, b));
                        board[sy][sx] = board[ey][ex]; board[ey][ex] = tmp;
                        a = (max)(a, res); if (b <= a) return res;
                    }
        } return res;
    }
    else {
        int res = 999999;
        for (int sy = 0; sy < 10; sy++) for (int sx = 0; sx < 9; sx++) {
            if (board[sy][sx].side == Side::RED)
                for (int ey = 0; ey < 10; ey++) for (int ex = 0; ex < 9; ex++)
                    if (canMove(sx, sy, ex, ey, true)) {
                        Piece tmp = board[ey][ex]; board[ey][ex] = board[sy][sx]; board[sy][sx] = { Type::NONE, Side::NONE };
                        res = (min)(res, minimax(d - 1, true, a, b));
                        board[sy][sx] = board[ey][ex]; board[ey][ex] = tmp;
                        b = (min)(b, res); if (b <= a) return res;
                    }
        } return res;
    }
}

void ChessModel::aiThreadFunc(HWND hwnd) {
    isAiThinking = true;
    static mt19937 rng(random_device{}());
    this_thread::sleep_for(chrono::milliseconds(800));
    int bs = -999999, bx = -1, by = -1, bex = -1, bey = -1;
    for (int sy = 0; sy < 10; sy++) for (int sx = 0; sx < 9; sx++) {
        if (board[sy][sx].side == Side::BLACK)
            for (int ey = 0; ey < 10; ey++) for (int ex = 0; ex < 9; ex++)
                if (canMove(sx, sy, ex, ey, true)) {
                    Piece tmp = board[ey][ex]; board[ey][ex] = board[sy][sx]; board[sy][sx] = { Type::NONE, Side::NONE };
                    int s = minimax(aiDepth, false, -999999, 999999);
                    board[sy][sx] = board[ey][ex]; board[ey][ex] = tmp;
                    if (s > bs) { bs = s; bx = sx; by = sy; bex = ex; bey = ey; }
                }
    }
    if (bx != -1) performMove(bx, by, bex, bey);
    isAiThinking = false;
    PostMessage(hwnd, WM_AI_DONE, 0, 0);
}
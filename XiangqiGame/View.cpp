#include "ChessApp.h"

void ChessView::render(HDC hdc, ChessModel& model, GameState state, int selX, int selY, bool blinkState) {
    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetTextRenderingHint(TextRenderingHintAntiAlias);
    if (state == GameState::MENU) drawMenu(g);
    else if (state == GameState::GAMEOVER) drawGameOver(g, model);
    else drawBoard(g, model, selX, selY, blinkState);
}

void ChessView::drawBoard(Graphics& g, ChessModel& model, int selX, int selY, bool blinkState) {
    RectF bgR(0, 0, 600, 850); LinearGradientBrush wood(bgR, Color(255, 245, 225, 180), Color(255, 210, 170, 120), LinearGradientModeForwardDiagonal);
    g.FillRectangle(&wood, bgR);
    REAL G = 60.0f, OX = 60.0f, OY = 135.0f;
    StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
    Gdiplus::Font sF(L"Arial", 24, FontStyleBold, UnitPixel);
    SolidBrush sBr(model.turn == Side::RED ? Color::DarkRed : Color::Black);
    g.DrawString(model.isAiThinking ? L"AI Thinking..." : (model.turn == Side::RED ? L"RED TURN" : L"BLACK TURN"), -1, &sF, RectF(0, 40, 600, 60), &sf, &sBr);
    Pen gridP(Color(180, 0, 0, 0), 1.5f);
    for (int i = 0; i < 10; i++) g.DrawLine(&gridP, OX, OY + i * G, OX + 8 * G, OY + i * G);
    for (int i = 0; i < 9; i++) { g.DrawLine(&gridP, OX + i * G, OY, OX + i * G, OY + 4 * G); g.DrawLine(&gridP, OX + i * G, OY + 5 * G, OX + i * G, OY + 9 * G); }
    Gdiplus::Font pF(L"楷體", 34, FontStyleBold, UnitPixel);
    for (int y = 0; y < 10; y++) for (int x = 0; x < 9; x++) {
        if (model.board[y][x].side == Side::NONE) continue;
        REAL px = OX + x * G, py = OY + y * G; RectF pR(px - 27, py - 27, 54, 54);
        if (x == selX && y == selY) { Pen p(Color::Gold, 4); g.DrawEllipse(&p, RectF(px - 32, py - 32, 64, 64)); if (!blinkState) continue; }
        SolidBrush pBg(Color::FloralWhite), tBr(model.board[y][x].side == Side::RED ? Color::DarkRed : Color::Black);
        g.FillEllipse(&pBg, pR); g.DrawEllipse(&gridP, pR);
        wstring t; switch (model.board[y][x].type) {
        case Type::KING: t = (model.board[y][x].side == Side::RED ? L"帥" : L"將"); break;
        case Type::ROOK: t = L"車"; break; case Type::KNIGHT: t = L"馬"; break;
        case Type::CANNON: t = (model.board[y][x].side == Side::RED ? L"炮" : L"砲"); break;
        case Type::BISHOP: t = (model.board[y][x].side == Side::RED ? L"相" : L"象"); break;
        case Type::GUARD: t = (model.board[y][x].side == Side::RED ? L"仕" : L"士"); break;
        case Type::PAWN: t = (model.board[y][x].side == Side::RED ? L"兵" : L"卒"); break;
        } g.DrawString(t.c_str(), -1, &pF, pR, &sf, &tBr);
    }
}

void ChessView::drawMenu(Graphics& g) {
    SolidBrush b(Color(255, 30, 30, 30)), gBr(Color::Gold); g.FillRectangle(&b, 0, 0, 600, 850);
    Gdiplus::Font tF(L"Arial Black", 42, FontStyleBold, UnitPixel), bF(L"Arial", 26, FontStyleBold, UnitPixel);
    StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(L"XIANGQI MASTER", -1, &tF, RectF(0, 180, 600, 100), &sf, &gBr);
    Pen p(Color::Gold, 2); RectF r1(150, 380, 300, 70), r2(150, 490, 300, 70);
    g.DrawRectangle(&p, r1); g.DrawString(L"PLAYER VS PLAYER", -1, &bF, r1, &sf, &gBr);
    g.DrawRectangle(&p, r2); g.DrawString(L"PLAYER VS AI", -1, &bF, r2, &sf, &gBr);
}

void ChessView::drawGameOver(Graphics& g, ChessModel& model) {
    SolidBrush m(Color(200, 0, 0, 0)), gBr(Color::Gold); g.FillRectangle(&m, 0, 0, 600, 850);
    Gdiplus::Font f(L"Arial Black", 50, FontStyleBold, UnitPixel), sfF(L"Arial", 22, FontStyleRegular, UnitPixel);
    StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
    wstring w = (model.winner == Side::RED ? L"RED WINS!" : L"BLACK WINS!");
    g.DrawString(w.c_str(), -1, &f, RectF(0, 300, 600, 150), &sf, &gBr);
    g.DrawString(L"CLICK ANYWHERE TO RESTART", -1, &sfF, RectF(0, 450, 600, 100), &sf, &gBr);
}
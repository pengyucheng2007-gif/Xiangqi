#include "ChessApp.h"

ChessModel m; ChessView v; GameState st = GameState::MENU;
int sX = -1, sY = -1; bool blink = true;

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: SetTimer(h, 1, 400, NULL); break;
    case WM_TIMER: blink = !blink; InvalidateRect(h, NULL, FALSE); break;
    case WM_AI_DONE: if (m.isGameOver) st = GameState::GAMEOVER; InvalidateRect(h, NULL, FALSE); break;
    case WM_LBUTTONDOWN: {
        if (m.isAiThinking) break;
        int mx = LOWORD(lp), my = HIWORD(lp);
        if (st == GameState::MENU) {
            if (mx >= 150 && mx <= 450 && my >= 380 && my <= 450) { m.isVsAI = false; st = GameState::PLAYING; m.reset(); }
            else if (mx >= 150 && mx <= 450 && my >= 490 && my <= 560) { m.isVsAI = true; st = GameState::PLAYING; m.reset(); }
        }
        else if (st == GameState::GAMEOVER) { st = GameState::MENU; }
        else {
            int x = (mx - 60 + 30) / 60, y = (my - 135 + 30) / 60;
            if (x >= 0 && x < 9 && y >= 0 && y < 10) {
                if (sX == -1 && m.board[y][x].side == m.turn) { sX = x; sY = y; }
                else if (sX != -1) {
                    if (m.board[y][x].side == m.turn) { sX = x; sY = y; }
                    else if (m.canMove(sX, sY, x, y)) {
                        m.performMove(sX, sY, x, y); sX = sY = -1;
                        if (m.isGameOver) st = GameState::GAMEOVER;
                        else if (m.isVsAI && m.turn == Side::BLACK) thread(&ChessModel::aiThreadFunc, &m, h).detach();
                    }
                }
            }
        }
        InvalidateRect(h, NULL, FALSE);
    } break;
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(h, &ps);
        HDC mdc = CreateCompatibleDC(hdc); HBITMAP mbm = CreateCompatibleBitmap(hdc, 600, 850);
        SelectObject(mdc, mbm); v.render(mdc, m, st, sX, sY, blink);
        BitBlt(hdc, 0, 0, 600, 850, mdc, 0, 0, SRCCOPY);
        DeleteObject(mbm); DeleteDC(mdc); EndPaint(h, &ps);
    } break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(h, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR c, int s) {
    ULONG_PTR tk; GdiplusStartupInput si; GdiplusStartup(&tk, &si, NULL);
    WNDCLASSW wc = { 0 }; wc.lpfnWndProc = WndProc; wc.hInstance = hi;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.lpszClassName = L"ChessApp";
    RegisterClassW(&wc);
    HWND hw = CreateWindowW(L"ChessApp", L"Xiangqi Master", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 100, 100, 620, 900, 0, 0, hi, 0);
    ShowWindow(hw, s);
    MSG msg; while (GetMessage(&msg, 0, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(tk); return 0;
}
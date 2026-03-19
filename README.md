# ♟️ 象棋大師 (Xiangqi Master) - C++ 核心開發專案

這是一個基於 **C++** 與 **Windows API (GDI+)** 開發的完整象棋遊戲系統。本專案嚴格遵循 **MVC (Model-View-Controller)** 軟體架構，確保資料邏輯與視覺渲染徹底解耦。同時，內部實作了基於 **Minimax 演算法** 與 **Alpha-Beta 剪枝** 的人工智慧對戰引擎。

---

## 🏗️ 1. 系統架構與檔案關聯 (System Architecture)

本專案由「一個標頭檔」與「三個核心模組」組成。標頭檔定義了世界觀與通訊語言，而三個模組則各司其職：

### 📄 通訊藍圖：`Chess.h` (或你的標頭檔名稱)
所有的 `.cpp` 檔案都必須 `#include` 這個標頭檔。它定義了整個遊戲的資料結構與全域常數。
* **枚舉定義 (Enums)**：
  * `enum Side { NONE, RED, BLACK }`：定義棋子與當前回合的陣營。
  * `enum PieceType { EMPTY, KING, GUARD, BISHOP, KNIGHT, ROOK, CANNON, PAWN }`：定義棋子的種類。
* **結構體 (Structs)**：
  * `struct Point { int x, y; }`：用於封裝棋盤上的二維網格座標。
  * `struct Move { Point from, to; int score; }`：用於 AI 運算時紀錄每一步走法與預期分數。
* **常數 (Constants)**：
  * `const int GRID_SIZE = 60;`：定義棋盤每一格的像素大小，用於座標轉換。
  * `const int WM_AI_DONE = WM_USER + 1;`：自定義的 Windows 訊息，用於多執行緒通訊。

### ⚙️ 邏輯核心：`Model.cpp` (數據與規則局)
城市的「真理來源」，完全不涉及任何繪圖相關的程式碼。
* **核心職責**：維護棋盤二維陣列、實作所有物理規則、執行 AI 深度推演。

### 🎨 視覺渲染：`View.cpp` (視覺宣傳部)
城市的「畫家」，屬於唯讀狀態，只能讀取 `Model` 的資料來畫圖，不能修改資料。
* **核心職責**：GDI+ 繪圖環境初始化、雙緩衝 (Double Buffering) 防閃爍處理、貼圖與 UI 繪製。

### 🕹️ 系統控制：`Main.cpp` (行政中心 Controller)
城市的「大腦」，負責接收外界刺激並指派任務。
* **核心職責**：接收滑鼠/鍵盤事件、將螢幕像素座標轉換為棋盤網格座標、管理背景執行緒。

---

## 📜 2. 核心變數與資料字典 (Data Dictionary)

在 `Model` 中，我們定義了以下關鍵變數來維持遊戲的運作：

| 變數名稱 | 資料型態 | 詳細說明 |
| :--- | :--- | :--- |
| `board[10][9]` | `int` (或自訂 `Piece`) | 核心狀態矩陣。10 列 9 行。正數代表紅方，負數代表黑方，0 代表空位。 |
| `currTurn` | `Side` | 狀態機。紀錄目前輪到 `RED` 還是 `BLACK` 下棋。 |
| `selectedX, selectedY` | `int` | 紀錄玩家目前滑鼠選中的棋子網格座標 $(x, y)$。值為 `-1` 代表當前未選取任何棋子。 |
| `isAiThinking` | `std::atomic<bool>` | 執行緒安全鎖。當 AI 在背景運算時設為 `true`，此時會封鎖玩家的滑鼠輸入，防止資料庫被同時讀寫造成崩潰。 |
| `aiDepth` | `int` | 定義 Minimax 演算法往下遞迴推演的層數（難度設定）。 |

---

## 🧠 3. 核心函式運作邏輯 (Core Functions)

### 【Model 規則與 AI 引擎】
* **`bool canMove(int sx, int sy, int ex, int ey)`**
  * **功能**：嚴格的步法過濾器。
  * **細節**：
    1. 檢查目標位置是否為己方棋子（不可自相殘殺）。
    2. 針對 `KNIGHT` (馬) 檢查 `(sx + ex)/2` 或 `(sy + ey)/2` 是否有棋子（蹩馬腿判定）。
    3. 針對 `BISHOP` (象) 檢查田字中心是否有棋子（塞象眼判定），並限制不可過河。
    4. 針對 `CANNON` (炮) 吃子時，統計起點與終點之間的棋子數量必須精準為 1（炮架判定）。
* **`bool isCheck(Side side)`**
  * **功能**：將軍判定。
  * **細節**：遍歷敵方所有存活的棋子，呼叫 `canMove` 測試其終點是否為己方的 `KING`。若是，則回傳 `true`。
* **`int evaluate()`**
  * **功能**：AI 的價值觀評分系統。
  * **細節**：掃描 `board`，賦予棋子權重（例：車=900, 炮=450, 馬=400）。若兵/卒越過楚河漢界，則額外增加分數。回傳「AI 總分 - 玩家總分」的差值。
* **`int minimax(int depth, int alpha, int beta, bool isMax)`**
  * **功能**：未來推演與決策。
  * **細節**：使用遞迴生成一棵博弈樹。`isMax` 輪到 AI 時尋求最高分，輪到玩家時尋求最低分。引入 `alpha` 與 `beta` 變數，若發現劣勢分支則提早 `break` 迴圈（Alpha-Beta 剪枝），大幅提升運算效能。

### 【View 介面繪製】
* **`void render(HDC hdc)`**
  * **功能**：畫面更新總管。
  * **細節**：建立記憶體 DC (Memory Device Context) 進行雙緩衝繪製。先畫出木紋背景，接著繪製 $9 \times 10$ 的網格線。最後雙層 `for` 迴圈掃描 `board[10][9]`，根據數值從 Sprite Sheet 或資源檔中呼叫 `Graphics::DrawImage` 畫出棋子。

### 【Main 事件處理】
* **`LRESULT CALLBACK WndProc(...)`**
  * **功能**：Windows 訊息處理幫浦 (Message Pump)。
  * **細節**：攔截 `WM_LBUTTONDOWN` 訊息。將拿到的滑鼠像素 `lParam`，代入公式 `col = (x - offset) / GRID_SIZE` 轉換為網格座標。檢查是否選中自己的棋子，或是否點擊了合法目標。
* **`void aiThreadFunc()`**
  * **功能**：多執行緒分離。
  * **細節**：使用 `std::thread` 呼叫此函式，在內部執行耗時的 `minimax`。算完後使用 `PostMessage(hwnd, WM_AI_DONE, 0, 0)` 將結果非同步傳遞回主視窗，避免視窗出現「沒有回應」的當機狀態。

---
# ♟️ Xiangqi Master (象棋大師) - C++ 核心開發專案

這是一個基於 **C++** 與 **Win32 GDI+** 開發的完整象棋遊戲系統。本專案採用專業的 **MVC (Model-View-Controller)** 架構，並實作了高效率的 **Minimax 遞迴演算法** 與 **Alpha-Beta 剪枝** 人工智慧引擎。

---

## 🏗️ 1. 系統架構與檔案關聯 (System Architecture)

專案將功能徹底模組化，三個核心模組透過 `ChessApp.h` 定義的通訊協議進行交互：

### 📂 檔案分工
* **`ChessApp.h` (藍圖定義 - The Blueprint)**
    * **職責**：定義全域枚舉（陣營、棋子類型、遊戲狀態）、`Piece` 結構體，以及 `ChessModel` 與 `ChessView` 的類別介面。
    * **地位**：作為專案的通訊協議，所有 `.cpp` 檔案皆須 `#include` 此檔以確保資料結構一致。
* **`Main.cpp` (行政中心 - Controller)**
    * **職責**：處理 Windows 訊息循環 (`WndProc`)。
    * **邏輯**：攔截滑鼠點擊 `WM_LBUTTONDOWN`，將像素座標轉為網格索引，並指派 `Model` 運算與 `View` 渲染。
    * **執行緒**：負責啟動異步 AI 執行緒 `std::thread`，防止介面在 AI 思考時進入「沒有回應」狀態。
* **`Model.cpp` (數據規則局 - Model)**
    * **職責**：管理 $10 \times 9$ 棋盤狀態矩陣、實作象棋物理規則與 AI 深度決策邏輯。
* **`View.cpp` (視覺渲染部 - View)**
    * **職責**：利用 GDI+ 進行畫面繪製，包含主選單、對戰場景及結束畫面之渲染。

---

## 📜 2. 資料字典 (Data Dictionary)

### 🔑 核心類型定義 (`ChessApp.h`)
| 名稱 | 類型 | 描述 |
| :--- | :--- | :--- |
| **`Side`** | `enum` | 定義陣營：`NONE`, `RED` (紅方), `BLACK` (黑方)。 |
| **`Type`** | `enum` | 定義棋子：`KING`, `GUARD`, `BISHOP`, `KNIGHT`, `ROOK`, `CANNON`, `PAWN`。 |
| **`GameState`** | `enum` | 遊戲階段：`MENU` (主選單), `PLAYING` (遊戲中), `GAMEOVER` (結算)。 |
| **`Piece`** | `struct` | 封裝單個棋子的種類與陣營。 |
| **`WM_AI_DONE`** | `macro` | 自定義訊息 (`WM_USER + 1`)，用於 AI 算完棋後通知主視緒更新畫面。 |

### 💾 關鍵變數 (Variables)
| 變數名稱 | 所屬類別/位置 | 描述 |
| :--- | :--- | :--- |
| **`board[10][9]`** | `ChessModel` | 儲存棋盤狀態的核心二維陣列。 |
| **`turn`** | `ChessModel` | 紀錄目前回合陣營（決定哪一方可移動棋子）。 |
| **`isAiThinking`** | `ChessModel` | 原子布林值 (`atomic<bool>`)，用於執行緒安全鎖，避免玩家與 AI 同時動到數據。 |
| **`st`** | `Main.cpp` | 全域變數，紀錄當前遊戲階段狀態。 |
| **`sX, sY`** | `Main.cpp` | 玩家目前滑鼠選中的棋格座標，若為 `-1` 代表未選取。 |

---

## ⚙️ 3. 核心運算邏輯 (Core Functions)

### ⚖️ `ChessModel` (數據與規則實作)
* **`canMove(sx, sy, ex, ey, checkCheck)`**
    * **物理規則**：實作「馬走日 (含蹩馬腿)」、「象走田 (含塞象眼)」、「炮需炮架吃子」等規則。
    * **自殺步檢測**：若 `checkCheck` 為真，會模擬移動並判定是否導致己方「被將軍」。
    * **王不見王**：判定兩王是否在同一直線上且中間無阻擋棋子。
* **`minimax(depth, isMax, alpha, beta)`**
    * **AI 核心**：遞迴預測未來數步走法。
    * **Alpha-Beta 剪枝**：動態更新上下界限制，提早捨棄無效益的分支以極大化運算效率。
* **`evaluate()`**
    * **盤面估值**：賦予棋子權重（如：王 10000, 車 900, 炮 450），回傳「黑方總分 - 紅方總分」的分差。

### 🎨 `ChessView` (視覺渲染實作)
* **`render()`**：總管函式。依據 `GameState` 切換繪製邏輯。
* **`drawBoard()`**
    * **背景渲染**：使用 `LinearGradientBrush` 繪製木質紋理。
    * **雙緩衝技術**：在 `Main.cpp` 透過 `CreateCompatibleBitmap` 實作，解決畫面閃爍問題。
    * **選取效果**：當 `x==selX && y==selY` 時繪製金色圓環，並搭配 `blinkState` 實現動態閃爍。

### 🕹️ `Main.cpp` (事件與執行緒控制)
* **`WndProc()`**
    * **計時器**：透過 `WM_TIMER` 定期切換 `blink` 狀態。
    * **座標轉換**：利用公式 `(mx - offset + 30) / 60` 將像素座標精準轉換為棋盤格索引。
    * **多執行緒**：玩家移動後，若輪到 AI 回合，呼叫 `thread(...).detach()` 開啟背景運算。

---

## 🤝 4. 團隊協作與版本控管 (Team SOP)

為了確保多人開發順利，請遵循以下規範：
1.  **開工同步 (Pull)**：每天開始開發前，務必執行 `git pull --rebase`。
2.  **變更通報**：若修改 `ChessApp.h` 中的結構定義，必須即時告知負責渲染 (View) 的成員。
3.  **垃圾檔案過濾**：`.gitignore` 必須包含 `.vs/`、`Debug/`、`Release/` 等編譯產生的暫存資料夾。
4.  **強制推送 (慎用)**：若發生合併衝突且確定本地版本正確，可使用 `git push -f` 覆蓋遠端。
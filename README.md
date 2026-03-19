# ♟️ 象棋大師 (Xiangqi Master) - C++ 核心開發專案

這是一個基於 **C++** 與 **Win32 GDI+** 開發的完整象棋遊戲系統。本專案採用專業的 **MVC (Model-View-Controller)** 架構，確保邏輯與表現層分離，並實作了高效率的 AI 決策引擎。

---

## 🏗️ 1. 系統架構與檔案關聯 (System Architecture)

本專案將程式功能拆分為三個核心「城市」，各司其職並透過訊息傳遞進行交互：

### 📂 檔案分工
* **`Main.cpp` (Controller - 行政中心)**
    * **職責**：處理 Windows 視窗訊息循環 (`WndProc`)、滑鼠點擊座標轉換。
    * **交互**：當玩家點擊 $(x, y)$，負責呼叫 `Model` 進行邏輯驗證，並通知 `View` 重繪。
* **`Model.cpp / .h` (Model - 數據與規則局)**
    * **職責**：管理棋盤陣列 `board[10][9]`、實作所有象棋物理規則、執行 AI 運算。
    * **交互**：它是遊戲的「真理來源」，不負責畫面，僅提供邏輯結果（如：這步棋是否合法）。
* **`View.cpp / .h` (View - 視覺宣傳部)**
    * **職責**：負責 GDI+ 繪圖初始化、雙緩衝渲染（防止閃爍）、介面動畫。
    * **交互**：它是「唯讀」的，根據 `Model` 提供的數據繪製出目前的棋局。

---

## 📜 2. 資料字典 (Data Dictionary)

### 🔑 核心常數 (Constants)
| 常數名稱 | 類型 | 功能描述 |
| :--- | :--- | :--- |
| `Side` | `enum` | 定義陣營：`RED` (紅方), `BLACK` (黑方), `NONE` (空)。 |
| `PieceType` | `enum` | 定義 7 種棋子種類（將、士、象、車、馬、炮、卒）。 |
| `WM_AI_DONE` | `uint` | 自定義 Windows 訊息，用於 AI 算完棋後的跨執行緒通知。 |

### 💾 關鍵變數 (Variables)
| 變數名稱 | 所屬檔案 | 功能描述 |
| :--- | :--- | :--- |
| `board[10][9]` | `Model` | 核心棋盤陣列，儲存每個座標點的棋子資訊。 |
| `turn` | `Model` | 紀錄目前輪到哪一方下棋。 |
| `selX, selY` | `Main` | 玩家目前滑鼠選中的棋子網格座標（-1 為未選取）。 |
| `isAiThinking` | `Model` | 原子布林值，防止 UI 執行緒在 AI 運算時改動棋盤數據。 |

### ⚙️ 核心函式 (Functions)

#### 【Model 邏輯類】
* `bool canMove(int sx, int sy, int ex, int ey)`：
    * 核心規則引擎。判斷是否符合特定棋子的步法，並檢查路徑障礙物（如：蹩馬腿、塞象眼）。
* `bool isCheck(Side s)`：
    * 危險檢測。掃描敵方所有棋子是否能攻擊到 `s` 方的王。
* `int evaluate()`：
    * 盤面估值。根據棋子殘餘價值與位置權重（如：過河卒價值提高）回傳分數給 AI。
* `int minimax(int depth, int alpha, int beta, bool isMax)`：
    * 遞迴預測未來走法。使用 **Alpha-Beta 剪枝** 捨棄無效分支，極大化 AI 利益。



#### 【View 渲染類】
* `void render(Graphics& g)`：
    * 主繪圖函式。遍歷 `board` 陣列，將對應的棋子圖片繪製在計算出的像素位置。
* `void drawSelection()`：
    * 在選中棋子的座標處繪製動態選取框。

#### 【Main 控制類】
* `LRESULT CALLBACK WndProc(...)`：
    * 訊息總管。攔截 `WM_LBUTTONDOWN` 並將像素 $(x, y)$ 換算為棋盤索引 $(col, row)$。
* `void threadAI()`：
    * 獨立執行緒。在後台執行 `minimax` 運算，確保 AI 思考時視窗不會顯示「沒有回應」。

---

## 🤝 3. 團隊協作與上傳規範 (Git SOP)
1.  **開工前 Pull**：`git pull --rebase origin main` (確保你拿到隊友最新的 Code)。
2.  **完成後 Push**：`git add .` -> `git commit -m "feat: 描述功能"` -> `git push`。
3.  **避開垃圾檔案**：確保 `.vs/`、`Debug/`、`Release/` 不被上傳，保持 Repository 乾淨。
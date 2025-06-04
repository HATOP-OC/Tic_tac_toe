# 🎮 Хрестики-Нулики з ШІ - Детальна документація

## 📋 Зміст
1. [Огляд проекту](#огляд-проекту)
2. [Архітектура системи](#архітектура-системи)
3. [Класи та їх функції](#класи-та-їх-функції)
4. [Алгоритм штучного інтелекту](#алгоритм-штучного-інтелекту)
5. [Рівні складності ШІ](#рівні-складності-ші)
6. [Система оцінки позицій](#система-оцінки-позицій)
7. [Крос-платформна підтримка](#крос-платформна-підтримка)
8. [Збереження статистики](#збереження-статистики)
9. [Інтерфейс користувача](#інтерфейс-користувача)
10. [Технічні деталі](#технічні-деталі)

---

## 🎯 Огляд проекту

Це консольна гра "Хрестики-Нулики" з продвинутим штучним інтелектом, написана на C++17. Проект демонструє використання алгоритму Minimax з Alpha-Beta відсіченням для створення непереможного ШІ, а також реалізацію об'єктно-орієнтованих принципів програмування.

### 🚀 Ключові особливості:
- **6 рівнів складності ШІ** - від новачка до майстра
- **Змінні розміри поля** - 3x3, 4x4, 5x5
- **Кольоровий інтерфейс** - підтримка ANSI escape кодів
- **Крос-платформність** - Linux і Windows
- **Збереження статистики** - автоматичне ведення рекордів
- **Різні режими гри** - людина проти людини, людина проти ШІ, ШІ проти ШІ

---

## 🏗️ Архітектура системи

Проект побудований за принципами ООП з використанням патернів проектування:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   GameSetup     │    │  GameManager    │    │   GameStats     │
│   (Controller)  │────│   (Controller)  │────│    (Model)      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                              │
                              │
                    ┌─────────────────┐
                    │   GameField     │
                    │    (Model)      │
                    └─────────────────┘
                              │
                              │
                    ┌─────────────────┐
                    │     Player      │
                    │  (Base Class)   │
                    └─────────────────┘
                              │
                    ┌─────────┴─────────┐
                    │                   │
            ┌───────────────┐   ┌──────────────┐
            │ HumanPlayer   │   │   AIPlayer   │
            │  (Derived)    │   │  (Derived)   │
            └───────────────┘   └──────────────┘
```

---

## 📦 Класи та їх функції

### 🎨 `ConsoleColor`
Утилітарний клас для роботи з кольоровим виводом у консолі.

```cpp
class ConsoleColor {
public:
    enum class Color {
        RESET = 0, BLACK = 30, RED = 31, GREEN = 32,
        YELLOW = 33, BLUE = 34, MAGENTA = 35, CYAN = 36, WHITE = 37
    };
    
    static std::string colorText(const std::string& text, Color color);
    static std::string bold(const std::string& text);
    static std::string colorChar(char c, Color color);
    static void clearScreen();
};
```

**Функції:**
- `colorText()` - забарвлює текст у вказаний колір
- `bold()` - робить текст жирним
- `colorChar()` - забарвлює один символ
- `clearScreen()` - очищує екран (кросплатформно)

### 🎲 `GameField`
Представляє ігрове поле та логіку гри.

```cpp
class GameField {
private:
    std::vector<std::vector<char>> cells;    // Масив клітинок
    int size;                                // Розмір поля
    std::map<char, ConsoleColor::Color> symbolColors; // Кольори символів

public:
    explicit GameField(int size = 3);
    GameField(const GameField& other);              // Копіювальний конструктор
    GameField& operator=(const GameField& other);   // Оператор присвоєння
    
    void reset();                                   // Очищення поля
    bool placeSymbol(int row, int col, char symbol); // Розміщення символу
    bool checkWin(char symbol) const;               // Перевірка виграшу
    bool isFull() const;                           // Перевірка заповненості
    char getCell(int row, int col) const;          // Отримання символу клітинки
    int getSize() const;                           // Розмір поля
    std::vector<std::pair<int, int>> getAvailableMoves() const; // Доступні ходи
    
    void setSymbolColor(char symbol, ConsoleColor::Color color);
    ConsoleColor::Color getSymbolColor(char symbol) const;
};
```

**Алгоритм перевірки виграшу:**
```cpp
bool checkWin(char symbol) const {
    // Перевірка рядків і стовпців
    for (int i = 0; i < size; ++i) {
        bool rowWin = true, colWin = true;
        for (int j = 0; j < size; ++j) {
            if (cells[i][j] != symbol) rowWin = false;
            if (cells[j][i] != symbol) colWin = false;
        }
        if (rowWin || colWin) return true;
    }
    
    // Перевірка діагоналей
    bool diag1 = true, diag2 = true;
    for (int i = 0; i < size; ++i) {
        if (cells[i][i] != symbol) diag1 = false;
        if (cells[i][size - 1 - i] != symbol) diag2 = false;
    }
    return diag1 || diag2;
}
```

### 👤 `Player` (базовий клас)
Абстрактний клас для всіх типів гравців.

```cpp
class Player {
protected:
    char symbol;        // Символ гравця ('X' або 'O')
    std::string name;   // Ім'я гравця

public:
    Player(char s, std::string n);
    virtual ~Player() = default;
    
    virtual std::pair<int, int> getMove(const GameField& field) = 0; // Чисто віртуальний метод
    
    char getSymbol() const;
    const std::string& getName() const;
    void setSymbol(char s);
    void setName(const std::string& n);
};
```

### 🧑 `HumanPlayer`
Реалізація гравця-людини з валідацією вводу.

```cpp
class HumanPlayer : public Player {
public:
    HumanPlayer(char s, std::string n);
    std::pair<int, int> getMove(const GameField& field) override;
};
```

**Алгоритм отримання ходу:**
1. Запит координат у користувача
2. Валідація введених даних
3. Перевірка меж поля
4. Перевірка доступності клітинки
5. Повернення координат або повторний запит

### 🤖 `AIPlayer`
Реалізація штучного інтелекту з алгоритмом Minimax.

```cpp
class AIPlayer : public Player {
public:
    enum class Difficulty { NEWBIE, EASY, MEDIUM, HARD, EXPERT, MASTER };

private:
    Difficulty difficulty;      // Рівень складності
    char opponentSymbol;        // Символ опонента
    std::mt19937 rng;          // Генератор випадкових чисел
    
    int evaluate(const GameField& field) const;
    int minimax(GameField field, int depth, bool isMaximizing, 
                int alpha, int beta, int maxDepth) const;
    bool makeStrategicMistake();

public:
    AIPlayer(char s, std::string n, Difficulty d, char opp);
    std::pair<int, int> getMove(const GameField& field) override;
    
    static std::string getDifficultyName(Difficulty difficulty);
    void setDifficulty(Difficulty d);
    Difficulty getDifficulty() const;
};
```

---

## 🧠 Алгоритм штучного інтелекту

### 🎯 Алгоритм Minimax

Minimax - це рекурсивний алгоритм пошуку найкращого ходу в іграх з нульовою сумою. Він моделює всі можливі ходи до певної глибини та вибирає оптимальний.

```cpp
int minimax(GameField field, int depth, bool isMaximizing, 
            int alpha, int beta, int maxDepth) const {
    
    // 1. Оцінка поточної позиції
    int score = evaluate(field);
    
    // 2. Базові випадки (кінець рекурсії)
    if (score != 0) return score - (isMaximizing ? depth : -depth);
    if (field.isFull()) return 0;
    if (depth >= maxDepth) return 0;
    
    // 3. Рекурсивний пошук
    if (isMaximizing) {  // Хід ШІ (максимізація)
        int best = -1000;
        for (const auto& move : field.getAvailableMoves()) {
            GameField newField = field;
            newField.placeSymbol(move.first, move.second, symbol);
            
            int eval = minimax(newField, depth + 1, false, alpha, beta, maxDepth);
            best = std::max(best, eval);
            alpha = std::max(alpha, best);
            
            // Alpha-Beta відсічення
            if (beta <= alpha) break;
        }
        return best;
    } else {  // Хід опонента (мінімізація)
        int best = 1000;
        for (const auto& move : field.getAvailableMoves()) {
            GameField newField = field;
            newField.placeSymbol(move.first, move.second, opponentSymbol);
            
            int eval = minimax(newField, depth + 1, true, alpha, beta, maxDepth);
            best = std::min(best, eval);
            beta = std::min(beta, best);
            
            // Alpha-Beta відсічення
            if (beta <= alpha) break;
        }
        return best;
    }
}
```

### 📊 Функція оцінки позиції

```cpp
int evaluate(const GameField& field) const {
    if (field.checkWin(symbol)) return 10;        // ШІ виграв
    if (field.checkWin(opponentSymbol)) return -10; // Опонент виграв
    return 0;                                      // Нічия або гра триває
}
```

**Корекція оцінки по глибині:**
```cpp
return score - (isMaximizing ? depth : -depth);
```
Це забезпечує перевагу швидшому виграшу над повільним.

### ⚡ Alpha-Beta відсічення

Alpha-Beta відсічення - це оптимізація алгоритму Minimax, яка дозволяє "обрізати" гілки дерева пошуку, які точно не призведуть до кращого результату.

```
Alpha (α) - найкраща оцінка для максимізуючого гравця
Beta (β)  - найкраща оцінка для мінімізуючого гравця

Якщо β ≤ α, то подальший пошук у цій гілці безглуздий
```

**Приклад роботи:**
```
        MAX
       /   \
     MIN   MIN
    / \   / \
   3   5 2   1
   
Після знаходження 3 та 5 у лівій гілці, α = 5
У правій гілці після знаходження 2, β = 2
Оскільки β(2) ≤ α(5), перевірка останнього вузла (1) не потрібна
```

### 🎲 Система випадкових помилок

```cpp
bool makeStrategicMistake() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double mistakeChance;
    
    switch (difficulty) {
        case Difficulty::NEWBIE: mistakeChance = 0.7; break;   // 70% помилок
        case Difficulty::EASY: mistakeChance = 0.4; break;     // 40% помилок
        case Difficulty::MEDIUM: mistakeChance = 0.2; break;   // 20% помилок
        case Difficulty::HARD: mistakeChance = 0.1; break;     // 10% помилок
        case Difficulty::EXPERT: mistakeChance = 0.05; break;  // 5% помилок
        default: mistakeChance = 0.0; break;                   // 0% помилок (MASTER)
    }
    
    return dist(rng) < mistakeChance;
}
```

**Призначення:** Робить ШІ більш "людським" та цікавим для гри.

---

## 🎚️ Рівні складності ШІ

### 🆕 NEWBIE (Новачок)
- **Стратегія:** 70% випадкових ходів
- **Алгоритм:** Практично не використовує логіку
- **Час "думання":** 300мс
- **Код:**
```cpp
case Difficulty::NEWBIE: {
    return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
}
```

### 🟢 EASY (Легкий)
- **Стратегія:** 40% помилок + базова логіка
- **Алгоритм:** Іноді розпізнає виграшні ходи (50% шанс)
- **Час "думання":** 400мс
- **Код:**
```cpp
case Difficulty::EASY: {
    for (const auto& move : available) {
        GameField temp = field;
        temp.placeSymbol(move.first, move.second, symbol);
        if (temp.checkWin(symbol)) {
            if (std::uniform_real_distribution<>(0, 1)(rng) < 0.5) {
                return move; // 50% шанс помітити виграшний хід
            }
        }
    }
    return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
}
```

### 🟡 MEDIUM (Середній)
- **Стратегія:** 20% помилок + тактична логіка
- **Алгоритм:** 
  1. Завжди робить виграшний хід
  2. Блокує очевидні загрози
  3. Займає центр (для 3x3)
- **Час "думання":** 600мс

```cpp
case Difficulty::MEDIUM: {
    // Пошук виграшного ходу
    for (const auto& move : available) {
        GameField temp = field;
        temp.placeSymbol(move.first, move.second, symbol);
        if (temp.checkWin(symbol)) return move;
    }
    
    // Блокування загроз
    for (const auto& move : available) {
        GameField temp = field;
        temp.placeSymbol(move.first, move.second, opponentSymbol);
        if (temp.checkWin(opponentSymbol)) return move;
    }
    
    // Стратегія центру
    if (field.getSize() == 3 && field.getCell(1, 1) == ' ') {
        return {1, 1};
    }
    
    // Випадковий хід
    return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
}
```

### 🔴 HARD (Важкий)
- **Стратегія:** 10% помилок + Minimax (глибина 2)
- **Алгоритм:** Дивиться на 2 ходи вперед
- **Час "думання":** 800мс

### 🟣 EXPERT (Експерт)
- **Стратегія:** 5% помилок + Minimax (глибина 4)
- **Алгоритм:** Дивиться на 4 ходи вперед
- **Час"думання":** 1000мс

### ⚫ MASTER (Майстер)
- **Стратегія:** 0% помилок + повний Minimax
- **Алгоритм:** Аналізує всі можливі ходи до кінця гри
- **Час "думання":** 1200мс
- **Результат:** Практично непереможний

```cpp
default: { // MASTER
    int bestVal = -1000;
    std::pair<int, int> bestMove = {-1, -1};
    
    for (const auto& move : available) {
        GameField temp = field;
        temp.placeSymbol(move.first, move.second, symbol);
        int moveVal = minimax(temp, 0, false, -1000, 1000, 999);
        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMove = move;
        }
    }
    return bestMove;
}
```

---

## 🎯 Система оцінки позицій

### 📈 Покрокова оцінка ходу

1. **Генерація всіх можливих ходів**
```cpp
auto available = field.getAvailableMoves();
```

2. **Оцінка кожного ходу**
```cpp
for (const auto& move : available) {
    GameField temp = field;
    temp.placeSymbol(move.first, move.second, symbol);
    int moveVal = minimax(temp, 0, false, -1000, 1000, maxDepth);
    
    if (moveVal > bestVal) {
        bestVal = moveVal;
        bestMove = move;
    }
}
```

3. **Вибір найкращого ходу**
Хід з найвищою оцінкою обирається як оптимальний.

### 🏆 Пріоритети ходів

1. **Виграшний хід** (оцінка +10): Миттєвий виграш
2. **Блокуючий хід** (оцінка 0): Запобігання програшу
3. **Стратегічний хід** (оцінка 0): Поліпшення позиції
4. **Нейтральний хід** (оцінка 0): Підтримання рівноваги

### 📊 Математична модель

Для поля 3x3 існує:
- **Початкових позицій:** 9
- **Максимальна глибина гри:** 9 ходів
- **Загальна кількість можливих ігор:** 255,168
- **Оптимальний результат:** Нічия при правильній грі з обох сторін

---

## 💾 Збереження статистики

### 📁 Клас `GameStats`

```cpp
class GameStats {
private:
    std::string filename;                    // Ім'я файлу статистики
    std::map<std::string, int> playerStats; // Статистика гравців

public:
    explicit GameStats(const std::string& filename = "game_stats.txt");
    
    void addWin(const std::string& playerName);    // Додати перемогу
    int getWins(const std::string& playerName) const; // Отримати кількість перемог
    void showLeaderboard() const;                  // Показати таблицю лідерів
    
private:
    void loadStats();  // Завантаження з файлу
    void saveStats();  // Збереження у файл
};
```

### 📄 Формат файлу статистики

```
Олександр:15
Комп'ютер:8
Марія:12
Комп'ютер 1:3
Комп'ютер 2:7
```

### 🏅 Алгоритм сортування лідерборду

```cpp
void showLeaderboard() const {
    // Сортування гравців за кількістю перемог
    std::vector<std::pair<std::string, int>> sortedStats(playerStats.begin(), playerStats.end());
    std::sort(sortedStats.begin(), sortedStats.end(), 
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Вивід відсортованого списку
    for (const auto& [player, wins] : sortedStats) {
        std::cout << std::left << std::setw(20) << player << std::setw(10) << wins << "\n";
    }
}
```

---

## 🖥️ Крос-платформна підтримка

### 🐧 Linux
```cpp
#ifndef _WIN32
    std::setlocale(LC_ALL, "");  // Підтримка UTF-8
#endif
```

### 🪟 Windows
```cpp
#ifdef _WIN32
    // Налаштування UTF-8 кодування
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
    // Включення ANSI escape послідовностей
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
```

### 🎨 Кольорова підтримка

**ANSI Escape коди:**
```cpp
static std::string colorText(const std::string& text, Color color) {
    return "\033[" + std::to_string(static_cast<int>(color)) + "m" + text + "\033[0m";
}
```

**Кросплатформне очищення екрану:**
```cpp
static void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
```

### ⏰ Кросплатформні затримки

```cpp
#ifdef _WIN32
    Sleep(thinkTime/3);  // Windows API
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(thinkTime/3)); // POSIX
#endif
```

---

## 🎮 Інтерфейс користувача

### 🏠 Головне меню

```
=== МЕНЮ ГРИ ===
1. Почати нову гру
2. Переглянути статистику
3. Змінити розмір поля (поточний: 3x3)
4. Перемкнути кольоровий режим
5. Вихід
```

### 🎯 Налаштування гри

1. **Вибір розміру поля:**
   - 3x3 (класичні)
   - 4x4 (розширені)
   - 5x5 (великі)

2. **Вибір режиму гри:**
   - Гравець проти Гравця
   - Гравець проти Комп'ютера
   - Комп'ютер проти Комп'ютера (демонстрація)

3. **Вибір складності ШІ:**
   - Новачок → Майстер (6 рівнів)

### 🎲 Відображення поля

```
  1   2   3 
-------------
1 |X | |O |
-------------
2 | |X | |
-------------
3 |O | |X |
-------------
```

### 🎨 Кольорова схема

- **X символи:** 🔴 Червоний
- **O символи:** 🔵 Синій
- **Меню:** 🟦 Жирний
- **Помилки:** 🟥 Червоний фон
- **Успіх:** 🟢 Зелений
- **Інформація:** 🟦 Блакитний

---

## ⚙️ Технічні деталі

### 📋 Вимоги до системи

- **Компілятор:** GCC 7.0+ або MSVC 2017+ з підтримкою C++17
- **Стандарт:** C++17
- **Платформи:** Linux, Windows
- **Бібліотеки:** STL (стандартна бібліотека)

### 🔧 Використані технології C++17

1. **Structured bindings:**
```cpp
for (const auto& [player, wins] : sortedStats) { ... }
```

2. **std::optional-подібна логіка через винятки:**
```cpp
try {
    char cell = field.getCell(row, col);
} catch (const std::out_of_range& e) { ... }
```

3. **Lambda expressions:**
```cpp
std::sort(sortedStats.begin(), sortedStats.end(), 
         [](const auto& a, const auto& b) { return a.second > b.second; });
```

4. **Smart pointers:**
```cpp
std::unique_ptr<Player> player1;
std::unique_ptr<Player> player2;
```

5. **enum class:**
```cpp
enum class Difficulty { NEWBIE, EASY, MEDIUM, HARD, EXPERT, MASTER };
```

### 🏗️ Принципи ООП

1. **Інкапсуляція:** Приватні дані з публічними методами доступу
2. **Наслідування:** `HumanPlayer` та `AIPlayer` наслідують `Player`
3. **Поліморфізм:** Віртуальний метод `getMove()`
4. **Абстракція:** Абстрактний клас `Player`

### 🧠 Алгоритмічна складність

- **Minimax без відсічення:** O(b^d), де b - кількість ходів, d - глибина
- **Minimax з Alpha-Beta:** O(b^(d/2)) у середньому випадку
- **Пошук виграшного ходу:** O(n), де n - кількість доступних клітинок
- **Перевірка виграшу:** O(n), де n - розмір поля

### 📊 Оптимізації

1. **Alpha-Beta відсічення** - зменшує час виконання в ~2 рази
2. **Обмеження глибини** - балансує силу гри та швидкість
3. **Копіювання полів** - безпечне тестування ходів
4. **Ранній вихід** - зупинка при знаходженні виграшного ходу

### 🔒 Безпека коду

1. **Валідація введення:** Перевірка меж масивів
2. **Обробка винятків:** try-catch блоки
3. **RAII:** Автоматичне управління ресурсами
4. **const-коректність:** Незмінні методи там, де можливо

---

## 🚀 Збірка проекту

### 📁 Makefile

```makefile
# Linux збірка
make linux
./tic_tac_toe

# Windows збірка (потрібен mingw-w64)
make windows
# Запуск через wine для тестування
wine tic_tac_toe.exe

# Очищення
make clean
```

### 🛠️ Компілятор команди

**Linux:**
```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread main.cpp -o tic_tac_toe
```

**Windows (MinGW):**
```bash
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -pthread main.cpp -lwinpthread -o tic_tac_toe.exe
```

**Visual Studio:**
```cmd
cl /EHsc /std:c++17 /O2 main.cpp /Fe:tic_tac_toe.exe
```

---

## 🎓 Навчальна цінність

Цей проект демонструє:

1. **Алгоритми:** Minimax, Alpha-Beta відсічення
2. **Структури даних:** Векторы, мапи, пари
3. **ООП:** Наслідування, поліморфізм, інкапсуляція
4. **Обробка файлів:** Читання/запис статистики
5. **Крос-платформне програмування:** Умовна компіляція
6. **Випадкові числа:** Mersenne Twister, розподіли
7. **Обробка помилок:** Винятки, валідація
8. **Інтерфейс користувача:** Консольний ввід/вивід

---

## 📈 Можливі покращення

1. **Графічний інтерфейс:** Qt або SDL2
2. **Мережева гра:** TCP/UDP сокети
3. **База даних:** SQLite для статистики
4. **Штучні нейронні мережі:** TensorFlow для ШІ
5. **Веб-інтерфейс:** WebAssembly порт
6. **Мобільні платформи:** Android/iOS версії
7. **Турнірний режим:** Система рейтингів
8. **Збереження ігор:** Можливість продовжити пізніше

---

## 📚 Висновок

Цей проект є відмінним прикладом застосування алгоритмів штучного інтелекту в ігровому програмуванні. Алгоритм Minimax з Alpha-Beta відсіченням демонструє, як математичні методи можуть створити непереможного опонента у детермінованих іграх.

Система рівнів складності показує, як можна адаптувати потужний алгоритм для різних типів користувачів, від новачків до експертів. Використання сучасних можливостей C++17 та принципів ООП робить код читабельним, розширюваним та підтримуваним.

Крос-платформна підтримка та увага до деталей користувацького інтерфейсу демонструють професійний підхід до розробки програмного забезпечення.

---

## 📞 Контакти

Проект створено як курсова робота з демонстрації алгоритмів штучного інтелекту та сучасних практик програмування на C++.

**Версія:** 1.0  
**Дата:** 2025  
**Мова:** C++17  
**Ліцензія:** Навчальна

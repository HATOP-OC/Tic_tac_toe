#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>

 
class ConsoleColor {
public:
    enum class Color {
        RESET = 0,
        BLACK = 30,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37
    };

    static std::string colorText(const std::string& text, Color color) {
        return "\033[" + std::to_string(static_cast<int>(color)) + "m" + text + "\033[0m";
    }
    
    static std::string bold(const std::string& text) {
        return "\033[1m" + text + "\033[0m";
    }
    
    static std::string colorChar(char c, Color color) {
        return "\033[" + std::to_string(static_cast<int>(color)) + "m" + c + "\033[0m";
    }
    
    static void clearScreen() {
        #ifdef _WIN32
            int result = system("cls");
            (void)result;  
        #else
            int result = system("clear");
            (void)result;  
        #endif
    }
};


class GameField {
private:
    std::vector<std::vector<char>> cells;
    int size;
    std::map<char, ConsoleColor::Color> symbolColors;

public:
    explicit GameField(int size = 3) : size(size) {
        reset();
        symbolColors['X'] = ConsoleColor::Color::RED;
        symbolColors['O'] = ConsoleColor::Color::BLUE;
    }

    // Копіювальний конструктор
    GameField(const GameField& other) : cells(other.cells), size(other.size), symbolColors(other.symbolColors) {}

    // Оператор присвоєння
    GameField& operator=(const GameField& other) {
        if (this != &other) {
            cells = other.cells;
            size = other.size;
            symbolColors = other.symbolColors;
        }
        return *this;
    }

    void reset() {
        cells = std::vector<std::vector<char>>(size, std::vector<char>(size, ' '));
    }

    bool placeSymbol(int row, int col, char symbol) {
        if (row < 0 || row >= size || col < 0 || col >= size) return false;
        if (cells[row][col] != ' ') return false;
        cells[row][col] = symbol;
        return true;
    }

    bool checkWin(char symbol) const {
        // Перевірка рядків і стовпців
        for (int i = 0; i < size; ++i) {
            bool rowWin = true;
            bool colWin = true;
            for (int j = 0; j < size; ++j) {
                if (cells[i][j] != symbol) rowWin = false;
                if (cells[j][i] != symbol) colWin = false;
            }
            if (rowWin || colWin) return true;
        }

        // Перевірка діагоналей
        bool diag1 = true;
        bool diag2 = true;
        for (int i = 0; i < size; ++i) {
            if (cells[i][i] != symbol) diag1 = false;
            if (cells[i][size - 1 - i] != symbol) diag2 = false;
        }
        return diag1 || diag2;
    }

    bool isFull() const {
        for (const auto& row : cells) {
            for (char cell : row) {
                if (cell == ' ') return false;
            }
        }
        return true;
    }

    char getCell(int row, int col) const {
        if (row < 0 || row >= size || col < 0 || col >= size) {
            throw std::out_of_range("Індекси клітинки поза межами поля");
        }
        return cells[row][col];
    }

    int getSize() const { return size; }

    // Отримати всі доступні ходи
    std::vector<std::pair<int, int>> getAvailableMoves() const {
        std::vector<std::pair<int, int>> moves;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (cells[i][j] == ' ') {
                    moves.emplace_back(i, j);
                }
            }
        }
        return moves;
    }
    
    // Додавання кольору для символу
    void setSymbolColor(char symbol, ConsoleColor::Color color) {
        symbolColors[symbol] = color;
    }
    
    // Отримання кольору символу
    ConsoleColor::Color getSymbolColor(char symbol) const {
        if (symbolColors.find(symbol) != symbolColors.end()) {
            return symbolColors.at(symbol);
        }
        return ConsoleColor::Color::WHITE;
    }
};

// Базовий клас гравця
class Player {
protected:
    char symbol;
    std::string name;

public:
    Player(char s, std::string n) : symbol(s), name(std::move(n)) {}
    virtual ~Player() = default;
    virtual std::pair<int, int> getMove(const GameField& field) = 0;
    
    char getSymbol() const { return symbol; }
    const std::string& getName() const { return name; }
    
    void setSymbol(char s) { symbol = s; }
    void setName(const std::string& n) { name = n; }
};

// Клас людини-гравця
class HumanPlayer : public Player {
public:
    HumanPlayer(char s, std::string n) : Player(s, std::move(n)) {}

    std::pair<int, int> getMove(const GameField& field) override {
        int row, col;
        do {
            std::cout << ConsoleColor::colorText(name, ConsoleColor::Color::GREEN) 
                      << ", введіть рядок (1-" << field.getSize()
                      << ") та стовпець (1-" << field.getSize() << "): ";
            
            // Перевірка правильності введення
            if (!(std::cin >> row >> col)) {
                std::cin.clear(); // Очищення помилки
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ігнорування невірного вводу
                std::cout << ConsoleColor::colorText("Неправильний ввід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                row = col = -1;
                continue;
            }
            
            row--; col--; // Переводимо з 1-індексації в 0-індексацію
            
            if (row < 0 || row >= field.getSize() ||
                col < 0 || col >= field.getSize()) {
                std::cout << ConsoleColor::colorText("Координати повинні бути в межах поля! Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                continue;
            }
            
            if (field.getCell(row, col) != ' ') {
                std::cout << ConsoleColor::colorText("Ця клітинка вже зайнята! Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
            }
        } while (row < 0 || row >= field.getSize() ||
                col < 0 || col >= field.getSize() ||
                field.getCell(row, col) != ' ');
        
        return {row, col};
    }
};

// Клас комп'ютерного гравця
class AIPlayer : public Player {
public:
    enum class Difficulty { NEWBIE, EASY, MEDIUM, HARD, EXPERT, MASTER };

private:
    Difficulty difficulty;
    char opponentSymbol;
    std::mt19937 rng; // Генератор випадкових чисел

    // Функція оцінки для мініміксного алгоритму
    int evaluate(const GameField& field) const {
        if (field.checkWin(symbol)) return 10;
        if (field.checkWin(opponentSymbol)) return -10;
        return 0;
    }

    // Мінімакс алгоритм з альфа-бета відсіченням
    int minimax(GameField field, int depth, bool isMaximizing, int alpha, int beta, int maxDepth) const {
        int score = evaluate(field);

        // Якщо є переможець або поле заповнене
        if (score != 0) return score - (isMaximizing ? depth : -depth);
        if (field.isFull()) return 0;
        
        // Обмеження глибини пошуку для різних рівнів складності
        if (depth >= maxDepth) {
            return 0;
        }

        if (isMaximizing) {
            int best = -1000;
            for (const auto& move : field.getAvailableMoves()) {
                GameField newField = field;
                newField.placeSymbol(move.first, move.second, symbol);
                best = std::max(best, minimax(newField, depth + 1, !isMaximizing, alpha, beta, maxDepth));
                alpha = std::max(alpha, best);
                if (beta <= alpha) break; // Альфа-бета відсічення
            }
            return best;
        } else {
            int best = 1000;
            for (const auto& move : field.getAvailableMoves()) {
                GameField newField = field;
                newField.placeSymbol(move.first, move.second, opponentSymbol);
                best = std::min(best, minimax(newField, depth + 1, !isMaximizing, alpha, beta, maxDepth));
                beta = std::min(beta, best);
                if (beta <= alpha) break; // Альфа-бета відсічення
            }
            return best;
        }
    }
    
    // Стратегічні помилки для нижчих рівнів складності
    bool makeStrategicMistake() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double mistakeChance;
        
        switch (difficulty) {
            case Difficulty::NEWBIE: mistakeChance = 0.7; break;
            case Difficulty::EASY: mistakeChance = 0.4; break;
            case Difficulty::MEDIUM: mistakeChance = 0.2; break;
            case Difficulty::HARD: mistakeChance = 0.1; break;
            case Difficulty::EXPERT: mistakeChance = 0.05; break;
            default: mistakeChance = 0.0; break;
        }
        
        return dist(rng) < mistakeChance;
    }

public:
    AIPlayer(char s, std::string n, Difficulty d, char opp)
        : Player(s, std::move(n)), difficulty(d), opponentSymbol(opp) {
        // Ініціалізація генератора випадкових чисел
        std::random_device rd;
        rng = std::mt19937(rd());
    }

    std::pair<int, int> getMove(const GameField& field) override {
        // Затримка для створення ілюзії "обдумування"
        std::cout << ConsoleColor::colorText(name, ConsoleColor::Color::CYAN) << " думає";
        
        int thinkTime;
        switch (difficulty) {
            case Difficulty::NEWBIE: thinkTime = 300; break;
            case Difficulty::EASY: thinkTime = 400; break;
            case Difficulty::MEDIUM: thinkTime = 600; break;
            case Difficulty::HARD: thinkTime = 800; break;
            case Difficulty::EXPERT: thinkTime = 1000; break;
            default: thinkTime = 1200; break;
        }
        
        // Анімація "думання"
        for (int i = 0; i < 3; i++) {
            std::cout << ".";
            std::cout.flush();
#ifdef _WIN32
            Sleep(thinkTime/3);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(thinkTime/3));
#endif
        }
        std::cout << "\n";
        
        auto available = field.getAvailableMoves();
        
        if (available.empty()) {
            throw std::runtime_error("Немає доступних ходів!");
        }

        // Якщо ШІ повинен зробити помилку, вибираємо випадковий хід
        if (makeStrategicMistake()) {
            return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
        }

        switch (difficulty) {
            case Difficulty::NEWBIE: {
                // Новачок переважно робить випадкові ходи
                return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
            }
            case Difficulty::EASY: {
                // Легкий рівень: іноді бачить виграшні ходи
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
            case Difficulty::MEDIUM: {
                // Середній рівень: бачить виграшні ходи і блокує очевидні загрози
                // Спроба знайти виграшний хід
                for (const auto& move : available) {
                    GameField temp = field;
                    temp.placeSymbol(move.first, move.second, symbol);
                    if (temp.checkWin(symbol)) return move;
                }
                
                // Спроба блокувати противника
                for (const auto& move : available) {
                    GameField temp = field;
                    temp.placeSymbol(move.first, move.second, opponentSymbol);
                    if (temp.checkWin(opponentSymbol)) return move;
                }
                
                // Якщо центр вільний, зайняти його (для поля 3x3)
                if (field.getSize() == 3 && field.getCell(1, 1) == ' ') {
                    return {1, 1};
                }
                
                // Інакше випадковий хід
                return available[std::uniform_int_distribution<int>(0, available.size() - 1)(rng)];
            }
            case Difficulty::HARD: {
                // Важкий рівень: використовує мінімакс з обмеженою глибиною
                int bestVal = -1000;
                std::pair<int, int> bestMove = {-1, -1};

                // Обмежена глибина для алгоритму мінімакс
                int maxDepth = 2;

                for (const auto& move : available) {
                    GameField temp = field;
                    temp.placeSymbol(move.first, move.second, symbol);
                    int moveVal = minimax(temp, 0, false, -1000, 1000, maxDepth);
                    if (moveVal > bestVal) {
                        bestVal = moveVal;
                        bestMove = move;
                    }
                }
                return bestMove;
            }
            case Difficulty::EXPERT: {
                // Експертний рівень: використовує мінімакс з більшою глибиною
                int bestVal = -1000;
                std::pair<int, int> bestMove = {-1, -1};

                // Більша глибина для алгоритму мінімакс
                int maxDepth = 4;

                for (const auto& move : available) {
                    GameField temp = field;
                    temp.placeSymbol(move.first, move.second, symbol);
                    int moveVal = minimax(temp, 0, false, -1000, 1000, maxDepth);
                    if (moveVal > bestVal) {
                        bestVal = moveVal;
                        bestMove = move;
                    }
                }
                return bestMove;
            }
            default: { // MASTER
                // Майстер: повний мінімакс без обмежень
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
        }
    }
    
    // Метод для отримання назви складності
    static std::string getDifficultyName(Difficulty difficulty) {
        switch (difficulty) {
            case Difficulty::NEWBIE: return "Новачок";
            case Difficulty::EASY: return "Легкий";
            case Difficulty::MEDIUM: return "Середній";
            case Difficulty::HARD: return "Важкий";
            case Difficulty::EXPERT: return "Експерт";
            case Difficulty::MASTER: return "Майстер";
            default: return "Невідомий";
        }
    }
    
    // Метод для встановлення складності
    void setDifficulty(Difficulty d) {
        difficulty = d;
    }
    
    // Метод для отримання поточної складності
    Difficulty getDifficulty() const {
        return difficulty;
    }
};

// Клас для збереження та завантаження статистики гри
class GameStats {
private:
    std::string filename;
    std::map<std::string, int> playerStats;

public:
    explicit GameStats(const std::string& filename = "game_stats.txt") : filename(filename) {
        loadStats();
    }

    void addWin(const std::string& playerName) {
        playerStats[playerName]++;
        saveStats();
    }

    int getWins(const std::string& playerName) const {
        if (playerStats.find(playerName) != playerStats.end()) {
            return playerStats.at(playerName);
        }
        return 0;
    }

    void showLeaderboard() const {
        std::cout << ConsoleColor::bold("\n=== ТАБЛИЦЯ ЛІДЕРІВ ===\n");
        
        // Сортування гравців за кількістю перемог
        std::vector<std::pair<std::string, int>> sortedStats(playerStats.begin(), playerStats.end());
        std::sort(sortedStats.begin(), sortedStats.end(), 
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        if (sortedStats.empty()) {
            std::cout << "Поки що немає даних.\n";
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Гравець" << std::setw(10) << "Перемоги" << "\n";
        std::cout << std::string(30, '-') << "\n";
        
        for (const auto& [player, wins] : sortedStats) {
            std::cout << std::left << std::setw(20) << player << std::setw(10) << wins << "\n";
        }
    }

private:
    void loadStats() {
        std::ifstream file(filename);
        if (!file.is_open()) return;
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            int wins;
            
            if (std::getline(iss, name, ':') && iss >> wins) {
                playerStats[name] = wins;
            }
        }
    }

    void saveStats() {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Не вдалося відкрити файл для запису статистики.\n";
            return;
        }
        
        for (const auto& [player, wins] : playerStats) {
            file << player << ":" << wins << "\n";
        }
    }
};


// Клас для керування грою
class GameManager {
private:
    GameField field;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    Player* currentPlayer;
    GameStats stats;
    bool useColors;
    
public:
    GameManager() : field(3), currentPlayer(nullptr), useColors(true) {}
    
    void setFieldSize(int size) {
        if (size < 3 || size > 10) {
            throw std::invalid_argument("Розмір поля повинен бути від 3 до 10");
        }
        field = GameField(size);
    }
    
    int getFieldSize() const {
        return field.getSize();
    }
    
    void setPlayers(std::unique_ptr<Player> p1, std::unique_ptr<Player> p2) {
        player1 = std::move(p1);
        player2 = std::move(p2);
        currentPlayer = player1.get();
    }
    
    void toggleColors() {
        useColors = !useColors;
        std::cout << "Кольоровий режим " << (useColors ? "увімкнено" : "вимкнено") << "\n";
    }
    
    void setSymbolColor(char symbol, ConsoleColor::Color color) {
        field.setSymbolColor(symbol, color);
    }
    
    void displayField() const {
        int size = field.getSize();
        
        // Друк заголовка колонок
        std::cout << "  ";
        for (int i = 0; i < size; ++i) {
            std::cout << " " << (i + 1) << " ";
        }
        std::cout << "\n";
        
        // Друк верхньої межі
        std::cout << "  ";
        for (int i = 0; i < size; ++i) {
            std::cout << "---";
        }
        std::cout << "\n";
        
        // Друк клітинок поля
        for (int i = 0; i < size; ++i) {
            std::cout << (i + 1) << " |";
            for (int j = 0; j < size; ++j) {
                char cell = field.getCell(i, j);
                if (useColors && cell != ' ') {
                    std::cout << ConsoleColor::colorChar(cell, field.getSymbolColor(cell)) << " |";
                } else {
                    std::cout << cell << " |";
                }
            }
            std::cout << "\n";
            
            // Друк нижньої межі
            std::cout << "  ";
            for (int j = 0; j < size; ++j) {
                std::cout << "---";
            }
            std::cout << "\n";
        }
    }
    
    void playRound() {
        field.reset();
        currentPlayer = player1.get();
        
        ConsoleColor::clearScreen();
        std::cout << ConsoleColor::bold("=== ПОЧАТОК ГРИ ===\n");
        std::cout << "Гравець 1: " << ConsoleColor::colorText(player1->getName(), ConsoleColor::Color::RED) 
                  << " (" << player1->getSymbol() << ")\n";
        std::cout << "Гравець 2: " << ConsoleColor::colorText(player2->getName(), ConsoleColor::Color::BLUE)
                  << " (" << player2->getSymbol() << ")\n\n";
        
        bool gameOver = false;
        while (!gameOver) {
            displayField();
            
            // Отримання ходу від поточного гравця
            std::pair<int, int> move = currentPlayer->getMove(field);
            
            // Розміщення символу на полі
            if (!field.placeSymbol(move.first, move.second, currentPlayer->getSymbol())) {
                std::cout << ConsoleColor::colorText("Неможливо зробити хід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                continue;
            }
            
            // Перевірка на виграш
            if (field.checkWin(currentPlayer->getSymbol())) {
                ConsoleColor::clearScreen();
                displayField();
                std::cout << ConsoleColor::colorText(currentPlayer->getName() + " перемагає!", 
                                                    ConsoleColor::Color::GREEN) << "\n";
                stats.addWin(currentPlayer->getName());
                gameOver = true;
            }
            // Перевірка на нічию
            else if (field.isFull()) {
                ConsoleColor::clearScreen();
                displayField();
                std::cout << ConsoleColor::colorText("Нічия!", ConsoleColor::Color::YELLOW) << "\n";
                gameOver = true;
            }
            // Перемикання гравця
            else {
                currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
                ConsoleColor::clearScreen();
            }
        }
        
        std::cout << ConsoleColor::bold("=== КІНЕЦЬ ГРИ ===\n");
    }
    
    void showStatistics() const {
        stats.showLeaderboard();
    }
};

// Налаштування гри
class GameSetup {
private:
    static void clearScreen() {
        ConsoleColor::clearScreen();
    }
    
    static std::string getDifficultyString(AIPlayer::Difficulty difficulty) {
        return AIPlayer::getDifficultyName(difficulty);
    }
    
    static void showIntro() {
        clearScreen();
        std::string title = R"(  
    ██    ██   ███████    ████████   ██████   █████                    ██████    ██████  
    ██    ██  ██     ██      ██     ██    ██ ██   ██                  ██    ██  ██      
    ████████  █████████      ██     ██    ██ ██████                   ██    ██  ██      
    ██    ██  ██     ██      ██     ██    ██ ██         ████████      ██    ██  ██      
    ██    ██  ██     ██      ██      ██████  ██         └──────┘       ██████    ██████  
    └─    └─  └─     └─      └─      └────┘  └─                         └────┘    └────┘       
)";
        std::cout << ConsoleColor::colorText(title, ConsoleColor::Color::CYAN) << "\n";
        std::cout << ConsoleColor::bold("Ласкаво просимо до гри ХРЕСТИКИ-НУЛИКИ!") << "\n";
        std::cout << "Версія 1.0\n";
        std::cout << "© 2025 Усі права захищені\n\n";
        std::cout << "Натисніть Enter, щоб продовжити...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    static AIPlayer::Difficulty chooseDifficulty() {
        std::cout << ConsoleColor::bold("\nВиберіть рівень складності комп'ютера:\n");
        std::cout << "1. " << getDifficultyString(AIPlayer::Difficulty::NEWBIE) << "\n";
        std::cout << "2. " << getDifficultyString(AIPlayer::Difficulty::EASY) << "\n";
        std::cout << "3. " << getDifficultyString(AIPlayer::Difficulty::MEDIUM) << "\n";
        std::cout << "4. " << getDifficultyString(AIPlayer::Difficulty::HARD) << "\n";
        std::cout << "5. " << getDifficultyString(AIPlayer::Difficulty::EXPERT) << "\n";
        std::cout << "6. " << getDifficultyString(AIPlayer::Difficulty::MASTER) << "\n";
        
        int choice;
        do {
            std::cout << "Ваш вибір (1-6): ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ConsoleColor::colorText("Неправильний ввід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                choice = 0;
            }
        } while (choice < 1 || choice > 6);
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: return AIPlayer::Difficulty::NEWBIE;
            case 2: return AIPlayer::Difficulty::EASY;
            case 3: return AIPlayer::Difficulty::MEDIUM;
            case 4: return AIPlayer::Difficulty::HARD;
            case 5: return AIPlayer::Difficulty::EXPERT;
            default: return AIPlayer::Difficulty::MASTER;
        }
    }
    
    static int chooseFieldSize() {
        std::cout << ConsoleColor::bold("\nВиберіть розмір поля:\n");
        std::cout << "1. 3x3 (класичні)\n";
        std::cout << "2. 4x4\n";
        std::cout << "3. 5x5\n";
        
        int choice;
        do {
            std::cout << "Ваш вибір (1-3): ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ConsoleColor::colorText("Неправильний ввід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                choice = 0;
            }
        } while (choice < 1 || choice > 3);
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: return 3;
            case 2: return 4;
            case 3: return 5;
            default: return 3;
        }
    }
    
    static std::string getPlayerName(int playerNum) {
        std::string name;
        std::cout << "Введіть ім'я гравця " << playerNum << ": ";
        std::getline(std::cin, name);
        if (name.empty()) {
            name = "Гравець " + std::to_string(playerNum);
        }
        return name;
    }
    
    static int chooseGameMode() {
        std::cout << ConsoleColor::bold("\nВиберіть режим гри:\n");
        std::cout << "1. Гравець проти Гравця\n";
        std::cout << "2. Гравець проти Комп'ютера\n";
        std::cout << "3. Комп'ютер проти Комп'ютера (демонстрація)\n";
        
        int choice;
        do {
            std::cout << "Ваш вибір (1-3): ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ConsoleColor::colorText("Неправильний ввід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                choice = 0;
            }
        } while (choice < 1 || choice > 3);
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        return choice;
    }

public:
    static void setupGame(GameManager& manager) {
        showIntro();
        
        int fieldSize = chooseFieldSize();
        manager.setFieldSize(fieldSize);
        
        int gameMode = chooseGameMode();
        
        std::unique_ptr<Player> player1;
        std::unique_ptr<Player> player2;
        
        switch (gameMode) {
            case 1: { // Гравець проти Гравця
                std::string name1 = getPlayerName(1);
                std::string name2 = getPlayerName(2);
                player1 = std::make_unique<HumanPlayer>('X', name1);
                player2 = std::make_unique<HumanPlayer>('O', name2);
                break;
            }
            case 2: { // Гравець проти Комп'ютера
                std::string name = getPlayerName(1);
                AIPlayer::Difficulty difficulty = chooseDifficulty();
                
                std::cout << ConsoleColor::bold("\nБажаєте грати першим? (y/n): ");
                char choice;
                std::cin >> choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (tolower(choice) == 'y') {
                    player1 = std::make_unique<HumanPlayer>('X', name);
                    player2 = std::make_unique<AIPlayer>('O', "Комп'ютер", difficulty, 'X');
                } else {
                    player1 = std::make_unique<AIPlayer>('X', "Комп'ютер", difficulty, 'O');
                    player2 = std::make_unique<HumanPlayer>('O', name);
                }
                break;
            }
            case 3: { // Комп'ютер проти Комп'ютера
                std::cout << ConsoleColor::bold("\nНалаштування першого комп'ютера:\n");
                AIPlayer::Difficulty difficulty1 = chooseDifficulty();
                
                std::cout << ConsoleColor::bold("\nНалаштування другого комп'ютера:\n");
                AIPlayer::Difficulty difficulty2 = chooseDifficulty();
                
                player1 = std::make_unique<AIPlayer>('X', "Комп'ютер 1", difficulty1, 'O');
                player2 = std::make_unique<AIPlayer>('O', "Комп'ютер 2", difficulty2, 'X');
                break;
            }
        }
        
        manager.setPlayers(std::move(player1), std::move(player2));
        
        // Налаштування кольорів (за замовчуванням)
        manager.setSymbolColor('X', ConsoleColor::Color::RED);
        manager.setSymbolColor('O', ConsoleColor::Color::BLUE);
    }
    
    static bool playAgain() {
        std::cout << "\nБажаєте зіграти ще одну гру? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return (tolower(choice) == 'y');
    }
    
    static void showMenu(GameManager& manager) {
        bool running = true;
        
        while (running) {
            clearScreen();
            std::cout << ConsoleColor::bold("\n=== МЕНЮ ГРИ ===\n");
            std::cout << "1. Почати нову гру\n";
            std::cout << "2. Переглянути статистику\n";
            std::cout << "3. Змінити розмір поля (поточний: " << manager.getFieldSize() << "x" << manager.getFieldSize() << ")\n";
            std::cout << "4. Перемкнути кольоровий режим\n";
            std::cout << "5. Вихід\n";
            
            int choice;
            std::cout << "\nВаш вибір: ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ConsoleColor::colorText("Неправильний ввід. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            switch (choice) {
                case 1:
                    setupGame(manager);
                    do {
                        manager.playRound();
                    } while (playAgain());
                    break;
                case 2:
                    manager.showStatistics();
                    std::cout << "\nНатисніть Enter, щоб продовжити...";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                case 3: {
                    std::cout << "Введіть новий розмір поля (3-10): ";
                    int size;
                    if (std::cin >> size) {
                        try {
                            manager.setFieldSize(size);
                            std::cout << "Розмір поля змінено на " << size << "x" << size << "\n";
                        } catch (const std::exception& e) {
                            std::cerr << ConsoleColor::colorText("Помилка: " + std::string(e.what()), ConsoleColor::Color::RED) << "\n";
                        }
                    } else {
                        std::cin.clear();
                        std::cout << ConsoleColor::colorText("Неправильний ввід.", ConsoleColor::Color::RED) << "\n";
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Натисніть Enter, щоб продовжити...";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
                case 4:
                    manager.toggleColors();
                    std::cout << "Натисніть Enter, щоб продовжити...";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                case 5:
                    running = false;
                    std::cout << "Дякуємо за гру! До побачення!\n";
                    break;
                default:
                    std::cout << ConsoleColor::colorText("Невідома опція. Спробуйте ще раз.", ConsoleColor::Color::RED) << "\n";
                    std::cout << "Натисніть Enter, щоб продовжити...";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }
};

// Головна функція

int main() {
    // Налаштування локалі для підтримки Unicode
    #ifdef _WIN32
        // Налаштування UTF-8 та ANSI обробки консолі Windows
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #else
        std::setlocale(LC_ALL, "");
    #endif
    
    // Ініціалізація генератора випадкових чисел
    std::srand(std::time(nullptr));
    
    try {
        GameManager gameManager;
        GameSetup::showMenu(gameManager);
    } catch (const std::exception& e) {
        std::cerr << "Критична помилка: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

#include "NoteManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>
// тут создание папок вин и линукс 
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif
// просто список с тегами работа учеба и тд инициализация 
const std::vector<Tag> TAGS = {
    {"Work",     FG_BBLUE,    BG_BLUE,    "💼"},
    {"Personal", FG_BGREEN,   BG_GREEN,   "🌿"},
    {"Urgent",   FG_BRED,     BG_RED,     "🔥"},
    {"Ideas",    FG_BYELLOW,  BG_YELLOW,  "💡"},
    {"Study",    FG_BMAGENTA, BG_MAGENTA, "📚"},
    {"Other",    FG_BCYAN,    BG_CYAN,    "📌"},

// путь к папки где будут хранится заметки 
const std::string DATA_DIR  = "notes_data";
const std::string DATA_FILE = DATA_DIR + "/notes.dat";
// тут функция для автоматического создания папки где буду хранится заметки 
static void ensureDataDir() {
    MKDIR(DATA_DIR.c_str());
}
// клваиши для винды для работы приложени 
#ifdef _WIN32
int readKey() {
    int c = _getch();
    if (c == 0 || c == 224) {
        int c2 = _getch();
        switch (c2) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            case 71: return KEY_HOME;
            case 79: return KEY_END;
            case 73: return KEY_PGUP;
            case 81: return KEY_PGDN;
        }
    }
    if (c == '\r') return KEY_ENTER;
    if (c == 8)    return KEY_BACKSPACE;
    return c;
}
#else
// тут линукс 
static struct termios orig_termios;

static void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int readKey() {
    enableRawMode();
    int c = getchar();
    if (c == 27) {
        int c2 = getchar();
        if (c2 == '[') {
            int c3 = getchar();
            disableRawMode();
            switch (c3) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
                case '5': getchar(); return KEY_PGUP;
                case '6': getchar(); return KEY_PGDN;
            }
        }
        disableRawMode();
        return KEY_ESC;
    }
    disableRawMode();
    if (c == '\n' || c == '\r') return KEY_ENTER;
    if (c == 127 || c == 8)    return KEY_BACKSPACE;
    if (c == 19)               return KEY_CTRL_S;
    return c;
}
#endif

std::string lineInput(const std::string& prompt, int maxLen) {
    std::cout << prompt;
    std::string s;
    while (true) {
        int k = readKey(); // функция рид кей нужна для считывания клавиш 
        if (k == KEY_ENTER) break; // при нажатии enter все завершится 
        if (k == KEY_BACKSPACE) {
            if (!s.empty()) { s.pop_back(); std::cout << "\b \b"; }
        } else if (k >= 32 && k < 256) {
            if ((int)s.size() < maxLen) {
                s += (char)k;
                std::cout << (char)k;
                std::cout.flush();
            }
        }
    }
    std::cout << "\n";
    return s;
}
void clearScreen() { system(CLEAR); }

void topLine(int w, const std::string& clr) {
    if (!clr.empty()) std::cout << clr;
    std::cout << "╔";
    for (int i = 0; i < w - 2; i++) std::cout << "═";
    std::cout << "╗" << RESET << "\n";
}

void botLine(int w, const std::string& clr) {
    if (!clr.empty()) std::cout << clr;
    std::cout << "╚";
    for (int i = 0; i < w - 2; i++) std::cout << "═";
    std::cout << "╝" << RESET << "\n";
}

void midLine(int w, const std::string& clr) {
    if (!clr.empty()) std::cout << clr;
    std::cout << "╠";
    for (int i = 0; i < w - 2; i++) std::cout << "═";
    std::cout << "╣" << RESET << "\n";
}

static int visLen(const std::string& s) {
    int len = 0;
    bool esc = false;
    for (char c : s) {
        if (c == '\033') { esc = true; continue; }
        if (esc)         { if (c == 'm') esc = false; continue; }
        len++;
    }
    return len;
}
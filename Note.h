#pragma once

#include <iostream>
#include <string>
#include <vector>

// тут чистка экрана для винды
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define CLEAR "cls"
#else
// а тут для линукса
#include <termios.h>
#include <unistd.h>
#define CLEAR "clear"
#endif
// тут стилизация текста анси 
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
// блок для цвета текста 
#define FG_BLACK    "\033[30m"
#define FG_RED      "\033[31m"
#define FG_GREEN    "\033[32m"
#define FG_YELLOW   "\033[33m"
#define FG_BLUE     "\033[34m"
#define FG_MAGENTA  "\033[35m"
#define FG_CYAN     "\033[36m"
#define FG_WHITE    "\033[37m"
// тут яркие цвета текста чтобы когда по кнопкам перемещался цвет ярче выделялся 
#define FG_BRED     "\033[91m"
#define FG_BGREEN   "\033[92m"
#define FG_BYELLOW  "\033[93m"
#define FG_BBLUE    "\033[94m"
#define FG_BMAGENTA "\033[95m"
#define FG_BCYAN    "\033[96m"
#define FG_BWHITE   "\033[97m"
// тут цвета фона 
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_DARK     "\033[40m"
#define BG_GRAY     "\033[100m"
// коды клавишь до 255 код обычное если больше то спец клавиши
enum Key {
    KEY_UP        = 1000,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER     = 13,
    KEY_ESC       = 27,
    KEY_BACKSPACE = 127,
    KEY_CTRL_S    = 19,
    KEY_TAB       = 9,
    KEY_HOME,
    KEY_END,
    KEY_PGUP,
    KEY_PGDN,
};
// тут структура для цветовых тегов последовательность 
struct Tag {
    std::string name;
    std::string color;
    std::string bg;
    std::string icon;
};
// глобальный вектор для доступных тегов 
extern const std::vector<Tag> TAGS;
// основная структура заметки 
struct Note {
    std::string title;
    std::string content;
    int         tagIndex  = 0;
    bool        pinned    = false;
    std::string createdAt;
    std::string updatedAt;
};

// тут уже ввод данных 
int         readKey();
std::string lineInput(const std::string& prompt, int maxLen = 100);
// тут что то типо графики на деле просто текстовая отрисовка интерфейса 
void clearScreen();
void topLine (int w, const std::string& clr = ""); //верхняя граница рамки
void botLine (int w, const std::string& clr = ""); // нижняя
void midLine (int w, const std::string& clr = ""); // разделительная линия 
void row     (int w, const std::string& content, // тут строка с контентом центрированный текст 
              const std::string& borderClr = "",
              const std::string& textClr   = "");
void titleRow(int w, const std::string& text, //отрисовка заголовка
              const std::string& borderClr = "",
              const std::string& textClr   = "");

// генерация строки визуально бейджика где цветной блок с именем 
std::string tagBadge(int idx);

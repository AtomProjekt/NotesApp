#include "NoteManager.h"

int main() {
// блок настройки терминала 
#ifdef _WIN32
// получение окна консоли 
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    // тут анси последовательность для текста цветов и стилей 
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    // ютф 8 для текста и смайликов (смайлики пришлось удалить тк текст сьезжал с рамки и было криво и некрасиво )
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::vector<Note> notes = loadNotes();
// цикл бесконечный для работы программы 
    while (true) {
        int choice = mainMenu(notes);
        switch (choice) {
            case 0: writeNote(notes);   break; //новая заметка 
            case 1: editNote(notes);    break; // редактирование существующей заметки 
            case 2: deleteNote(notes);  break; //удаление заметки 
            case 3: searchNotes(notes); break; // поиск заметки 
            case 4: { // выход из программы 
                clearScreen();
                int W = 44;
                std::cout << "\n";
                topLine(W, FG_BCYAN);
                titleRow(W, std::string(BOLD) + FG_BCYAN + " ❤︎ Goodbye! ", FG_BCYAN); //прощание с пользователем 
                botLine(W, FG_BCYAN);
                std::cout << "\n";
                return 0; //завершение мейна 
            }
        }
    }
}

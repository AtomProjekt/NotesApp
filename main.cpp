<<<<<<< HEAD
#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Метаданные приложения
    app.setApplicationName("NotesApp");
    app.setOrganizationName("NotesApp");
    app.setApplicationVersion("1.0.0");

    // Иконка из ресурсов
    app.setWindowIcon(QIcon(":/logo/myicon.ico"));

    // Сглаживание шрифтов
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();

    return app.exec();
=======
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
>>>>>>> 67b1d302a7d7a4982757a2e02559596618cbfe43
}

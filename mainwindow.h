#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QVector>
#include <QDateTime>
#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include <QDialog>

//  Структура заметки 
struct Note {
    QString title;
    QString content;
    int     tagIndex  = 0;
    bool    pinned    = false;
    QString createdAt;
    QString updatedAt;
};

//  Тег с цветом 
struct Tag {
    QString name;
    QColor  color;
    QString emoji;
};

//  Кастомный элемент списка 
class NoteListItem : public QWidget {
    Q_OBJECT
public:
    explicit NoteListItem(const Note &note, const Tag &tag, QWidget *parent = nullptr);
    void updateData(const Note &note, const Tag &tag);

private:
    QLabel *avatarLabel;
    QLabel *titleLabel;
    QLabel *previewLabel;
    QLabel *timeLabel;
    QLabel *pinLabel;
    QLabel *tagBadge;
};

//  Диалог создания/редактирования заметки 
class NoteDialog : public QDialog {
    Q_OBJECT
public:
    explicit NoteDialog(const QVector<Tag> &tags, QWidget *parent = nullptr,
                        const Note *existing = nullptr);

    QString getTitle()   const;
    QString getContent() const;
    int     getTagIdx()  const;
    bool    getPinned()  const;

private:
    QLineEdit  *titleEdit;
    QTextEdit  *contentEdit;
    QComboBox  *tagCombo;
    QPushButton *pinBtn;
    bool        pinnedState = false;

    void applyStyles(const QVector<Tag> &tags);
};

//  Главное окно 
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNoteSelected(int row);
    void onNewNote();
    void onEditNote();
    void onDeleteNote();
    void onTogglePin();
    void onSearch(const QString &query);
    void onContextMenu(const QPoint &pos);

private:
    //  данные
    QVector<Note> notes;
    QVector<Tag>  tags;
    int           currentIndex = -1;

    // левая панель
    QWidget       *leftPanel;
    QLineEdit     *searchBar;
    QListWidget   *noteList;
    QPushButton   *newNoteBtn;

    // правая панель
    QWidget       *rightPanel;
    QLabel        *chatTitle;
    QLabel        *chatSubtitle;
    QLabel        *tagIndicator;
    QScrollArea   *contentScroll;
    QLabel        *contentLabel;
    QLabel        *metaLabel;
    QWidget       *emptyState;

    // топ-бар
    QWidget       *topBar;

    // методы
    void initTags();
    void buildUI();
    void applyTheme();
    void loadNotes();
    void saveNotes();
    void refreshList(const QString &filter = "");
    void showNote(int index);
    void showEmpty();
    QString currentTime() const;
    QString notePreview(const Note &n) const;
    QColor  tagColor(int idx) const;
};

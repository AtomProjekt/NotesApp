#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QScreen>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QTimer>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// NoteListItem
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
NoteListItem::NoteListItem(const Note &note, const Tag &tag, QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(72);
    setObjectName("noteItem");

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(12, 8, 12, 8);
    root->setSpacing(12);

    // Аватар — цветной круг с первой буквой
    avatarLabel = new QLabel(this);
    avatarLabel->setFixedSize(46, 46);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setObjectName("avatar");

    // Текстовый блок
    auto *textCol = new QVBoxLayout();
    textCol->setSpacing(3);

    auto *topRow = new QHBoxLayout();
    topRow->setSpacing(6);

    titleLabel = new QLabel(this);
    titleLabel->setObjectName("itemTitle");
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    timeLabel = new QLabel(this);
    timeLabel->setObjectName("itemTime");
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    topRow->addWidget(titleLabel);
    topRow->addWidget(timeLabel);

    auto *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(6);

    previewLabel = new QLabel(this);
    previewLabel->setObjectName("itemPreview");
    previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    pinLabel = new QLabel("📌", this);
    pinLabel->setObjectName("pinLabel");
    pinLabel->setFixedWidth(20);

    tagBadge = new QLabel(this);
    tagBadge->setObjectName("tagBadge");
    tagBadge->setAlignment(Qt::AlignCenter);

    bottomRow->addWidget(previewLabel);
    bottomRow->addWidget(pinLabel);
    bottomRow->addWidget(tagBadge);

    textCol->addLayout(topRow);
    textCol->addLayout(bottomRow);

    root->addWidget(avatarLabel);
    root->addLayout(textCol);

    updateData(note, tag);
}

void NoteListItem::updateData(const Note &note, const Tag &tag)
{
    // Аватар
    QString firstChar = note.title.isEmpty() ? QString("?") : QString(note.title.at(0).toUpper());
    QPixmap pix(46, 46);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(tag.color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(0, 0, 46, 46);
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(18);
    f.setBold(true);
    p.setFont(f);
    p.drawText(QRect(0, 0, 46, 46), Qt::AlignCenter, firstChar);
    p.end();
    avatarLabel->setPixmap(pix);

    // Заголовок
    QString displayTitle = note.title.isEmpty() ? "Без названия" : note.title;
    if (displayTitle.length() > 24) displayTitle = displayTitle.left(21) + "…";
    titleLabel->setText(displayTitle);

    // Время
    QString t = note.updatedAt.isEmpty() ? note.createdAt : note.updatedAt;
    if (t.length() >= 16) t = t.right(5); // HH:MM
    timeLabel->setText(t);

    // Превью
    QString preview = note.content.simplified();
    if (preview.length() > 45) preview = preview.left(42) + "…";
    if (preview.isEmpty()) preview = "Пустая заметка";
    previewLabel->setText(preview);

    // Пин
    pinLabel->setVisible(note.pinned);

    // Тег
    tagBadge->setText(tag.name);
    tagBadge->setStyleSheet(QString(
        "background: %1; color: white; border-radius: 8px;"
        "padding: 1px 7px; font-size: 10px; font-weight: bold;"
    ).arg(tag.color.name()));
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// NoteDialog
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
NoteDialog::NoteDialog(const QVector<Tag> &tags, QWidget *parent, const Note *existing)
    : QDialog(parent)
{
    setModal(true);
    setMinimumSize(520, 460);
    setWindowTitle(existing ? "Редактировать заметку" : "Новая заметка");
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Карточка
    auto *card = new QWidget(this);
    card->setObjectName("dialogCard");
    root->addWidget(card);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // Заголовок диалога
    auto *hdr = new QHBoxLayout();
    auto *dlgTitle = new QLabel(existing ? "✏️  Редактировать" : "✏️  Новая заметка", card);
    dlgTitle->setObjectName("dialogTitle");
    auto *closeBtn = new QPushButton("✕", card);
    closeBtn->setObjectName("dialogClose");
    closeBtn->setFixedSize(28, 28);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    hdr->addWidget(dlgTitle);
    hdr->addStretch();
    hdr->addWidget(closeBtn);
    layout->addLayout(hdr);

    // Заголовок заметки
    auto *lbTitle = new QLabel("Заголовок", card);
    lbTitle->setObjectName("fieldLabel");
    layout->addWidget(lbTitle);

    titleEdit = new QLineEdit(card);
    titleEdit->setObjectName("dialogField");
    titleEdit->setPlaceholderText("Введите заголовок…");
    titleEdit->setMaxLength(60);
    if (existing) titleEdit->setText(existing->title);
    layout->addWidget(titleEdit);

    // Тег
    auto *lbTag = new QLabel("Категория", card);
    lbTag->setObjectName("fieldLabel");
    layout->addWidget(lbTag);

    tagCombo = new QComboBox(card);
    tagCombo->setObjectName("dialogCombo");
    for (const auto &t : tags)
        tagCombo->addItem(t.emoji + "  " + t.name);
    if (existing) tagCombo->setCurrentIndex(existing->tagIndex);
    layout->addWidget(tagCombo);

    // Содержимое
    auto *lbContent = new QLabel("Содержимое", card);
    lbContent->setObjectName("fieldLabel");
    layout->addWidget(lbContent);

    contentEdit = new QTextEdit(card);
    contentEdit->setObjectName("dialogTextArea");
    contentEdit->setPlaceholderText("Введите текст заметки…");
    contentEdit->setMinimumHeight(160);
    if (existing) contentEdit->setPlainText(existing->content);
    layout->addWidget(contentEdit);

    // Пин + кнопки
    auto *footer = new QHBoxLayout();

    pinBtn = new QPushButton("📌  Закрепить", card);
    pinBtn->setObjectName("pinToggle");
    pinBtn->setCheckable(true);
    pinnedState = existing ? existing->pinned : false;
    pinBtn->setChecked(pinnedState);
    pinBtn->setText(pinnedState ? "📌  Закреплено" : "📌  Закрепить");
    connect(pinBtn, &QPushButton::toggled, [this](bool on){
        pinnedState = on;
        pinBtn->setText(on ? "📌  Закреплено" : "📌  Закрепить");
    });

    auto *saveBtn = new QPushButton(existing ? "Сохранить" : "Создать", card);
    saveBtn->setObjectName("primaryBtn");
    connect(saveBtn, &QPushButton::clicked, this, &QDialog::accept);

    auto *cancelBtn = new QPushButton("Отмена", card);
    cancelBtn->setObjectName("secondaryBtn");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    footer->addWidget(pinBtn);
    footer->addStretch();
    footer->addWidget(cancelBtn);
    footer->addWidget(saveBtn);
    layout->addLayout(footer);

    applyStyles(tags);
}

void NoteDialog::applyStyles(const QVector<Tag> &)
{
    setStyleSheet(R"(
        QWidget#dialogCard {
            background: #1c2733;
            border-radius: 16px;
            border: 1px solid #2b3847;
        }
        QLabel#dialogTitle {
            color: #ffffff;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton#dialogClose {
            background: #2b3847;
            color: #8da2b5;
            border: none;
            border-radius: 14px;
            font-size: 14px;
        }
        QPushButton#dialogClose:hover { background: #e53935; color: white; }

        QLabel#fieldLabel {
            color: #8da2b5;
            font-size: 12px;
            font-weight: bold;
            letter-spacing: 0.5px;
            text-transform: uppercase;
        }
        QLineEdit#dialogField, QTextEdit#dialogTextArea {
            background: #131f2b;
            border: 1.5px solid #2b3847;
            border-radius: 10px;
            color: #e8f0f7;
            font-size: 14px;
            padding: 10px 14px;
            selection-background-color: #2b5278;
        }
        QLineEdit#dialogField:focus, QTextEdit#dialogTextArea:focus {
            border-color: #5288c1;
        }
        QComboBox#dialogCombo {
            background: #131f2b;
            border: 1.5px solid #2b3847;
            border-radius: 10px;
            color: #e8f0f7;
            font-size: 14px;
            padding: 8px 14px;
        }
        QComboBox#dialogCombo::drop-down { border: none; width: 24px; }
        QComboBox#dialogCombo:focus { border-color: #5288c1; }
        QComboBox QAbstractItemView {
            background: #1c2733;
            color: #e8f0f7;
            border: 1px solid #2b3847;
            selection-background-color: #2b5278;
        }
        QPushButton#primaryBtn {
            background: #5288c1;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 28px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton#primaryBtn:hover   { background: #4a7ab0; }
        QPushButton#primaryBtn:pressed { background: #3d6a9e; }

        QPushButton#secondaryBtn {
            background: #2b3847;
            color: #8da2b5;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton#secondaryBtn:hover { background: #364657; color: #e8f0f7; }

        QPushButton#pinToggle {
            background: transparent;
            color: #8da2b5;
            border: 1.5px solid #2b3847;
            border-radius: 10px;
            padding: 8px 16px;
            font-size: 13px;
        }
        QPushButton#pinToggle:checked {
            background: #2b3847;
            color: #f5c542;
            border-color: #f5c542;
        }
        QPushButton#pinToggle:hover { border-color: #5288c1; }
    )");
}

QString NoteDialog::getTitle()   const { return titleEdit->text().trimmed(); }
QString NoteDialog::getContent() const { return contentEdit->toPlainText(); }
int     NoteDialog::getTagIdx()  const { return tagCombo->currentIndex(); }
bool    NoteDialog::getPinned()  const { return pinnedState; }

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// MainWindow
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    initTags();
    loadNotes();
    buildUI();
    applyTheme();
    refreshList();

    setWindowTitle("Notes App");
    resize(1100, 700);
    setMinimumSize(800, 550);

    // Центрировать на экране
    if (auto *scr = QApplication::primaryScreen()) {
        auto geo = scr->availableGeometry();
        move(geo.center() - rect().center());
    }

    // Шорткат Ctrl+N — новая заметка
    auto *sc = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(sc, &QShortcut::activated, this, &MainWindow::onNewNote);
}

MainWindow::~MainWindow() { saveNotes(); }

// ── Теги ────────────────────────────────────────────────────────────────
void MainWindow::initTags()
{
    tags = {
        {"Работа",     QColor("#5288c1"), "💼"},
        {"Личное",     QColor("#4caf80"), "🌿"},
        {"Срочно",     QColor("#e53935"), "🔥"},
        {"Идеи",       QColor("#f5a623"), "💡"},
        {"Учёба",      QColor("#9c5fd4"), "📚"},
        {"Другое",     QColor("#26a5a4"), "📌"},
    };
}

// ── Построение UI ───────────────────────────────────────────────────────
void MainWindow::buildUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ════════════════════════════════════════
    // ЛЕВАЯ ПАНЕЛЬ (список заметок)
    // ════════════════════════════════════════
    leftPanel = new QWidget(central);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(300);

    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // Топ-бар левой панели
    auto *leftTop = new QWidget(leftPanel);
    leftTop->setObjectName("leftTopBar");
    leftTop->setFixedHeight(60);

    auto *ltLayout = new QHBoxLayout(leftTop);
    ltLayout->setContentsMargins(16, 0, 16, 0);

    auto *appIcon = new QLabel("📓", leftTop);
    appIcon->setObjectName("appIcon");
    auto *appTitle = new QLabel("Notes", leftTop);
    appTitle->setObjectName("appTitle");

    ltLayout->addWidget(appIcon);
    ltLayout->addWidget(appTitle);
    ltLayout->addStretch();

    newNoteBtn = new QPushButton("+", leftTop);
    newNoteBtn->setObjectName("newNoteBtn");
    newNoteBtn->setFixedSize(36, 36);
    newNoteBtn->setToolTip("Новая заметка (Ctrl+N)");
    connect(newNoteBtn, &QPushButton::clicked, this, &MainWindow::onNewNote);
    ltLayout->addWidget(newNoteBtn);

    // Поиск
    auto *searchWrap = new QWidget(leftPanel);
    searchWrap->setObjectName("searchWrap");
    searchWrap->setFixedHeight(52);

    auto *swLayout = new QHBoxLayout(searchWrap);
    swLayout->setContentsMargins(12, 8, 12, 8);

    searchBar = new QLineEdit(searchWrap);
    searchBar->setObjectName("searchBar");
    searchBar->setPlaceholderText("🔍  Поиск заметок…");
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearch);
    swLayout->addWidget(searchBar);

    // Список
    noteList = new QListWidget(leftPanel);
    noteList->setObjectName("noteList");
    noteList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    noteList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(noteList, &QListWidget::currentRowChanged, this, &MainWindow::onNoteSelected);
    connect(noteList, &QListWidget::customContextMenuRequested, this, &MainWindow::onContextMenu);

    leftLayout->addWidget(leftTop);
    leftLayout->addWidget(searchWrap);
    leftLayout->addWidget(noteList, 1);

    // ════════════════════════════════════════
    // ПРАВАЯ ПАНЕЛЬ (просмотр заметки)
    // ════════════════════════════════════════
    rightPanel = new QWidget(central);
    rightPanel->setObjectName("rightPanel");

    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Топ-бар правой панели
    topBar = new QWidget(rightPanel);
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(60);

    auto *tbLayout = new QHBoxLayout(topBar);
    tbLayout->setContentsMargins(20, 0, 20, 0);
    tbLayout->setSpacing(12);

    chatTitle = new QLabel("Выберите заметку", topBar);
    chatTitle->setObjectName("chatTitle");

    chatSubtitle = new QLabel("", topBar);
    chatSubtitle->setObjectName("chatSubtitle");

    auto *titleCol = new QVBoxLayout();
    titleCol->setSpacing(2);
    titleCol->addWidget(chatTitle);
    titleCol->addWidget(chatSubtitle);

    tagIndicator = new QLabel(topBar);
    tagIndicator->setObjectName("tagIndicator");

    auto *editBtn = new QPushButton("✏️", topBar);
    editBtn->setObjectName("iconBtn");
    editBtn->setFixedSize(36, 36);
    editBtn->setToolTip("Редактировать");
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditNote);

    auto *pinBtn2 = new QPushButton("📌", topBar);
    pinBtn2->setObjectName("iconBtn");
    pinBtn2->setFixedSize(36, 36);
    pinBtn2->setToolTip("Закрепить/открепить");
    connect(pinBtn2, &QPushButton::clicked, this, &MainWindow::onTogglePin);

    auto *delBtn = new QPushButton("🗑", topBar);
    delBtn->setObjectName("iconBtnDanger");
    delBtn->setFixedSize(36, 36);
    delBtn->setToolTip("Удалить");
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteNote);

    tbLayout->addLayout(titleCol);
    tbLayout->addStretch();
    tbLayout->addWidget(tagIndicator);
    tbLayout->addWidget(editBtn);
    tbLayout->addWidget(pinBtn2);
    tbLayout->addWidget(delBtn);

    // Стек: пустое состояние ↔ содержимое
    auto *stack = new QStackedWidget(rightPanel);
    stack->setObjectName("contentStack");

    // — Пустое состояние
    emptyState = new QWidget(stack);
    auto *emptyLayout = new QVBoxLayout(emptyState);
    emptyLayout->setAlignment(Qt::AlignCenter);

    auto *emptyIcon = new QLabel("📓", emptyState);
    emptyIcon->setAlignment(Qt::AlignCenter);
    emptyIcon->setObjectName("emptyIcon");

    auto *emptyText = new QLabel("Выберите заметку\nили создайте новую", emptyState);
    emptyText->setAlignment(Qt::AlignCenter);
    emptyText->setObjectName("emptyText");

    auto *emptyHint = new QPushButton("+ Новая заметка", emptyState);
    emptyHint->setObjectName("primaryBtn");
    emptyHint->setFixedWidth(180);
    connect(emptyHint, &QPushButton::clicked, this, &MainWindow::onNewNote);

    emptyLayout->addWidget(emptyIcon);
    emptyLayout->addWidget(emptyText);
    emptyLayout->addSpacing(20);
    emptyLayout->addWidget(emptyHint, 0, Qt::AlignCenter);

    // — Содержимое заметки (пузырь как в Telegram)
    contentScroll = new QScrollArea(stack);
    contentScroll->setObjectName("contentScroll");
    contentScroll->setWidgetResizable(true);
    contentScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *bubbleWrap = new QWidget(contentScroll);
    bubbleWrap->setObjectName("bubbleWrap");
    auto *bwLayout = new QVBoxLayout(bubbleWrap);
    bwLayout->setContentsMargins(40, 30, 40, 30);
    bwLayout->setSpacing(10);

    contentLabel = new QLabel(bubbleWrap);
    contentLabel->setObjectName("contentBubble");
    contentLabel->setWordWrap(true);
    contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    metaLabel = new QLabel(bubbleWrap);
    metaLabel->setObjectName("metaLabel");
    metaLabel->setAlignment(Qt::AlignRight);

    bwLayout->addWidget(contentLabel);
    bwLayout->addWidget(metaLabel);
    bwLayout->addStretch();

    contentScroll->setWidget(bubbleWrap);

    stack->addWidget(emptyState);
    stack->addWidget(contentScroll);
    stack->setCurrentWidget(emptyState);

    // Сохраним ссылку на стек в rightPanel
    rightPanel->setProperty("stack", QVariant::fromValue(static_cast<QObject*>(stack)));

    rightLayout->addWidget(topBar);
    rightLayout->addWidget(stack, 1);

    // ════════════════════════════════════════
    // Разделитель
    // ════════════════════════════════════════
    auto *divider = new QFrame(central);
    divider->setObjectName("divider");
    divider->setFrameShape(QFrame::VLine);
    divider->setFixedWidth(1);

    rootLayout->addWidget(leftPanel);
    rootLayout->addWidget(divider);
    rootLayout->addWidget(rightPanel, 1);
}

// ── Тема ────────────────────────────────────────────────────────────────
void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        /* ── Фоны ──────────────────────────────────── */
        QMainWindow, QWidget { background: #17212b; color: #e8f0f7; }

        QWidget#leftPanel  { background: #17212b; }
        QWidget#rightPanel { background: #0d1117; }

        /* ── Левый топ-бар ──────────────────────────── */
        QWidget#leftTopBar {
            background: #1c2733;
            border-bottom: 1px solid #2b3847;
        }
        QLabel#appIcon { font-size: 22px; }
        QLabel#appTitle {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            letter-spacing: 0.5px;
        }
        QPushButton#newNoteBtn {
            background: #5288c1;
            color: white;
            border: none;
            border-radius: 18px;
            font-size: 22px;
            font-weight: bold;
        }
        QPushButton#newNoteBtn:hover   { background: #4a7ab0; }
        QPushButton#newNoteBtn:pressed { background: #3d6a9e; }

        /* ── Поиск ──────────────────────────────────── */
        QWidget#searchWrap { background: #1c2733; border-bottom: 1px solid #2b3847; }
        QLineEdit#searchBar {
            background: #131f2b;
            border: none;
            border-radius: 18px;
            color: #e8f0f7;
            font-size: 14px;
            padding: 6px 16px;
        }
        QLineEdit#searchBar:focus { border: 1.5px solid #5288c1; }

        /* ── Список заметок ────────────────────────── */
        QListWidget#noteList {
            background: #17212b;
            border: none;
            outline: none;
        }
        QListWidget#noteList::item { border: none; padding: 0; }
        QListWidget#noteList::item:selected {
            background: #2b3847;
            border-left: 3px solid #5288c1;
        }
        QListWidget#noteList::item:hover:!selected { background: #1e2d3d; }

        /* ── Элемент списка ────────────────────────── */
        QWidget#noteItem { background: transparent; }
        QLabel#itemTitle {
            color: #ffffff;
            font-size: 14px;
            font-weight: bold;
        }
        QLabel#itemPreview { color: #8da2b5; font-size: 12px; }
        QLabel#itemTime    { color: #8da2b5; font-size: 11px; }

        /* ── Разделитель ────────────────────────────── */
        QFrame#divider { background: #2b3847; }

        /* ── Правый топ-бар ─────────────────────────── */
        QWidget#topBar {
            background: #1c2733;
            border-bottom: 1px solid #2b3847;
        }
        QLabel#chatTitle {
            color: #ffffff;
            font-size: 15px;
            font-weight: bold;
        }
        QLabel#chatSubtitle { color: #8da2b5; font-size: 12px; }
        QLabel#tagIndicator {
            border-radius: 10px;
            padding: 3px 12px;
            font-size: 12px;
            font-weight: bold;
            color: white;
        }
        QPushButton#iconBtn {
            background: transparent;
            border: none;
            border-radius: 18px;
            font-size: 18px;
        }
        QPushButton#iconBtn:hover { background: #2b3847; }
        QPushButton#iconBtnDanger {
            background: transparent;
            border: none;
            border-radius: 18px;
            font-size: 18px;
        }
        QPushButton#iconBtnDanger:hover { background: #4a1515; }

        /* ── Контент ────────────────────────────────── */
        QStackedWidget#contentStack { background: #0d1117; }
        QScrollArea#contentScroll   { background: #0d1117; border: none; }
        QWidget#bubbleWrap          { background: #0d1117; }

        QLabel#contentBubble {
            background: #1c2733;
            border-radius: 16px;
            border-bottom-left-radius: 4px;
            color: #e8f0f7;
            font-size: 15px;
            line-height: 1.6;
            padding: 18px 22px;
        }
        QLabel#metaLabel {
            color: #5d7a93;
            font-size: 11px;
        }

        /* ── Пустое состояние ───────────────────────── */
        QLabel#emptyIcon { font-size: 64px; }
        QLabel#emptyText {
            color: #5d7a93;
            font-size: 16px;
            line-height: 1.6;
        }

        /* ── Общая кнопка ───────────────────────────── */
        QPushButton#primaryBtn {
            background: #5288c1;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 24px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton#primaryBtn:hover   { background: #4a7ab0; }
        QPushButton#primaryBtn:pressed { background: #3d6a9e; }

        /* ── Скроллбары ─────────────────────────────── */
        QScrollBar:vertical {
            background: transparent; width: 6px; margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #2b3847; border-radius: 3px; min-height: 30px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }

        /* ── Контекстное меню ───────────────────────── */
        QMenu {
            background: #1c2733;
            border: 1px solid #2b3847;
            border-radius: 10px;
            padding: 6px;
            color: #e8f0f7;
        }
        QMenu::item { padding: 8px 20px; border-radius: 6px; font-size: 13px; }
        QMenu::item:selected { background: #2b5278; }
        QMenu::separator { background: #2b3847; height: 1px; margin: 4px 8px; }
    )");
}

// ── Утилиты ─────────────────────────────────────────────────────────────
QString MainWindow::currentTime() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
}

QString MainWindow::notePreview(const Note &n) const
{
    QString p = n.content.simplified();
    return p.length() > 60 ? p.left(57) + "…" : p;
}

QColor MainWindow::tagColor(int idx) const
{
    if (idx < 0 || idx >= tags.size()) return QColor("#5288c1");
    return tags[idx].color;
}

// ── Обновление списка ────────────────────────────────────────────────────
void MainWindow::refreshList(const QString &filter)
{
    noteList->clear();

    // Сначала закреплённые, потом остальные
    QVector<int> order;
    for (int i = 0; i < notes.size(); i++) if (notes[i].pinned)  order.append(i);
    for (int i = 0; i < notes.size(); i++) if (!notes[i].pinned) order.append(i);

    QString lf = filter.toLower();

    for (int idx : order) {
        const Note &n = notes[idx];
        if (!lf.isEmpty()) {
            bool titleMatch = n.title.toLower().contains(lf);
            bool tagMatch   = tags[n.tagIndex].name.toLower().contains(lf);
            bool contentMatch = n.content.toLower().contains(lf);
            if (!titleMatch && !tagMatch && !contentMatch) continue;
        }

        auto *item   = new QListWidgetItem(noteList);
        auto *widget = new NoteListItem(n, tags[n.tagIndex], noteList);
        item->setData(Qt::UserRole, idx); // сохраняем реальный индекс
        item->setSizeHint(widget->sizeHint());
        noteList->setItemWidget(item, widget);
    }

    if (noteList->count() > 0)
        noteList->setCurrentRow(0);
    else
        showEmpty();
}

// ── Показать заметку ─────────────────────────────────────────────────────
void MainWindow::showNote(int index)
{
    if (index < 0 || index >= notes.size()) { showEmpty(); return; }
    const Note &n = notes[index];
    currentIndex  = index;

    // Топ-бар
    chatTitle->setText(n.title.isEmpty() ? "Без названия" : n.title);

    QString sub = n.updatedAt.isEmpty()
        ? "Создано: " + n.createdAt
        : "Изменено: " + n.updatedAt;
    if (n.pinned) sub = "📌 Закреплено  ·  " + sub;
    chatSubtitle->setText(sub);

    // Тег-индикатор
    const Tag &t = tags[n.tagIndex];
    tagIndicator->setText(t.emoji + "  " + t.name);
    tagIndicator->setStyleSheet(QString(
        "background: %1; border-radius: 10px; padding: 3px 12px;"
        "font-size: 12px; font-weight: bold; color: white;"
    ).arg(t.color.name()));

    // Контент
    QString html = n.content.toHtmlEscaped().replace("\n", "<br>");
    contentLabel->setText("<html><body>" + html + "</body></html>");

    // Мета
    QString meta = "Создано: " + n.createdAt;
    if (!n.updatedAt.isEmpty() && n.updatedAt != n.createdAt)
        meta += "  ·  Изменено: " + n.updatedAt;
    metaLabel->setText(meta);

    // Бордер пузыря в цвет тега
    contentLabel->setStyleSheet(QString(
        "background: #1c2733;"
        "border-radius: 16px;"
        "border-bottom-left-radius: 4px;"
        "border-left: 3px solid %1;"
        "color: #e8f0f7;"
        "font-size: 15px;"
        "padding: 18px 22px;"
    ).arg(t.color.name()));

    // Показываем стек с контентом
    auto *stack = qobject_cast<QStackedWidget*>(
        rightPanel->property("stack").value<QObject*>()
    );
    if (stack) stack->setCurrentWidget(contentScroll);
}

void MainWindow::showEmpty()
{
    currentIndex = -1;
    chatTitle->setText("Выберите заметку");
    chatSubtitle->setText("");
    tagIndicator->setText("");

    auto *stack = qobject_cast<QStackedWidget*>(
        rightPanel->property("stack").value<QObject*>()
    );
    if (stack) stack->setCurrentWidget(emptyState);
}

// ── Слоты ───────────────────────────────────────────────────────────────
void MainWindow::onNoteSelected(int row)
{
    if (row < 0) return;
    auto *item = noteList->item(row);
    if (!item) return;
    int idx = item->data(Qt::UserRole).toInt();
    showNote(idx);
}

void MainWindow::onNewNote()
{
    NoteDialog dlg(tags, this);
    if (dlg.exec() != QDialog::Accepted) return;

    Note n;
    n.title     = dlg.getTitle().isEmpty() ? "Без названия" : dlg.getTitle();
    n.content   = dlg.getContent();
    n.tagIndex  = dlg.getTagIdx();
    n.pinned    = dlg.getPinned();
    n.createdAt = currentTime();
    n.updatedAt = currentTime();
    notes.prepend(n);
    saveNotes();
    refreshList(searchBar->text());
}

void MainWindow::onEditNote()
{
    if (currentIndex < 0 || currentIndex >= notes.size()) return;
    Note &n = notes[currentIndex];

    NoteDialog dlg(tags, this, &n);
    if (dlg.exec() != QDialog::Accepted) return;

    n.title     = dlg.getTitle().isEmpty() ? "Без названия" : dlg.getTitle();
    n.content   = dlg.getContent();
    n.tagIndex  = dlg.getTagIdx();
    n.pinned    = dlg.getPinned();
    n.updatedAt = currentTime();
    saveNotes();
    refreshList(searchBar->text());
    showNote(currentIndex);
}

void MainWindow::onDeleteNote()
{
    if (currentIndex < 0 || currentIndex >= notes.size()) return;

    QMessageBox mb(this);
    mb.setWindowTitle("Удалить заметку");
    mb.setText("Удалить «" + notes[currentIndex].title + "»?");
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mb.setDefaultButton(QMessageBox::No);
    mb.setStyleSheet(R"(
        QMessageBox { background: #1c2733; color: #e8f0f7; }
        QPushButton {
            background: #2b3847; color: #e8f0f7;
            border: none; border-radius: 8px; padding: 8px 20px;
        }
        QPushButton:default { background: #e53935; color: white; }
    )");
    if (mb.exec() != QMessageBox::Yes) return;

    notes.removeAt(currentIndex);
    saveNotes();
    currentIndex = -1;
    refreshList(searchBar->text());
}

void MainWindow::onTogglePin()
{
    if (currentIndex < 0 || currentIndex >= notes.size()) return;
    notes[currentIndex].pinned = !notes[currentIndex].pinned;
    notes[currentIndex].updatedAt = currentTime();
    saveNotes();
    refreshList(searchBar->text());
    showNote(currentIndex);
}

void MainWindow::onSearch(const QString &query)
{
    refreshList(query);
}

void MainWindow::onContextMenu(const QPoint &pos)
{
    auto *item = noteList->itemAt(pos);
    if (!item) return;
    int idx = item->data(Qt::UserRole).toInt();
    if (idx < 0 || idx >= notes.size()) return;

    currentIndex = idx;

    QMenu menu(this);
    menu.addAction("✏️  Редактировать", this, &MainWindow::onEditNote);
    menu.addSeparator();
    QString pinText = notes[idx].pinned ? "📌  Открепить" : "📌  Закрепить";
    menu.addAction(pinText, this, &MainWindow::onTogglePin);
    menu.addSeparator();
    menu.addAction("🗑  Удалить", this, &MainWindow::onDeleteNote);
    menu.exec(noteList->mapToGlobal(pos));
}

// ── Сохранение и загрузка ────────────────────────────────────────────────
void MainWindow::saveNotes()
{
    QString dir  = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    QString path = dir + "/notes.json";

    QJsonArray arr;
    for (const auto &n : notes) {
        QJsonObject o;
        o["title"]     = n.title;
        o["content"]   = n.content;
        o["tagIndex"]  = n.tagIndex;
        o["pinned"]    = n.pinned;
        o["createdAt"] = n.createdAt;
        o["updatedAt"] = n.updatedAt;
        arr.append(o);
    }
    QFile f(path);
    if (f.open(QIODevice::WriteOnly))
        f.write(QJsonDocument(arr).toJson());
}

void MainWindow::loadNotes()
{
    QString dir  = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/notes.json";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        // Миграция из старого формата .dat
        QString legacyPath = "notes_data/notes.dat";
        QFile lf(legacyPath);
        if (lf.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&lf);
            Note cur; bool inNote = false;
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line == "---") {
                    if (inNote) notes.append(cur);
                    cur = Note(); inNote = false;
                } else if (line.startsWith("TITLE:"))   { cur.title    = line.mid(6); inNote = true; }
                else if (line.startsWith("TAG:"))       { cur.tagIndex = line.mid(4).toInt(); }
                else if (line.startsWith("PINNED:"))    { cur.pinned   = (line.mid(7) == "1"); }
                else if (line.startsWith("CREATED:"))   { cur.createdAt = line.mid(8); }
                else if (line.startsWith("UPDATED:"))   { cur.updatedAt = line.mid(8); }
                else if (line.startsWith("CONTENT:")) {
                    QString enc = line.mid(8), dec;
                    for (int i = 0; i < enc.size(); i++) {
                        if (enc[i] == '\\' && i + 1 < enc.size() && enc[i+1] == 'n') { dec += '\n'; i++; }
                        else dec += enc[i];
                    }
                    cur.content = dec;
                }
            }
        }
        return;
    }
    auto doc = QJsonDocument::fromJson(f.readAll());
    for (const auto &v : doc.array()) {
        auto o = v.toObject();
        Note n;
        n.title     = o["title"].toString();
        n.content   = o["content"].toString();
        n.tagIndex  = o["tagIndex"].toInt();
        n.pinned    = o["pinned"].toBool();
        n.createdAt = o["createdAt"].toString();
        n.updatedAt = o["updatedAt"].toString();
        notes.append(n);
    }
}

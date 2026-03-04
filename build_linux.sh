#!/bin/bash
echo "=========================================="
echo "  NotesApp Qt Build Script (Linux/macOS)"
echo "=========================================="

# Проверяем наличие Qt
if ! command -v qmake &>/dev/null && ! command -v qmake5 &>/dev/null; then
    echo "[ОШИБКА] Qt не найден!"
    echo ""
    echo "Установка:"
    echo "  Ubuntu/Debian: sudo apt install qt5-qmake qtbase5-dev"
    echo "  Fedora:        sudo dnf install qt5-qtbase-devel"
    echo "  macOS:         brew install qt5"
    exit 1
fi

QMAKE=$(command -v qmake5 || command -v qmake)
echo "qmake: $QMAKE"

mkdir -p build && cd build

echo ""
echo "[1/3] Запуск qmake..."
$QMAKE ../NotesApp.pro CONFIG+=release

echo ""
echo "[2/3] Компиляция..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
if [ -f "NotesApp" ]; then
    echo "[3/3] Готово!"
    echo "=========================================="
    echo "  Бинарник: build/NotesApp"
    echo "=========================================="
    chmod +x NotesApp
else
    echo "[ОШИБКА] Сборка не удалась."
    exit 1
fi

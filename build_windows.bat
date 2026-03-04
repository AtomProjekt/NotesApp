@echo off
echo ==========================================
echo   NotesApp Qt Build Script (Windows)
echo ==========================================

:: Ищем Qt автоматически в стандартных путях
set QT_PATH=
for %%p in (
    "C:\Qt\6.7.0\msvc2019_64\bin"
    "C:\Qt\6.6.0\msvc2019_64\bin"
    "C:\Qt\5.15.2\msvc2019_64\bin"
    "C:\Qt\5.15.2\mingw81_64\bin"
    "C:\Qt\5.15.2\mingw_64\bin"
) do (
    if exist %%p\qmake.exe (
        set QT_PATH=%%~p
        goto :found
    )
)

:: Если не нашли — проверяем PATH
where qmake >nul 2>&1
if %errorlevel% == 0 (
    echo Qt найден в PATH
    goto :build
)

echo [ОШИБКА] Qt не найден!
echo Установи Qt с https://www.qt.io/download
echo Или отредактируй этот скрипт, указав путь вручную.
pause
exit /b 1

:found
echo Qt найден: %QT_PATH%
set PATH=%QT_PATH%;%PATH%

:build
mkdir build 2>nul
cd build

echo.
echo [1/3] Запуск qmake...
qmake ..\NotesApp.pro -spec win32-msvc CONFIG+=release
if %errorlevel% neq 0 (
    echo Попытка с mingw...
    qmake ..\NotesApp.pro -spec win32-g++ CONFIG+=release
)

echo.
echo [2/3] Компиляция (nmake / mingw32-make)...
nmake release 2>nul || mingw32-make release 2>nul || make release

echo.
echo [3/3] Деплой Qt DLL (windeployqt)...
if exist release\NotesApp.exe (
    windeployqt --release release\NotesApp.exe
    echo.
    echo ==========================================
    echo   ГОТОВО! Файл: build\release\NotesApp.exe
    echo ==========================================
) else (
    echo [ОШИБКА] NotesApp.exe не создан. Проверь вывод выше.
)

cd ..
pause

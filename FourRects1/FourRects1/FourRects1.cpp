#include <Windows.h>

#define COLS 3
#define ROWS 2

//====================================================

// Структура для хранения информации о прямоугольниках
struct RectInfo {
    RECT rect;
    bool isHovered = false;
    int rectWidth; // Ширина прямоугольника
    int rectHeight; // Высота прямоугольника
};

//====================================================

// Прототип функции окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//====================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    LPCTSTR szClass = TEXT("sdfghjkllkjhgf"); // Имя класса окна

    // Регистрация класса окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc; // Указывается обработчик сообщений для главного окна
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Заливка окна
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Курсор

    // Регистрация класса
    if (!::RegisterClass(&wc)) {
        return -1;
    }

    // Создание главного окна
    HWND hMainWindow = ::CreateWindow(szClass, "Главное окно", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hMainWindow) {
        return -1; // Если окно не удалось создать, то возвращает ошибку
    }

    // Показываем главное окно
    ::ShowWindow(hMainWindow, nCmdShow);

    MSG msg;
    // Цикл обработки сообщений
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::DispatchMessage(&msg); 
    }
    return 0;
}

//====================================================

void dividingClientArea(RECT clientRect, HDC hdc, RectInfo rects[]) {

    int width = clientRect.right - clientRect.left; // Ширина окна
    int height = clientRect.bottom - clientRect.top; // Высота окна

    rects->rectWidth = width / COLS; // Ширина прямоугольника
    rects->rectHeight = height / ROWS; // Высота прямоугольника

    for (int i = 0; i < (COLS * ROWS); ++i) {
        int x = (i % COLS) * rects->rectWidth;
        int y = (i / COLS) * rects->rectHeight;

        rects[i].rect = { x, y, x + rects->rectWidth, y + rects->rectHeight };
    }
}

void fillingRectangle(HDC hdc, RectInfo rects[]) {
    for (int i = 0; i < (COLS * ROWS); i++) {
        if (rects[i].isHovered) {
            HBRUSH hBrsh = ::CreateSolidBrush(RGB(222, 233, 205));
            FillRect(hdc, &rects[i].rect, hBrsh);
            DeleteObject(hBrsh);
        }
    }
}

void drawingGrid(HDC hdc, int rectWidth, int rectHeight, RECT clientRect) {
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oPen = (HPEN)SelectObject(hdc, hPen);

    for (int i = 0; i <= COLS; ++i)
    {
        MoveToEx(hdc, i * rectWidth, 0, NULL);
        LineTo(hdc, i * rectWidth, clientRect.bottom);
    }
    for (int i = 0; i <= ROWS; ++i)
    {
        MoveToEx(hdc, 0, i * rectHeight, NULL);
        LineTo(hdc, clientRect.right, i * rectHeight);
    }

    SelectObject(hdc, oPen);
    DeleteObject(hPen);
}

//====================================================

// Обработчик сообщений для окон
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    static RectInfo rects[COLS * ROWS];
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message) {

    case WM_SIZE:
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_NCMOUSEMOVE: {
        for (int i = 0; i < COLS * ROWS; i++) {
            rects[i].isHovered = false;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_MOUSEMOVE: {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        for (int i = 0; i < COLS * ROWS; i++) {
            if (PtInRect(&rects[i].rect, pt)) {
                rects[i].isHovered = true;
            }     
            else {
                rects[i].isHovered = false;
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }

    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        dividingClientArea(clientRect, hdc, rects);
        fillingRectangle(hdc, rects);
        drawingGrid(hdc, rects->rectWidth, rects->rectHeight, clientRect);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY: // Закрытие окна
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam); // Обработка других сообщений
}
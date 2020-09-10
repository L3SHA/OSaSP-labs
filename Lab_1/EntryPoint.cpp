#include<windows.h>
#include<stdlib.h>
#include<gdiplus.h>
#include<math.h>
#pragma comment(lib, "gdiplus.lib")

const double ROTATE_ANGLE = 0.005;
const int STEP_SIZE = 5;
const int MIN_WINDOW_WIDTH = 400;
const int MIN_WINDOW_HEIGHT = 400;

struct WindowSize {
    int height;
    int width;
} windowSize;

enum Directions {
    Up,
    Down,
    Left,
    Right,
    Positive,
    Negative
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void DoubleBufferPaint(HWND hwnd, HDC hdc, PAINTSTRUCT ps);
void Paint(HWND hwnd, HDC hdc, PAINTSTRUCT ps);
void SetStartCoordinates();
void RotateImage(float angle, Directions direction);
void MoveUp();
void MoveDown();
void MoveLeft();
void MoveRight();
void SetWindowSize(HWND hwnd);
bool IsSpriteInBorders(Directions direction);

Gdiplus::PointF coordinates[4];
UINT imageHeight;
UINT imageWidth;
float imageCenterX;
float imageCenterY;
bool isFirstCall = true;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"OSaSP Lab_1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPMINMAXINFO lpMMI;

    switch (uMsg)
    {
    case WM_CREATE:
    {

    }
    return 0;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    return 0;

    case WM_MOUSEWHEEL:
    {
        if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT) {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
                MoveRight();
            }
            else {
                MoveLeft();
            }
        }
        else {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
                MoveUp();
            }
            else {
                MoveDown();
            }
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_UP:
        {
            MoveUp();
        }
        break;
        case VK_DOWN:
        {
            MoveDown();
        }
        break;
        case VK_RIGHT:
        {
            MoveRight();
        }
        break;
        case VK_LEFT:
        {
            MoveLeft();
        }
        break;
        case VK_TAB:
        {
            RotateImage(ROTATE_ANGLE, Positive);
        }
        break;
        case VK_SPACE:
        {
            RotateImage(ROTATE_ANGLE, Negative);
        }
        }

        InvalidateRect(hwnd, NULL, FALSE);

    }
    return 0;

    case WM_ERASEBKGND:
        return (LRESULT)1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hwnd, &ps);
        DoubleBufferPaint(hwnd, hdc, ps);
        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_SIZE:
    {
        SetWindowSize(hwnd);
    }
    return 0;

    case WM_GETMINMAXINFO:
    {
        lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
        lpMMI->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
    }
    return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}

void RotateImage(float angle, Directions direction) {

    switch (direction)
    {
    case Positive:
    {
        if (IsSpriteInBorders(Positive)) {
            for (int i = 0; i < 4; i++) {
                coordinates[i].X = imageCenterX + ((double)coordinates[i].X - imageCenterX) * cos(angle) - ((double)coordinates[i].Y - imageCenterY) * sin(angle);
                coordinates[i].Y = imageCenterY + ((double)coordinates[i].X - imageCenterX) * sin(angle) + ((double)coordinates[i].Y - imageCenterY) * cos(angle);
            }
        }
    }
    break;

    case Negative:
    {
        if (IsSpriteInBorders(Negative)) {
            for (int i = 0; i < 4; i++) {
                coordinates[i].X = imageCenterX + ((double)coordinates[i].X - imageCenterX) * cos(angle) + ((double)coordinates[i].Y - imageCenterY) * sin(angle);
                coordinates[i].Y = imageCenterY - ((double)coordinates[i].X - imageCenterX) * sin(angle) + ((double)coordinates[i].Y - imageCenterY) * cos(angle);
            }
        }
    }
    break;
    }


}

void MoveRight() {
    if (IsSpriteInBorders(Right)) {
        for (int i = 0; i < 4; i++) {
            coordinates[i].X += STEP_SIZE;
        }
        imageCenterX += STEP_SIZE;
    }
}

void MoveLeft() {
    if (IsSpriteInBorders(Left)) {
        for (int i = 0; i < 4; i++) {
            coordinates[i].X -= STEP_SIZE;
        }
        imageCenterX -= STEP_SIZE;
    }
}

void MoveDown() {
    if (IsSpriteInBorders(Down)) {
        for (int i = 0; i < 4; i++) {
            coordinates[i].Y += STEP_SIZE;
        }
        imageCenterY += STEP_SIZE;
    }
}

void MoveUp() {
    if (IsSpriteInBorders(Up))
    {
        for (int i = 0; i < 4; i++) {
            coordinates[i].Y -= STEP_SIZE;
        }
        imageCenterY -= STEP_SIZE;
    }
}

void SetWindowSize(HWND hwnd) {
    RECT rect = {};
    GetClientRect(hwnd, &rect);
    windowSize.width = rect.right - rect.left;
    windowSize.height = rect.bottom - rect.top;
}

bool IsSpriteInBorders(Directions direction) {
    bool isMovePossible = true;
    switch (direction)
    {
    case Up:
    {
        for (int i = 0; i < 4; i++) {
            if (coordinates[i].Y - 1 < 0) {
                isMovePossible = false;
            }
        }
    }
    break;

    case Down:
    {
        for (int i = 0; i < 4; i++) {
            if (coordinates[i].Y + 1 > windowSize.height) {
                isMovePossible = false;
            }
        }
    }
    break;

    case Left:
    {
        for (int i = 0; i < 4; i++) {
            if (coordinates[i].X - 1 < 0) {
                isMovePossible = false;
            }
        }
    }
    break;

    case Right:
    {
        for (int i = 0; i < 4; i++) {
            if (coordinates[i].X + 1 > windowSize.width) {
                isMovePossible = false;
            }
        }
    }
    break;

    case Positive:
    {
        for (int i = 0; i < 4; i++) {
            double tempX = imageCenterX + ((double)coordinates[i].X - imageCenterX) * cos(ROTATE_ANGLE) - ((double)coordinates[i].Y - imageCenterY) * sin(ROTATE_ANGLE);
            double tempY = imageCenterY + ((double)coordinates[i].X - imageCenterX) * sin(ROTATE_ANGLE) + ((double)coordinates[i].Y - imageCenterY) * cos(ROTATE_ANGLE);
            if ((tempX < 0) || (tempX > windowSize.width) || (tempY < 0) || (tempY > windowSize.height)) {
                isMovePossible = false;
            }
        }
    }
    break;

    case Negative:
    {
        for (int i = 0; i < 4; i++) {
            double tempX = imageCenterX + ((double)coordinates[i].X - imageCenterX) * cos(ROTATE_ANGLE) + ((double)coordinates[i].Y - imageCenterY) * sin(ROTATE_ANGLE);
            double tempY = imageCenterY - ((double)coordinates[i].X - imageCenterX) * sin(ROTATE_ANGLE) + ((double)coordinates[i].Y - imageCenterY) * cos(ROTATE_ANGLE);
            if ((tempX < 0) || (tempX > windowSize.width) || (tempY < 0) || (tempY > windowSize.height)) {
                isMovePossible = false;
            }
        }
        
    }
    break;

    }

    return isMovePossible;

}

void SetStartCoordinates() {
    coordinates[0].X = 0;
    coordinates[0].Y = 0;
    coordinates[1].X = imageWidth;
    coordinates[1].Y = 0;
    coordinates[2].Y = imageHeight;
    coordinates[2].X = 0;
    coordinates[3].X = imageWidth;
    coordinates[3].Y = imageHeight;
    imageCenterX = imageWidth / 2;
    imageCenterY = imageHeight / 2;
}

void Paint(HWND hwnd, HDC hdc, PAINTSTRUCT ps) {
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    Gdiplus::Graphics graphics(hdc);

    Gdiplus::Image* image = Gdiplus::Image::FromFile(L"Pictures/horse.png");

    if (isFirstCall) {
        imageHeight = image->GetHeight();
        imageWidth = image->GetWidth();
        SetStartCoordinates();
        isFirstCall = false;
    }

    graphics.DrawImage(image, coordinates, 3);
}

void DoubleBufferPaint(HWND hwnd, HDC hdc, PAINTSTRUCT ps)
{
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;

    hdcMem = CreateCompatibleDC(ps.hdc);
    hbmMem = CreateCompatibleBitmap(ps.hdc, windowSize.width, windowSize.height);
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    GetClientRect(hwnd, &rc);

    FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW));

    Gdiplus::Graphics graphics(hdcMem);

    Gdiplus::Bitmap image(L"Pictures/horse.png");

    if (isFirstCall) {
        imageHeight = image.GetHeight();
        imageWidth = image.GetWidth();
        SetStartCoordinates();
        isFirstCall = false;
    }

    graphics.DrawImage(&image, coordinates, 3);

    BitBlt(ps.hdc,
            rc.left, rc.top,
            rc.right - rc.left, rc.bottom - rc.top,
            hdcMem,
            rc.left, rc.top,
            SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}
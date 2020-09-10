#include <windows.h>
#include<stdlib.h>
#include<gdiplus.h>
#include<math.h>
#pragma comment(lib, "gdiplus.lib")

#define ROTATE_ANGLE 0.005f;
#define STEP_SIZE 1;

struct WindowSize {
    int height;
    int width;
} windowSize;

enum Directions{
    Up,
    Down,
    Left,
    Right,
    Positive,
    Negative
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void SetStartCoordinates();
void RotateImage(float angle, Directions direction);
void MoveUp();
void MoveDown();
void MoveLeft();
void MoveRight();
void SetWindowSize(HWND hwnd);
bool IsSpriteInBorders(Directions direction);

Gdiplus::PointF coordinates[4];
Gdiplus::Image* image;
UINT imageHeight;
UINT imageWidth;
float imageCenterX;
float imageCenterY;
HFONT hfnt = NULL;
LPWSTR szCaption = NULL;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              
        CLASS_NAME,                     
        L"Learn to Program Windows",    
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
    switch (uMsg)
    {
        case WM_CREATE:
        {
            image = Gdiplus::Image::FromFile(L"horse.png");
            imageHeight = image->GetHeight();
            imageWidth = image->GetWidth();
            SetStartCoordinates();
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
                    float angle = ROTATE_ANGLE;
                    RotateImage(angle, Positive);
                }
                break;
                case VK_SPACE:
                {
                    float angle = ROTATE_ANGLE;
                    RotateImage(angle, Negative);
                }
            }

            InvalidateRect(hwnd, NULL, FALSE);

        }
        return 0;
    
        case WM_ERASEBKGND:
            return (LRESULT)1;

        case WM_SETFONT:
            hfnt = (HFONT)wParam;
            break;

        case WM_SETTEXT:
            lstrcpy(szCaption, (LPWSTR)lParam);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            /*RECT rc;
            HDC hdcMem;
            HBITMAP hbmMem, hbmOld;
            HBRUSH hbrBkGnd;
            HFONT hfntOld = NULL;
            */
            


            HDC hdc = BeginPaint(hwnd, &ps);    

            /*GetClientRect(hwnd, &rc);
            hdcMem = CreateCompatibleDC(ps.hdc);

            hbmMem = CreateCompatibleBitmap(ps.hdc,
                rc.right - rc.left,
                rc.bottom - rc.top);

            hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

            hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            FillRect(hdcMem, &rc, hbrBkGnd);
            DeleteObject(hbrBkGnd);

            if (hfnt) {
                hfntOld =(HFONT) SelectObject(hdcMem, hfnt);
            }

            //
            // Render the image into the offscreen DC.
            //

            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT));
            DrawText(hdcMem,
                szCaption,
                -1,
                &rc,
                DT_CENTER);

            if (hfntOld) {
                SelectObject(hdcMem, hfntOld);
            }

            //
            // Blt the changes to the screen DC.
            //

            BitBlt(ps.hdc,
                rc.left, rc.top,
                rc.right - rc.left, rc.bottom - rc.top,
                hdcMem,
                0, 0,
                SRCCOPY);

            //
            // Done with off-screen bitmap and DC.
            //

            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);*/

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            Gdiplus::Graphics graphics(hdc);

           graphics.DrawImage(image, coordinates, 3);

            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_SIZE:
        {
            SetWindowSize(hwnd);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RotateImage(float angle, Directions direction) {
   
    switch (direction)
    {
        case Positive:
        {
            for (int i = 0; i < 4; i++) {
                coordinates[i].X = imageCenterX + (coordinates[i].X - imageCenterX) * cos(angle) - (coordinates[i].Y - imageCenterY) * sin(angle);
                coordinates[i].Y = imageCenterY + (coordinates[i].X - imageCenterX) * sin(angle) + (coordinates[i].Y - imageCenterY) * cos(angle);
            }
        }
        break;

        case Negative:
        {
            for (int i = 0; i < 4; i++) {
                coordinates[i].X = imageCenterX + (coordinates[i].X - imageCenterX) * cos(angle) + (coordinates[i].Y - imageCenterY) * sin(angle);
                coordinates[i].Y = imageCenterY - (coordinates[i].X - imageCenterX) * sin(angle) + (coordinates[i].Y - imageCenterY) * cos(angle);
            }
        }
        break;
    }

    
}

void MoveRight() {  
    if (IsSpriteInBorders(Right)) {
        coordinates[0].X += 1;
        coordinates[1].X += 1;
        coordinates[2].X += 1;
        coordinates[3].X += 1;
        imageCenterX += 1;
    }
}

void MoveLeft() {
    if (IsSpriteInBorders(Left)) {
        coordinates[0].X -= 1;
        coordinates[1].X -= 1;
        coordinates[2].X -= 1;
        coordinates[3].X -= 1;
        imageCenterX -= 1;
    }
}

void MoveDown() {
    if (IsSpriteInBorders(Down)) {
        coordinates[0].Y += 1;
        coordinates[1].Y += 1;
        coordinates[2].Y += 1;
        coordinates[3].Y += 1;
        imageCenterY += 1;
    }
}

void MoveUp() {
    if (IsSpriteInBorders(Up))
    {
        coordinates[0].Y -= 1;
        coordinates[1].Y -= 1;
        coordinates[2].Y -= 1;
        coordinates[3].Y -= 1;
        imageCenterY -= 1;
    }
}

void SetWindowSize(HWND hwnd) {
    RECT rect = {};
    GetWindowRect(hwnd, &rect);
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
                if (coordinates[i].Y + 1 + 35 > windowSize.height) {
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
    }

    return isMovePossible;

}

void SetStartCoordinates() {
    coordinates[0].X = 0;
    coordinates[0].Y = 0;
    coordinates[1].X = 0 + imageWidth;
    coordinates[1].Y = 0;
    coordinates[2].Y = 0 + imageHeight;
    coordinates[2].X = 0;
    coordinates[3].X = 0 + imageWidth;
    coordinates[3].Y = 0 + imageHeight;
    imageCenterX = 0 + imageWidth / 2;
    imageCenterY = 0 + imageHeight / 2;
}
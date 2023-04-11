#include "pong.h"
#include <stdexcept>
#include <Windows.h>
std::wstring const pong::s_class_name{ L"PONG" };



pong::pong(HINSTANCE instance)
    : m_instance{ instance }
{
    register_class();

    DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;
    m_main = create_window(main_style);
    m_popup = create_window(popup_style, m_main);
    SetLayeredWindowAttributes(m_popup, 0, 255, LWA_ALPHA);
    Bx = 10;
    By = 10;
    Brad = 7;
    speedx = 5;
    speedy = 5;

    
    
}


bool pong::register_class()
{
    WNDCLASSEXW desc{};
    if (GetClassInfoExW(m_instance, s_class_name.c_str(),
        &desc) != 0)
        return true;

    desc = {
    .cbSize = sizeof(WNDCLASSEXW),
    .lpfnWndProc = window_proc_static,
    .hInstance = m_instance,
    .hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
    .hbrBackground = CreateSolidBrush(RGB(144, 238, 144)),
    .lpszMenuName= MAKEINTRESOURCEW(IDR_MENU1),
    .lpszClassName = s_class_name.c_str()
    };
    return RegisterClassExW(&desc) != 0;
}

HWND pong::create_window(DWORD style, HWND parent, DWORD ex_style)
{

    RECT prnt;
    GetWindowRect(parent, &prnt); // p window 
   
    int width = (GetSystemMetrics(SM_CXSCREEN) - (prnt.right - prnt.left)) / 2; 
    int height = (GetSystemMetrics(SM_CYSCREEN) - (prnt.bottom - prnt.top)) / 2;
    SetWindowPos(parent, NULL, width, height, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    update_transparency();


    HWND window = CreateWindowExW(
        ex_style,
        s_class_name.c_str(),
        L"PONG",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        350,
        parent,
        nullptr,
        m_instance,
        this);

    RECT paddl;
    pw = 15;
    ph = 60;
    GetClientRect(window, &paddl); //client area of the window
    int posa = (paddl.right - pw);
    int posb = (paddl.bottom - ph) / 2;

   CreateWindowExW(
        0,
        L"STATIC",
        nullptr,
        WS_CHILD | WS_VISIBLE,
        posa,
        posb,
        pw,
        ph,
        window,
        nullptr,
        m_instance,
        nullptr);
 
 
    return window;
}




LRESULT pong::window_proc_static(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
    pong* app = nullptr;
    if (message == WM_NCCREATE)
    {
        app = static_cast<pong*>(
            reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(app));

    }
    else
        app = reinterpret_cast<pong*>(
            GetWindowLongPtrW(window, GWLP_USERDATA));
    LRESULT res = app ?
        app->window_proc(window, message, wparam, lparam) :
        DefWindowProcW(window, message, wparam, lparam);
    if (message == WM_NCDESTROY)
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
    return res;
}


void pong::ball_paddle_collision()
{
    RECT ball{ Bx - Brad, By - Brad, Bx + Brad, By + Brad }; 
    HWND paddle = FindWindowExW(m_main, nullptr, L"STATIC", nullptr);
    RECT pad_rec;
    GetWindowRect(paddle, &pad_rec);
    MapWindowPoints(nullptr, m_main, reinterpret_cast<POINT*>(&pad_rec), 2);

    if (IntersectRect(&ball, &ball, &pad_rec))
    {
        rscore++;
        speedx = -speedx;
        if (speedx < 0)
            Bx = pad_rec.left - Brad;
        else
            Bx = pad_rec.right + Brad;
    }
}


void pong::move_paddle(int y)
{
    RECT move;
    GetWindowRect(GetParent(FindWindowExW(m_main, nullptr, L"STATIC", nullptr)), &move);

    RECT prect;
    GetWindowRect(FindWindowExW(m_main, nullptr, L"STATIC", nullptr), &prect);
    MapWindowPoints(nullptr, m_main, reinterpret_cast<POINT*>(&prect), 2);

    int paddleHeight = prect.bottom - prect.top;
    int newY = max(0, min(y, move.bottom - move.top - paddleHeight));

    MoveWindow(FindWindowExW(m_main, nullptr, L"STATIC", nullptr), prect.left, newY, prect.right - prect.left,
        prect.bottom - prect.top, true);
}

//TRAIIL 

void pong::Draw(HDC hdc)
{
   

    COLORREF ballColor = RGB(255, 0, 0); 
    HBRUSH ballBrush = CreateSolidBrush(ballColor);
    SelectObject(hdc, ballBrush);
    HPEN ballPen = CreatePen(PS_SOLID, 1, ballColor);
    HPEN old = (HPEN)SelectObject(hdc, ballPen);

  
    Ellipse(hdc, Bx - Brad, By - Brad, Bx + Brad, By + Brad);

  
    int trailrad = Brad / 1.2;
    int trailOp = 50;
    int trailno = 5; 
    int x = Bx - (speedx * 3); 
    int y = By - (speedy * 3);
    int spacing = trailrad / 4; 
    for (int i = trailno; i >= 1; i--)
    {
        int rad = trailrad * i / trailno + spacing;
        int opac = trailOp * i / trailno;

       
        COLORREF trailColor = RGB(255, 0, 0); 
        trailColor = trailColor & 0x00FFFFFF ; 
        HBRUSH trailBrush = CreateSolidBrush(trailColor);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, trailBrush);
        Ellipse(hdc, x - rad, y - rad, x + rad, y + rad);

        SelectObject(hdc, oldBrush);
        DeleteObject(trailBrush);

        // next trail ppos
        x -= speedx * 2;
        y -= speedy * 2;
    }
    SelectObject(hdc, old);
    DeleteObject(ballBrush);
    DeleteObject(ballPen);

}

void pong::bitmap(HWND window, LPCWSTR szFile)
{
    
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, szFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBitmap == NULL) {return;}

    HDC hdc = GetDC(window);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    ReleaseDC(window, hdc);
    DeleteObject(hBitmap);
}


LRESULT pong::window_proc(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)

   
{
    switch (message)
    {
    case WM_PAINT: //find doc
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window, &ps);

        //ball object
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, Bx - Brad, By - Brad, Bx + Brad, By + Brad);
        //https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-ellipse
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
        Draw(hdc);

      //counter 
        COLORREF invcolor = RGB(255 - GetRValue(background), 255 - GetGValue(background), 255 - GetBValue(background));
        //https://cpp.hotexamples.com/examples/-/-/GetBValue/cpp-getbvalue-function-examples.html

        HFONT hFont = CreateFont(80, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));
    //https://learn.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes#choosing-a-font
      

        HFONT F1 = (HFONT)SelectObject(hdc, hFont);
        TCHAR l_buff[32];
        wsprintf((LPWSTR)l_buff, L"%d", lscore);
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, RGB(0, 0, 0));
        SetTextColor(hdc, invcolor);
        TextOut(hdc, 90, 20, l_buff, lstrlen(l_buff));
        SelectObject(hdc, F1);
        DeleteObject(hFont);

        HFONT hFont1 = CreateFont(80, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));

        HFONT F2 = (HFONT)SelectObject(hdc, hFont1);
        TCHAR r_buff[32];
        wsprintf((LPWSTR)r_buff, L"%d", rscore);
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, RGB(0, 0, 0));
        SetTextColor(hdc, invcolor);
        TextOut(hdc, 300, 20, r_buff, lstrlen(r_buff));
        SelectObject(hdc, F2);
        DeleteObject(hFont1);

        //https://cpp.hotexamples.com/examples/-/-/CreateFont/cpp-createfont-function-examples.html

        bitmap(window, szFile);
        EndPaint(window, &ps);
    }
    break;

    case WM_TIMER:
    {
        
        Bx += speedx;
        By += speedy;
        RECT B;
        GetClientRect(window, &B);

        HWND paddle = FindWindowExW(window, nullptr, L"STATIC", nullptr);
        RECT pad;
        GetWindowRect(paddle, &pad);
        ball_paddle_collision();

       
       /* if (Bx + Brad > B.right) {
            speedx *= 0;
            speedy *= 0;
        }*///this was used to stop ball in the lab part

        if (Bx + Brad >= pad.left && Bx - Brad <= pad.right && By + Brad >= pad.top && By - Brad <= pad.bottom)
        {
            speedx = -speedx;
           
        }

         if (Bx - Brad < 0) //left side
        {

            speedx = abs(speedx);
            Bx = Brad;
           
        }
        else if (Bx + Brad > B.right) //right side
        {
             lscore++;
            speedx = -abs(speedx);
            Bx = B.right - Brad;
          
        }

        if (By - Brad < 0 || By + Brad > B.bottom)
        {
            speedy = -speedy;  //BOUNCE
        }

        InvalidateRect(window, NULL, TRUE);
    }
    break;

    case WM_MOUSEMOVE:
    {
        int Y = HIWORD(lparam) - (ph / 2); 
        move_paddle(Y); 
        break;
    }
    case WM_COMMAND: 
        switch (LOWORD(wparam))
        {

        case ID_EXIT_APP:
            PostQuitMessage(0);
            break;
        case ID_NEW_GAME:
           
            Bx = pw /6;
            By = ph /6;

          
            speedx = 5;
            speedy = 5;

           
            lscore = 0;
            rscore = 0;
            break;
        case ID_ABOUT:
            MessageBox(window, L"PONG Information hehehehe\n\n 2023\n\n By: Chelcea Igihozo", L"About", MB_OK);
            //https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
            break;
        case ID_BITMAP:
        {//https://stackoverflow.com/questions/16791953/openfilename-open-dialog

          
            OPENFILENAME open;
            ZeroMemory(&open, sizeof(open));
            open.lStructSize = sizeof(open);
            open.hwndOwner = window;
            open.lpstrFile = szFile;
            open.lpstrFile[0] = '\0';
            open.nMaxFile = sizeof(szFile);
            open.lpstrFileTitle = NULL;
            open.nMaxFileTitle = 0;
            open.lpstrInitialDir = NULL;
            open.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&open))
            {
                HMENU Fmenu = GetMenu(window);
                EnableMenuItem(Fmenu, ID_BACKGROUND_TILE, MF_ENABLED);
                EnableMenuItem(Fmenu, ID_BACKGROUND_STRETCH, MF_ENABLED);
                hBitmap = (HBITMAP)LoadImage(NULL, szFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBitmap != NULL)
                    DeleteObject(hBitmap);
        
                if (hBitmap == NULL)
                {
                    MessageBox(window, L"Failed to load bitmap!", L"Error", MB_OK | MB_ICONERROR);
                    return 0;
                }
                isBitmap = true;
      
            }
            
            break;
        }
       
        case ID_COLOR: 
            
            CHOOSECOLOR cc = { 0 }; //https://learn.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes#choosing-a-color
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = window;

            COLORREF default_color = GetBkColor(GetDC(window));
            static COLORREF custom_colors[16];
            cc.lpCustColors = custom_colors;
            cc.rgbResult = default_color;
            cc.Flags = CC_FULLOPEN | CC_ANYCOLOR; 

           
            if (ChooseColor(&cc))
            {
               
                HBRUSH hBrush = CreateSolidBrush(cc.rgbResult);
                SetClassLongPtr(window, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                InvalidateRect(window, nullptr, TRUE);
            }
            break;
        }
        break;

    case WM_CLOSE:
            DestroyWindow(m_main);
            return 0;
    case WM_DESTROY:
            if (window == m_main)
                PostQuitMessage(EXIT_SUCCESS);
            return 0;
            }
            return DefWindowProcW(window, message, wparam, lparam);
        }
        
    


void pong::update_transparency()  
{
    SetWindowLong(m_main, GWL_EXSTYLE, GetWindowLong(m_main, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(m_main, 0, 255 * 80 / 100, LWA_ALPHA);
}

int pong::run(int show_command)
{
    ShowWindow(m_main, show_command);
    MSG msg{};
    BOOL result = TRUE;
    SetTimer(m_main, 1, 50, NULL);

    HACCEL hAccelTable = LoadAccelerators(m_instance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
    {
        if (result == -1)
            return EXIT_FAILURE;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return EXIT_SUCCESS;
}



#include <windows.h>
#include <commdlg.h>
#include <fstream>

#define ID_EDIT 101

#define IDM_NEW 201
#define IDM_OPEN 202
#define IDM_SAVE 203
#define IDM_SAVEAS 204
#define IDM_EXIT 205

#define IDM_UNDO 301
#define IDM_CUT 302
#define IDM_COPY 303
#define IDM_PASTE 304
#define IDM_DELETE 305
#define IDM_SELECTALL 306

#define IDM_WORDWRAP 401
#define IDM_FONT 402

#define IDM_STATUSBAR 501
#define IDM_ABOUT 601

HWND hEdit, hStatus;
HFONT hFont;
bool wordWrap = true;
bool showStatus = true;
char fileName[MAX_PATH] = "";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void AddMenus(HWND hwnd) {
    HMENU hMenu = CreateMenu();

    HMENU file = CreateMenu();
    HMENU edit = CreateMenu();
    HMENU format = CreateMenu();
    HMENU view = CreateMenu();
    HMENU help = CreateMenu();

    AppendMenu(file, MF_STRING, IDM_NEW, "New");
    AppendMenu(file, MF_STRING, IDM_OPEN, "Open");
    AppendMenu(file, MF_STRING, IDM_SAVE, "Save");
    AppendMenu(file, MF_STRING, IDM_SAVEAS, "Save As");
    AppendMenu(file, MF_SEPARATOR, 0, NULL);
    AppendMenu(file, MF_STRING, IDM_EXIT, "Exit");

    AppendMenu(edit, MF_STRING, IDM_UNDO, "Undo");
    AppendMenu(edit, MF_SEPARATOR, 0, NULL);
    AppendMenu(edit, MF_STRING, IDM_CUT, "Cut");
    AppendMenu(edit, MF_STRING, IDM_COPY, "Copy");
    AppendMenu(edit, MF_STRING, IDM_PASTE, "Paste");
    AppendMenu(edit, MF_STRING, IDM_DELETE, "Delete");
    AppendMenu(edit, MF_SEPARATOR, 0, NULL);
    AppendMenu(edit, MF_STRING, IDM_SELECTALL, "Select All");

    AppendMenu(format, MF_CHECKED, IDM_WORDWRAP, "Word Wrap");
    AppendMenu(format, MF_STRING, IDM_FONT, "Font");

    AppendMenu(view, MF_CHECKED, IDM_STATUSBAR, "Status Bar");

    AppendMenu(help, MF_STRING, IDM_ABOUT, "About");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)file, "File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)edit, "Edit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)format, "Format");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)view, "View");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)help, "Help");

    SetMenu(hwnd, hMenu);
}

void OpenFile() {
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::ifstream file(fileName);
        std::string text((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        SetWindowText(hEdit, text.c_str());
    }
}

void SaveFile(bool saveAs) {
    if (saveAs || strlen(fileName) == 0) {
        OPENFILENAME ofn = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;

        if (!GetSaveFileName(&ofn)) return;
    }

    int len = GetWindowTextLength(hEdit);
    char* buffer = new char[len + 1];
    GetWindowText(hEdit, buffer, len + 1);

    std::ofstream file(fileName);
    file << buffer;
    file.close();
    delete[] buffer;
}

void ChooseFontDialog(HWND hwnd) {
    CHOOSEFONT cf = {0};
    LOGFONT lf = {0};

    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.Flags = CF_EFFECTS | CF_SCREENFONTS;

    if (ChooseFont(&cf)) {
        hFont = CreateFontIndirect(&lf);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmd) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "MyNotepad";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("MyNotepad", "Krish Notepad",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 900, 600,
        NULL, NULL, hInst, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        AddMenus(hwnd);
        hEdit = CreateWindow("EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0,
            hwnd, (HMENU)ID_EDIT, NULL, NULL);
        break;

    case WM_SIZE:
        MoveWindow(hEdit, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
        break;

    case WM_COMMAND:
        switch (wp) {
        case IDM_NEW: SetWindowText(hEdit, ""); fileName[0] = '\0'; break;
        case IDM_OPEN: OpenFile(); break;
        case IDM_SAVE: SaveFile(false); break;
        case IDM_SAVEAS: SaveFile(true); break;
        case IDM_EXIT: PostQuitMessage(0); break;

        case IDM_UNDO: SendMessage(hEdit, WM_UNDO, 0, 0); break;
        case IDM_CUT: SendMessage(hEdit, WM_CUT, 0, 0); break;
        case IDM_COPY: SendMessage(hEdit, WM_COPY, 0, 0); break;
        case IDM_PASTE: SendMessage(hEdit, WM_PASTE, 0, 0); break;
        case IDM_DELETE: SendMessage(hEdit, WM_CLEAR, 0, 0); break;
        case IDM_SELECTALL: SendMessage(hEdit, EM_SETSEL, 0, -1); break;

        case IDM_WORDWRAP:
            wordWrap = !wordWrap;
            SetWindowLong(hEdit, GWL_STYLE,
                wordWrap ? WS_CHILD | WS_VISIBLE | ES_MULTILINE :
                           WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_HSCROLL);
            InvalidateRect(hEdit, NULL, TRUE);
            break;

        case IDM_FONT: ChooseFontDialog(hwnd); break;

        case IDM_ABOUT:
            MessageBox(hwnd, "Krish Notepad\nver - 2.0", "About", MB_OK);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}


# 基本的なウィンドウとGDI

## 基本的なコンソールアプリケーション
### 基本的なスタータープログラム: int main()
CまたはC++プログラムのエントリーポイントは、`main`という名前の関数です。  
コンパイラは、指定したエントリーファイルに`main`という関数がない場合、エラーを出します。  
```c
int main() {
    return 0;
}
```

### コマンドライン引数: `int main(int argc, char* argv)`
関数はいくつかのシグネチャを持つことができます。以下はその1つのバージョンです。  
```c
int main(int argc, char** argv)
{
    return 0;
}
```
`argc`と`argv`は、プログラムに渡された引数を解析するために使用されます。これがターミナルプログラムが引数を受け取る方法です。  
- `argc`: 「スペース」で区切られたパラメータの数  
- `argv`: char*ポインタのリスト、つまりC文字列  

戻り値の数は、プログラムがどのように終了したかを外部プログラムに示します。  
OSはこの数を使用しませんが、この数は別のプログラムによって読み取られる可能性があります。  

初めてのプログラム：  
```c
#include <stdio.h>

int main() 
{
    printf("Hello World!");
    return 0;
}
```
`stdio.h`には、コンソール出力、ディスクの読み書きなど、I/O関連のユーティリティのリストが含まれています。  

## 基本的なWin32ウィンドウアプリケーション
`int main()`はどの構成やOSでも動作しますが、主にターミナルアプリケーション向けです。  
グラフィックアプリケーション（GUI）の場合、OSは特定の形式を課します。  

Windowsでは、最低レベルのAPIはWin32です。これは、Windowsで書かれたすべてのプログラムが、Windows SDKによって宣言されたAPIエンドポイント（一般にWin32と呼ばれる）をどこかで呼び出すことを意味します。  
Win32は[グラフィカルアプリケーション用の独自のエントリーポイント](https://learn.microsoft.com/ja-jp/windows/win32/learnwin32/winmain--the-application-entry-point)を宣言しています。  
```c
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
```
これらのパラメータの意味は、ドキュメントを参照してください。  
ゲームを書いているので、ピクセルを専用のウィンドウに表示する必要があります。  
以下は、Win32でウィンドウを作成する例です。  
```c
LRESULT CALLBACK 
WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (iMsg)  
    {
        case WM_CREATE:
        {
        }break;
        
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
        }break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }break;
    }
    
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)  
{
    hinstance = hInstance;
    
    WNDCLASS wndclass;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndclass.lpszMenuName  = 0;
    wndclass.lpszClassName = L"WinMin";
    wndclass.cbWndExtra    = sizeof(void*);
    
    RegisterClass(&wndclass);
    hwnd = CreateWindowA("WinMin", "Minimal Window", 
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 600, 400, NULL, 
                        NULL, hInstance, szCmdLine);
    
    HDC dc = GetDC(hwnd);
    
    while(true)
    {
        MSG msg;
        while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
        {    
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    ReleaseDC(hwnd, dc);
    
    return 0;
}
```

このコードを実行すると、以下のようなウィンドウが表示されます。  
![image](https://github.com/user-attachments/assets/e8abdb9d-b94f-4433-b637-f181d30b55d0)

`CreateWindowA`は、ウィンドウのスタイルや動作を制御するAPIです。サイズ、タイトル、フレームスタイル、開始時に最小化または最大化するなどのプロパティを設定できます。  
この関数は、ここでは`hwnd`と名付けられた`HANDLE`を返します。これはウィンドウのプロセスへの参照であり、ウィンドウのサイズや位置を取得するなどの操作に必要です。  

重要なフィールドの1つは`lpfnWndProc`です。これは、OSが次のようにメッセージを処理する際に呼び出されるコールバックです。  

```c
MSG msg;
while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
{    
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```

毎フレーム、メッセージはキューから取得され、プログラムによって処理されます。  
Win32では、`MSG`構造体にはイベントキーを表す`message`フィールドがあります。  

```c
while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
{
    if(msg.message == WM_QUIT)
    {
        // WM_QUITイベントが発生したときにプログラムに送信されるメッセージ
    }
    
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```
これがOSがプログラムにイベント（マウス/キーボード入力イベントやウィンドウシステム固有のメッセージなど）を送信する主要な方法です。  

**注**: メッセージングは、ほとんどのオペレーティングシステムがユーザープログラムに使用する推奨メカニズムです。Linuxでは、OSメッセージは同じ方法で取得されます。  
```c
// Xlib（X11）のメッセージ処理の例
while (running) {
    XEvent event;
    XNextEvent(display, &event);  // ブロッキング呼び出し

    switch (event.type) {
        case Expose:
            printf("Expose (再描画) イベント\n");
            break;
        case KeyPress:
            printf("キーが押されました、終了します。\n");
            running = false;
            break;
        case DestroyNotify:
            printf("ウィンドウが閉じられました。\n");
            running = false;
            break;
    }
}
```

ウィンドウを作成した後、明示的に閉じられるまで無期限に実行される無限ループを設定します。  
```c
while(true) {
    // プログラムとGUIを更新
}
```
OSメッセージ部分と組み合わせると、次のようになります：  
```c
while(true) 
{
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // プログラムとGUIを更新
}
```
これがGUIプログラムが最も基本的なレベルで書かれる方法です。  
ゲームの場合、`while(true)`のスコープは「ゲームループ」と呼ばれます。  

### GDIとソフトウェアレンダリング
実際、ゲームループの各イテレーションは、新しいフレームの内容で画面をリフレッシュします。  
Win32では、「レンダリング」、つまりピクセルを画面に描画する方法は、たとえば四角形を描くなど、さまざまなオプションがあります。  
このコースでは、「ソフトウェアレンダラ」を作成します。これは、GPUではなくCPUでピクセルを描画することを意味します。  

Win32がレンダリングに使用する主要なAPIは、GDI（Graphics Device Interface）と呼ばれます。これは2Dグラフィックスを描画するためのコアコンポーネントです。  

レンダラを書く前に、処理する必要のある2つのOSメッセージがあります。  
#### WM_SIZE
`WM_SIZE`は、ウィンドウのサイズが変更された後（ウィンドウが最初に開かれたときも含む）に送信されます。ここで、画面全体をカバーするのに十分なメモリを確保する必要があります。  
`WindowClass`コールバック関数に戻り、`WM_SIZE`を処理します。  
```c
LRESULT CALLBACK 
WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (iMsg)  
    {
        case WM_CREATE:
        {
        }break;
        
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
        }break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }break;

        /**
        WM_SIZEメッセージを処理する新しいケース
        **/
        case WM_SIZE:
        {
            // ここでメモリを確保します
        }break;
    }
    
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
```

**注**: メモリの部分は一般に「**バッファ**」と呼ばれます。レンダリングのコンテキストでは、ピクセルデータを書き込むバッファを**バックバッファ**と呼びます。  

次に、ウィンドウと描画可能エリアの寸法を取得します。  
[GetClientRect](https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-getclientrect)は、ウィンドウの寸法を取得するために使用されます。  
```c
RECT clientRect; // 右、左、上、下
GetClientRect(hwnd, &clientRect);
```

`RECT`は、画面のサブリージョンの座標を提供するWin32構造体です。  
![image](https://github.com/user-attachments/assets/24b201c9-d6df-4ca7-8f5f-22f6aa9c8cdf)

特に`GetClientRect`を使用すると、プログラムの位置とサイズを取得できます。  
ウィンドウのサイズを計算できます。  
```c
int width = clientRect.right - clientRect.left;
int height = clientRect.bottom - clientRect.top;
```

物理的なモニターでは、画面に近づくと、電気電流で制御される小さな色のドットで構成されていることがわかります。  
プログラミングでも概念は似ています：利用可能な最小の矩形単位に色を書き込みます。悪名高いRGBタプルは、ピクセルの色を表現するために使用されます。  
それぞれは1バイトのデータで表され、したがって1ピクセルあたり3バイトです。さらに、透明度（またはアルファ）のために、別の1バイトのデータを追加し、合計で1ピクセルあたり4バイト（または4bpp）になります。  

**注**: R、G、Bはチャンネルと呼ばれます。1つのチャンネルは0から255までの値を取ることができます。これは、たとえばCSS3で使用される悪名高いRGBタプルを表します。  

したがって、必要なメモリの総量を確保できます。  
```c
int bytesPerPixel = 4;
int totalBackbufferSize = bytesPerPixel * width * height;
```
最後に、Win32 APIを呼び出して、OSからヒープメモリを要求します。  
```c
void* backbuffer = VirtualAlloc(0, totalBackbufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
```

**注**: `VirtualAlloc`は、Windowsで利用可能なすべてのメモリ割り当てライブラリの基本APIであり、`malloc`のような関数を使用する代わりに、OSと直接対話します。  

このルーチンを`Win32ResizeDIBSection`という関数に書き込み、さらにバックバッファをグローバル変数として設定します。後でリファクタリングします。  
```c
void* backbuffer = NULL; // リマインダー：ポインタは、メモリ内のアドレスを参照します！

void Win32ResizeDIBSection(RECT clientRect)
{
    // ウィンドウの寸法を計算
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    int bytesPerPixel = 4; // RGBA => 32ビットまたは4バイト
    int bytesToAllocate = bytesPerPixel * engine->width * engine->height;
    backbuffer = VirtualAlloc(0, bytesToAllocate, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
```

次に、バックバッファを描画する新しいルーチンを作成します。  
GDIでは、バッファを受け取り、画面に描画するAPIである`StretchDIBits`があります。  
```c
Win32UpdateWindow(HDC dc, RECT clientRect)
{
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;
    
    BITMAPINFO bmInfo = {};
    bmInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmInfo.bmiHeader.biWidth = width;
    bmInfo.bmiHeader.biHeight = -height; // サインを反転させて、Y軸を逆にした座標系にします
    bmInfo.bmiHeader.biPlanes = 1;
    bmInfo.bmiHeader.biBitCount = 32;
    bmInfo.bmiHeader.biCompression = BI_RGB;
    
    StretchDIBits(dc,
                  0, 0, windowWidth, windowHeight, // 新しいRECT
                  0, 0, engine->width, engine->height, // 古いRECT
                  backbuffer,
                  &engine->bmInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}
```
さらに、バックバッファのサイズを記述するBitmap Info構造体を埋めます。  
#### WM_PAINT
プログラムは`WM_PAINT`に反応して、バックバッファをウィンドウに描画します。  
```c
case WM_PAINT:
{
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(hwnd, &paint);
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    // ここで実際に描画します！
    Win32UpdateWindow(&engine, dc, clientRect);
    //
    EndPaint(hwnd, &paint);
}break;
```
ただし、これはウィンドウのサイズ変更ごとに1回だけ呼び出されるため、ゲームループでも`Win32UpdateWindow`関数を呼び出す必要があります。  

```c
while(true) 
{
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    Win32UpdateWindow(dc, clientRect);
}
```

これで、基本的なレンダラが完成しましたが、画面には何も描画されていません。すべてのピクセルが(0, 0, 0, 0)に設定されているため、すべて黒になります。これは理にかなっています。  
このセッションを締めくくるために、画面にいくつかのピクセルを描画しましょう。  
### 画面に最初のエフェクトを描画する
基本的に、バッファにピクセルを書き込む必要があります。  
これを2重のforループを使用して行います：  
```c
for(int y = 0; y < height; ++y) {
    for(int x = 0; x < width; ++x) {
        // ここに色を書き込みます
    }
}
```

ポインタ演算を使用して始めましょう：  
```c
int pitch = width * bytesPerPixel; // ピクセルの行ごとのメモリ量
unsigned char* row = (unsigned char*)backbuffer; // ポインタ型を最小の整数に変換します

for(int y = 0;
    y < height;
    ++y)
{
    // 書き込む現在のピクセルの位置を使用してインクリメントする新しい変数を作成
    unsigned int* pixel = (unsigned int*)row;
    for(int x = 0;
        x < width;
        ++x)
    {
        // 現在のピクセルのデータを生成
        // これは任意であり、値を変えて新しいパターンを生成できます
        unsigned char r = 0;
        unsigned char g = (unsigned char)(y + yoffset);
        unsigned char b = (unsigned char)(x + xoffset);

        // ピクセルに書き込み、チャンネルの順序はARGBです！
        // これはビット演算子です。もし慣れていない場合は、オンラインで調べることができます
        *pixel = r << 16 | g << 8 | b;

        // 次のピクセルに移動するためにポインタをインクリメント
        ++pixel;
    }

    // データサイズの1行分を追加して、次の行に移動！
    row += pitch;
}
```
![image](https://github.com/user-attachments/assets/727898db-3b4b-4e63-bf16-47dab3175efb)

このコードは意図的に不完全であり、独自の方法を見つけ、間違いを犯し、それを解決することでC/C++の書き方を学ぶことができます。  
今週の内容のコピーはGitHubページにもあり、次のセッションのためにそこから始めることができます。  

## Win32 APIの概要
Win32 APIは、Windowsオペレーティングシステムで動作するすべてのプログラムの構成ブロックです。Windows自身の.NET C#言語スイートも含みます。  
以下のAPIインターフェースを含みます：  
- [プロセスとスレッド管理](https://learn.microsoft.com/ja-jp/windows/win32/procthread/processes-and-threads)  
- [メモリ管理](https://learn.microsoft.com/ja-jp/windows/win32/memory/about-memory-management)  
- [ファイルとI/O](https://learn.microsoft.com/ja-jp/windows/win32/fileio/file-systems)  
- [ネットワークI/O](https://learn.microsoft.com/ja-jp/windows/win32/networking)  
- コンソールI/O  
- プロセス間通信（IPC）  
- ウィンドウとGUI  
- グラフィックス（GDI、D3D、DirectWrite、Direct2D、...）  
- ユーザー入力（DirectInput、RawInput、メッセージング）  
- ...  

### MSDNの読み方
Win32 API関数の場合、通常、ドキュメントは以下のような関数シグネチャを提供します：  
```c
BOOL WriteFile(
  [in]                HANDLE       hFile,
  [in]                LPCVOID      lpBuffer,
  [in]                DWORD        nNumberOfBytesToWrite,
  [out, optional]     LPDWORD      lpNumberOfBytesWritten,
  [in, out, optional] LPOVERLAPPED lpOverlapped
);
```
- `in`と`out`は、パラメータが読み取り（in）または書き込み（out）であることを示し、その後に型と名前が続きます。`optional`は、パラメータが0（またはNULL）でもよいことを意味します。  
- Windowsでは、一般に型が`LP`で始まる場合、通常「ロングポインタ」を意味します。  
```c
HANDLE file;

char* buffer = "Content";

int writtenBytes = 0;

WriteFile(file, buffer, sizeof(buffer), &writtenBytes, NULL);
```

通常、ページの最後には`Requirements`セクションがあり、プログラムにAPIを含める方法に関する情報が記載されています。  
- ヘッダー：インクルードするファイル、`windows.h`にすでに含まれている場合を除く  
- ライブラリ：コンパイル時にリンクする静的ライブラリ  
- DLL：このAPIが含まれる動的ライブラリ  

## まとめ
このセッションの重要なポイント：  
- Win32ウィンドウを開く  
- バックバッファとしてヒープメモリを割り当てる  
- 画面に何かを描画する  


#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

static HWND hwnd;
static HINSTANCE hinstance;
static BITMAPINFO bmInfo = {};
static void* bmMemory = NULL;
static int bmWidth;
static int bmHeight;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

struct quad {
	int x;
	int y;
	int width;
	int height;
};

static void
RenderGradient(int xoffset, int yoffset)
{
	int pitch = bmWidth * 4;
	unsigned char* row = (unsigned char*)bmMemory;
	for (int y = 0;
		y < bmHeight;
		++y)
	{
		unsigned int* pixel = (unsigned int*)row;
		for (int x = 0;
			x < bmWidth;
			++x)
		{
			unsigned char r = 0;
			unsigned char g = (unsigned char)(y + yoffset);
			unsigned char b = (unsigned char)(x + xoffset);

			*pixel = r << 16 | g << 8 | b;
			++pixel;
		}

		row += pitch;
	}
	
}

static void
Win32ResizeDIBSection(RECT* clientRect)
{
	int width = clientRect->right - clientRect->left;
	int height = clientRect->bottom - clientRect->top;

	if (bmMemory)
	{
		VirtualFree(bmMemory, 0, MEM_RELEASE);
	}

	bmWidth = width;
	bmHeight = height;
	int bytesPerPixel = 4;
	int bytesToAllocate = bytesPerPixel * bmWidth * bmHeight;
	bmMemory = VirtualAlloc(0, bytesToAllocate, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);
	bmInfo.bmiHeader.biWidth = bmWidth;
	bmInfo.bmiHeader.biHeight = -bmHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
}

static void
Win32UpdateWindow(HDC dc, RECT clientRect)
{
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	StretchDIBits(dc,
		0, 0, windowWidth, windowHeight,
		0, 0, bmWidth, bmHeight,
		bmMemory,
		&bmInfo,
		DIB_RGB_COLORS, SRCCOPY);

}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	hinstance = hInstance;

	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = L"WinMin";

	RegisterClass(&wndclass);
	hwnd = CreateWindowA("WinMin", "Minimal Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 600, 400, NULL, NULL, hInstance, szCmdLine);

	{
		quad q[10000] = {};
	}
	

	HDC dc = GetDC(hwnd);

	int xoff = 0;
	int yoff = 0;
	bool running = true;

	while (running)
	{
		MSG msg;
		while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				running = false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		RenderGradient(xoff, yoff);
		++xoff;
		++yoff;

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		Win32UpdateWindow(dc, clientRect);

		Sleep(10);
	}

	ReleaseDC(hwnd, dc);

	return 0;
}

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

	case WM_SIZE:
	{
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);

		Win32ResizeDIBSection(&clientRect);
	}break;

	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC dc = BeginPaint(hwnd, &paint);
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		Win32UpdateWindow(dc, clientRect);
		EndPaint(hwnd, &paint);
	}break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

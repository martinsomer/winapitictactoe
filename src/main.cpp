#include "board.hpp"
#include <windows.h>

HWND gameWindow, boardSizePicker;
int cellSizePixels = 50;
Board board;

LRESULT CALLBACK Game_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_LBUTTONDOWN: {
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam);
			
			board.setSelectedCell(yPos / cellSizePixels * board.getSizeW() + xPos / cellSizePixels);
			board.makeMove();
			
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			
			board.validate();
			board.switchPlayers();
			
			break;
		}
		
		case WM_KEYDOWN: {
			switch (wParam) {
				case VK_UP: {
					if (board.getSelectedCell() - board.getSizeW() >= 0)
						board.setSelectedCell(board.getSelectedCell() - board.getSizeW());
					
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				
				case VK_DOWN: {
					if (board.getSelectedCell() + board.getSizeW() <= board.getSizeW() * board.getSizeH())
						board.setSelectedCell(board.getSelectedCell() + board.getSizeW());
					
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				
				case VK_LEFT: {
					if (board.getSelectedCell() > 0)
						board.setSelectedCell(board.getSelectedCell() - 1);
					
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				
				case VK_RIGHT: {
					if (board.getSelectedCell() < board.getSizeW() * board.getSizeH() - 1)
						board.setSelectedCell(board.getSelectedCell() + 1);
					
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
				}
				
				case VK_RETURN: {
					board.makeMove();
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					
					board.validate();
					board.switchPlayers();
					
					break;
				}
			}
			
			break;
		}
		
		case WM_PAINT: {
			RECT rect;
			PAINTSTRUCT ps;
			
			GetClientRect(hwnd, &rect);
			HDC hdc = BeginPaint(hwnd, &ps);
			
			FillRect(hdc, &rect, (HBRUSH) (COLOR_WINDOW + 1));
			
			int selectedCellX = board.getSelectedCell() % board.getSizeW() * cellSizePixels;
			int selectedCellY = board.getSelectedCell() / board.getSizeW() * cellSizePixels;
			Rectangle(hdc, selectedCellX, selectedCellY, selectedCellX + cellSizePixels, selectedCellY + cellSizePixels);
			
			HFONT hFont = CreateFont(45, 0, 0, 0,
				FW_LIGHT,
				FALSE, FALSE, FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_MODERN,
				"Arial");
			
			SelectObject(hdc, hFont);
			
			for (int i = 0; i < board.getSize(); i++) {
				Board::cellState state = board.getCellState(i);
				
				switch (state) {
					case Board::cross:
						TextOut(hdc, i % board.getSizeW() * cellSizePixels + 12, i / board.getSizeW() * cellSizePixels + 3, "X", 1);
						break;
					case Board::nought:
						TextOut(hdc, i % board.getSizeW() * cellSizePixels + 10, i / board.getSizeW() * cellSizePixels + 3, "O", 1);
						break;
					default:
						// Blank
						break;
				}
			}
			
			EndPaint(hwnd, &ps);
			break;
		}
		
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK BoardSizePicker_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HWND widthTextBox, heightTextBox, confirmButton;
	
	const LONGLONG ID_WIDTH = 1;
	const LONGLONG ID_HEIGHT = 2;
	const LONGLONG ID_SYMBOLSFORWIN = 3;
	const LONGLONG ID_CONFIRM = 4;
	
	switch (Message) {
		case WM_CREATE: {
			widthTextBox = CreateWindow("EDIT",
				NULL,
				WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER,
				125, 20,
				50, 25,
				hwnd,
				(HMENU) ID_WIDTH,
				NULL, NULL);
			
			SetDlgItemText(hwnd, ID_WIDTH, "9");
				
			heightTextBox = CreateWindow("EDIT",
				NULL,
				WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER,
				125, 65,
				50, 25,
				hwnd,
				(HMENU) ID_HEIGHT,
				NULL, NULL);
			
			SetDlgItemText(hwnd, ID_HEIGHT, "9");
			
			heightTextBox = CreateWindow("EDIT",
				NULL,
				WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER,
				125, 110,
				50, 25,
				hwnd,
				(HMENU) ID_SYMBOLSFORWIN,
				NULL, NULL);
			
			SetDlgItemText(hwnd, ID_SYMBOLSFORWIN, "4");
			
			confirmButton = CreateWindow("BUTTON",
				"OK",
				WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
				62, 155,
				75, 25,
				hwnd,
				(HMENU) ID_CONFIRM,
				NULL, NULL);
				
			break;
		}
		
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_CONFIRM: {
					int widthValue = GetDlgItemInt(hwnd, ID_WIDTH, NULL, FALSE);
					int heightValue = GetDlgItemInt(hwnd, ID_HEIGHT, NULL, FALSE);
					int symbolsForWin = GetDlgItemInt(hwnd, ID_SYMBOLSFORWIN, NULL, FALSE);
					
					if (3 > widthValue || widthValue > 25 || 3 > heightValue || heightValue > 25) {
						MessageBox(NULL, "Board size must be between 3 and 25 units.", "Note", MB_ICONINFORMATION);
						break;
					} else if (3 > symbolsForWin || symbolsForWin > std::max(widthValue, heightValue)) {
						MessageBox(NULL, "Number of symbols required to win must be \nbetween 3 and board's longer side length.", "Note", MB_ICONINFORMATION);
						break;
					}
					
					board.setSize(widthValue, heightValue);
					board.setNumberOfSymbolsForWin(symbolsForWin);
				
					gameWindow = CreateWindow(
						"GameWindow",														// Pointer to registered class name
						"TicTacToe", 														// Pointer to window name
						WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,	// Window style
						CW_USEDEFAULT, CW_USEDEFAULT,										// Window position x, y
						widthValue * cellSizePixels + 6, heightValue * cellSizePixels + 29, // Window width, height
						NULL,																// Handle to parent window
						NULL,																// Handle to child window identifier
						NULL,																// Handle to application instance
						NULL);																// Pointer to window creation data
					
					ShowWindow(gameWindow, TRUE);
				    UpdateWindow(gameWindow);
					
					ShowWindow(hwnd, FALSE);
						
					break;
				}
			}
			
			break;
		}
		
		case WM_PAINT: {
			RECT rect;
			PAINTSTRUCT ps;
			
			GetClientRect(hwnd, &rect);
			HDC hdc = BeginPaint(hwnd, &ps);
			
			FillRect(hdc, &rect, (HBRUSH) (COLOR_WINDOW + 1));
			TextOut(hdc, 71, 25, "Width:", 6);
			TextOut(hdc, 67, 70, "Height:", 7);
			TextOut(hdc, 20, 115, "Combo length:", 13);
			
			break;
		}
		
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;
	
	/**
	 * Register window classes (template which specifies
	 * common attributes of all windows based on the class)
	 */
	
	memset(&wc, 0, sizeof(wc));
	wc.style		 = 0;								// Class style
	wc.lpfnWndProc	 = Game_WndProc;					// Long pointer to the window procedure
	wc.cbClsExtra 	 = 0;								// Number of extra bytes to allocate following the window-class structure
	wc.cbWndExtra	 = 0;								// Number of extra bytes to allocate following the window instance
	wc.hInstance	 = hInstance;						// Handle to parent instance
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); // Load a standard icon
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);		// Handle to the class cursor.
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);		// Handle to the class background brush
	wc.lpszMenuName  = NULL;							// Long pointer to resource name of the class menu
	wc.lpszClassName = "GameWindow";					// Specifies the window class name
	RegisterClass(&wc);
	
	memset(&wc, 0, sizeof(wc));
	wc.style		 = 0;
	wc.lpfnWndProc	 = BoardSizePicker_WndProc;
	wc.cbClsExtra 	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BoardSizePicker";
	RegisterClass(&wc);
	
	/**
	 * Instantiate window based on registered classes
	 */
	
	boardSizePicker = CreateWindow(
		"BoardSizePicker",
		"Board Size",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX & ~WS_SIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		205, 225,
		NULL,
		NULL,
		hInstance,
		NULL);
    
	ShowWindow(boardSizePicker, TRUE);
    UpdateWindow(boardSizePicker);
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}


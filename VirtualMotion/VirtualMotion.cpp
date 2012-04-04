/**
 * VirtualMotion.cpp
 * 
 * Copyright (c) 2008 by <your name/ organization here>
 */
// VirtualMotion.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "VirtualMotion.h"

#define cRepeatON				(0x0001)
#define IBM_EXTENDED_KEYBOARD	(4)
#define MOUSE_EVENT_X(x)		((x) * (65535 / ::GetSystemMetrics(SM_CXSCREEN)))
#define MOUSE_EVENT_Y(y)		((y) * (65535 / ::GetSystemMetrics(SM_CYSCREEN)))

static int g_isExtendedKey = -1;

static LPARAM MakeKeyEventLParam(BOOL bKeyUp, BOOL bPrevKeyState, BOOL bAltDown, USHORT uVirtualKey);
static UINT VirtualKeyDownWithPostMessage(HWND hTargetWnd, UINT uVirtualKey);
static UINT VirtualKeyDownWithSendInput(HWND hTargetWnd, UINT uVirtualKey);

static void VMMouseButtonUp(HWND hTargetWnd,  BOOL bUsePostMessage, DWORD dwMouseEvent, const POINT* pos);
static BOOL VMMouseDragWithPostMessage(const VMMouseMessage* mouseMessage, int reduceCount);
static BOOL VMMouseDragWithSendInput(const VMMouseMessage* mouseMessage, int reduceCount);

static void ActMouse(DWORD type);
static void ActMoveMouse(int x, int y);


/**
 * @brief
 * 仮想的に指定されたキーコードのキー押下を実現します。
 * 
 * @param hTargetWnd
 * キー押下対象のウィンドウハンドル。
 * 
 * @param uVirtualKey
 * 対象のキーコード。
 * 
 * @param bUsePostMessage
 * PostMessageによるキー押下を行う場合にTRUE、SendInputを使用する場合にはFALSEを指定します。
 * （キー入力とマウス入力の異なるウィンドウ[RTT系]の場合にPostMessage、同一の場合[Autodesk系]にSendInputを使用するようにします）
 * 
 * @returns
 * PostMessageまたはSendInputの結果。
 * 
 * 仮想的に指定されたキーコードのキー押下を実現します。
 * 
 * @remarks
 * VMVirtualKeyUp()でキーの押下を解除します。
 * 
 * @see
 * VMVirtualKeyUp()
 */
UINT WINAPI VMVirtualKeyDown(HWND hTargetWnd, UINT uVirtualKey, BOOL bUsePostMessage)
{
	if (bUsePostMessage) {
		return VirtualKeyDownWithPostMessage(hTargetWnd, uVirtualKey);
	} else {
		return VirtualKeyDownWithSendInput(hTargetWnd, uVirtualKey);
	}
}

/**
 * @brief
 * 仮想的に指定されたキーの押下を解除します。
 * 
 * @param hTargetWnd
 * キー押下解除対象のウィンドウハンドル。
 * 
 * @param uVirtualKey
 * 対象のキーコード。
 * 
 * @returns
 * 操作が成功した場合には1、失敗した場合には0を返します。
 * 
 * 仮想的に指定されたキーの押下を解除します。
 * 
 * @remarks
 * VMVirtualKeyDown()によって押下した仮想キーコードや一般のキーを離す場合に使用します。
 * 
 * @see
 * VMVirtualKeyDown()
 */
UINT WINAPI VMVirtualKeyUp(HWND hTargetWnd, UINT uVirtualKey)
{
	DWORD dwFlags = (GetKeyboardType(0) == IBM_EXTENDED_KEYBOARD) ? KEYEVENTF_EXTENDEDKEY : 0;
	dwFlags |= KEYEVENTF_KEYUP;
#if UNICODE || _UNICODE
	dwFlags |= KEYEVENTF_UNICODE;
#endif

	INPUT input;
	input.type				= INPUT_KEYBOARD;
	input.ki.wVk			= (WORD)uVirtualKey;
	input.ki.wScan			= (WORD)MapVirtualKey(uVirtualKey, 0);
	input.ki.dwFlags		= dwFlags;
	input.ki.time			= 0;
	input.ki.dwExtraInfo	= GetMessageExtraInfo();

	SetFocus(hTargetWnd);
	return SendInput(1, &input, sizeof(input));
}

/**
 * @brief
 * キーの名前から仮想キーコードを取得します。
 * 
 * @param szKey
 * 取得対象のキー名。
 * 
 * @returns
 * キーコード、失敗した場合には0。
 * 
 * キーの名前からキーコードを取得します。
 */
UINT WINAPI VMGetVirtualKey(PCTSTR szKey)
{
	if (!_tcsicmp(szKey, TEXT("ctrl")) || !_tcsicmp(szKey, TEXT("control"))) {
		return VK_CONTROL;

	} else if (!_tcsicmp(szKey, TEXT("alt")) || !_tcsicmp(szKey, TEXT("menu"))) {
		return VK_MENU;

	} else if (!_tcsicmp(szKey, TEXT("shift"))) {
		return VK_SHIFT;

	} else if (szKey[0] == TEXT('F') || szKey[0] == TEXT('f')) {
		if (!_tcsicmp(szKey, TEXT("F1"))) {
			return VK_F1;
		}  else if (!_tcsicmp(szKey, TEXT("F2"))) {
			return VK_F2;
		}  else if (!_tcsicmp(szKey, TEXT("F3"))) {
			return VK_F3;
		}  else if (!_tcsicmp(szKey, TEXT("F4"))) {
			return VK_F4;
		}  else if (!_tcsicmp(szKey, TEXT("F5"))) {
			return VK_F5;
		}  else if (!_tcsicmp(szKey, TEXT("F6"))) {
			return VK_F6;
		}  else if (!_tcsicmp(szKey, TEXT("F7"))) {
			return VK_F7;
		}  else if (!_tcsicmp(szKey, TEXT("F8"))) {
			return VK_F8;
		}  else if (!_tcsicmp(szKey, TEXT("F9"))) {
			return VK_F9;
		}  else if (!_tcsicmp(szKey, TEXT("F10"))) {
			return VK_F10;
		}  else if (!_tcsicmp(szKey, TEXT("F11"))) {
			return VK_F11;
		}  else if (!_tcsicmp(szKey, TEXT("F12"))) {
			return VK_F12;
		}
		
	} else if (!_tcsicmp(szKey, TEXT("UP"))) {
		return VK_UP;

	} else if (!_tcsicmp(szKey, TEXT("DOWN"))) {
		return VK_DOWN;

	} else if (!_tcsicmp(szKey, TEXT("LEFT"))) {
		return VK_LEFT;

	} else if (!_tcsicmp(szKey, TEXT("RIGHT"))) {
		return VK_RIGHT;

	} else if (!_tcsicmp(szKey, TEXT("PAGE UP")) || !_tcsicmp(szKey, TEXT("PAGEUP")) || !_tcsicmp(szKey, TEXT("PRIOR"))) {
		return VK_PRIOR;

	} else if (!_tcsicmp(szKey, TEXT("PAGE DOWN")) || !_tcsicmp(szKey, TEXT("PAGEDOWN")) || !_tcsicmp(szKey, TEXT("NEXT"))) {
		return VK_NEXT;

	} else if (!_tcsicmp(szKey, TEXT("RETURN")) || !_tcsicmp(szKey, TEXT("ENTER"))) {
		return VK_RETURN;

	} else if (!_tcsicmp(szKey, TEXT("SPACE"))) {
		return VK_SPACE;

	} else if (!_tcsicmp(szKey, TEXT("ESC")) || !_tcsicmp(szKey, TEXT("ESCAPE"))) {
		return VK_ESCAPE;

	} else if (!_tcsicmp(szKey, TEXT("TAB"))) {
		return VK_TAB;

	} else if (!_tcsicmp(szKey, TEXT("HOME"))) {
		return VK_HOME;

	} else if (!_tcsicmp(szKey, TEXT("END"))) {
		return VK_END;

	} else if (!_tcsicmp(szKey, TEXT("DEL")) || !_tcsicmp(szKey, TEXT("DELETE"))) {
		return VK_DELETE;

	} else if (!_tcsicmp(szKey, TEXT("BACK")) || !_tcsicmp(szKey, TEXT("BACK SPACE")) || !_tcsicmp(szKey, TEXT("BACKSPACE"))) {
		return VK_BACK;

	}
	return 0;
}

/**
 * @brief
 * 仮想的にキー[Altなどの修飾キーを除く一般キー]を押下します。
 * 
 * @param hTargetWnd
 * キー押下対象のウィンドウハンドル。
 * 
 * @param uKey
 * 対象のキーコード['A'や'1'など]。
 * 
 * @param dwMilliseconds
 * 押下後のスリープ時間[msec]。
 * 
 * 仮想的にキーを押下します。
 * 
 * @remarks
 * 押下を解除するにはVMKeyUp()を呼び出します。
 * 
 * @see
 * VMKeyUp()
 */
void WINAPI VMKeyDown(HWND hTargetWnd, UINT uKey, DWORD dwMilliseconds)
{
	PostMessage(hTargetWnd, WM_KEYDOWN, uKey, MAKELPARAM(cRepeatON, MapVirtualKey(uKey, 0)));
	Sleep(dwMilliseconds);
}

/**
 * @brief
 * 仮想キーの押下を解除します。
 * 
 * @param hTargetWnd
 * キー押下解除対象のウィンドウハンドル。
 * 
 * @param uKey
 * 対象のキーコード。
 * 
 * 仮想キーの押下を解除します。
 * 
 * @remarks
 * 押下するにはVMKeyDown()を呼び出します。
 * 
 * @see
 * VMKeyDown()
 */
void WINAPI VMKeyUp(HWND hTargetWnd, UINT uKey)
{
	PostMessage(hTargetWnd, WM_KEYUP, uKey, 0);
}

/**
 * @brief
 * 仮想的にマウスドラッグを実現します。
 * 
 * @param mouseMessage
 * 必要なマウス情報へのポインタ。
 * 
 * @returns
 * キー操作とマウス操作メッセージの送信を行った場合にはTRUE、行えなかった場合にはFALSEが返ります。
 * 
 * 仮想的にマウスドラッグを実現します。
 * キー押下->マウスクリック/MOVE/クリック解除->キーを離す。
 * 
 */
BOOL WINAPI VMMouseDrag(const VMMouseMessage* mouseMessage, int reduceCount)
{
	if (reduceCount == 0) {
		reduceCount = 1;
	}
	if (mouseMessage->bUsePostMessage) {
		return VMMouseDragWithPostMessage(mouseMessage, reduceCount);
	} else {
		return VMMouseDragWithSendInput(mouseMessage, reduceCount);
	}
}

void VMMouseButtonUp(HWND hTargetWnd, BOOL bUsePostMessage, DWORD dwMouseEvent, const POINT* pos) {
	if (bUsePostMessage) {
		PostMessage(hTargetWnd, dwMouseEvent, 0, MAKELPARAM(pos->x, pos->y));
	} else {
		INPUT input[] = {
			{ INPUT_MOUSE, MOUSE_EVENT_X(pos->x), MOUSE_EVENT_Y(pos->y), 0, dwMouseEvent, 0, 0 },
		};
		SetFocus(hTargetWnd);
		SendInput(1, input, sizeof(INPUT));
	}
}

// bKeyUp: WM_SYSKEYUP/WM_KEYUPなら1
// bPrevKeyState: 前回キーが押されている状態なら1（WM_SYSKEYUP/WM_KEYUPの時は必ず1）
// bAltDown: Altキーが押されているか（AltキーのWM_SYSKEYUP/WM_KEYUPの時は必ず0）
LPARAM MakeKeyEventLParam(BOOL bKeyUp, BOOL bPrevKeyState, BOOL bAltDown, USHORT uVirtualKey)
{
	LPARAM lParam = 0;
	if (g_isExtendedKey == -1) {
		if (GetKeyboardType(0) == IBM_EXTENDED_KEYBOARD) {	// 0:キーボードのタイプを取得
			g_isExtendedKey = 1;
		} else {
			g_isExtendedKey = 0;
		}
	}

	if (bKeyUp) {
		lParam |= (1 << 31);
	}
	if (bPrevKeyState) {
		lParam |= (1 << 30);
	}
	if (bAltDown) {
		lParam |= (1 << 29);
	}
	if (g_isExtendedKey) {
		lParam |= (1 << 24);
	}
	lParam |= (MapVirtualKey(uVirtualKey, 0) << 16);
	lParam |= 1;

	return lParam;
}

// キー入力ウィンドウとマウス入力ウィンドウが分かれている場合など
UINT VirtualKeyDownWithPostMessage(HWND hTargetWnd, UINT uVirtualKey)
{
	if (uVirtualKey == VK_MENU) {
		PostMessage(hTargetWnd, WM_SYSKEYDOWN, uVirtualKey, MakeKeyEventLParam(FALSE, TRUE, TRUE, uVirtualKey));
	} else {
		PostMessage(hTargetWnd, WM_KEYDOWN, uVirtualKey, MakeKeyEventLParam(FALSE, TRUE, FALSE, uVirtualKey));
	}
	return 0;
}

// キー入力ウィンドウとマウス入力ウィンドウが同一の場合など
UINT VirtualKeyDownWithSendInput(HWND hTargetWnd, UINT uVirtualKey)
{
	WORD wScan = (WORD)MapVirtualKey(uVirtualKey, 0);
	DWORD dwFlags = (GetKeyboardType(0) == IBM_EXTENDED_KEYBOARD) ? KEYEVENTF_EXTENDEDKEY : 0;
#if UNICODE || _UNICODE
	dwFlags |= KEYEVENTF_UNICODE;
#endif
	ULONG_PTR dwExtraInfo = GetMessageExtraInfo();

	INPUT input[1];
	input[0].type			= INPUT_KEYBOARD;
	input[0].ki.wVk			= (WORD)uVirtualKey;
	input[0].ki.wScan		= wScan;
	input[0].ki.dwFlags		= dwFlags;
	input[0].ki.time		= 0;
	input[0].ki.dwExtraInfo	= dwExtraInfo;

	SetFocus(hTargetWnd);
	return SendInput(sizeof(input)/sizeof(input[0]), input, sizeof(input[0]));
}


void WINAPI VMMouseClick(const VMMouseMessage* mouseMessage, BOOL release) {
	static DWORD mouseDownMessage = 0;
	static DWORD mouseUpMessage = 0;
	static HWND lastTargetWnd = NULL;
	static BOOL bLastUsePostMessage = FALSE;

	if (release) {
		if (mouseDownMessage != 0) {
			VMMouseButtonUp(lastTargetWnd, bLastUsePostMessage, mouseUpMessage, &mouseMessage->dragEndPos);
			mouseDownMessage = 0;
			mouseUpMessage = 0;
			lastTargetWnd = NULL;
			bLastUsePostMessage = FALSE;
		}
		return;
	}

	if (mouseMessage->bUsePostMessage) {
		switch (mouseMessage->dragButton) {
		case LButtonDrag:
			mouseDownMessage = WM_LBUTTONDOWN;
			mouseUpMessage = WM_LBUTTONUP;
			break;
		case MButtonDrag:
			mouseDownMessage = WM_MBUTTONDOWN;
			mouseUpMessage = WM_MBUTTONUP;
			break;
		case RButtonDrag:
			mouseDownMessage = WM_RBUTTONDOWN;
			mouseUpMessage = WM_RBUTTONUP;
			break;
		}
		PostMessage(mouseMessage->hTargetWnd, mouseDownMessage, mouseMessage->uKeyState, MAKELPARAM(mouseMessage->dragStartPos.x, mouseMessage->dragStartPos.y));
	} else {
		switch (mouseMessage->dragButton) {
		case LButtonDrag:
			mouseDownMessage = MOUSEEVENTF_LEFTDOWN;
			mouseUpMessage = MOUSEEVENTF_LEFTUP;
			break;
		case MButtonDrag:
			mouseDownMessage = MOUSEEVENTF_MIDDLEDOWN;
			mouseUpMessage = MOUSEEVENTF_MIDDLEUP;
			break;
		case RButtonDrag:
			mouseDownMessage = MOUSEEVENTF_RIGHTDOWN;
			mouseUpMessage = MOUSEEVENTF_RIGHTUP;
			break;
		}
		ActMoveMouse(mouseMessage->dragStartPos.x, mouseMessage->dragStartPos.y);
		ActMouse(mouseDownMessage);
	}
	lastTargetWnd = mouseMessage->hTargetWnd;
	bLastUsePostMessage = mouseMessage->bUsePostMessage;
}

void WINAPI VMMouseMove(const VMMouseMessage* mouseMessage/*, int reduceCount*/) {
	if (mouseMessage->bUsePostMessage) {
		PostMessage(mouseMessage->hTargetWnd, WM_MOUSEMOVE, mouseMessage->uKeyState, MAKELPARAM(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y));
	} else {
		ActMoveMouse(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y);
	}
}

BOOL VMMouseDragWithSendInput(const VMMouseMessage* mouseMessage, int reduceCount)
{
	static DWORD mouseDownMessage = 0;
	static DWORD mouseUpMessage = 0;
	static DWORD counter = 0;

	static VMMouseMessage lastMouseMessage = {0};

	if (mouseMessage == NULL) {	// reset
		if (mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, FALSE, mouseUpMessage, &lastMouseMessage.dragEndPos);
		}
		mouseDownMessage = 0;
		mouseUpMessage = 0;
		counter = 0;
		return TRUE;
	}

	// 保存
	CopyMemory(&lastMouseMessage, mouseMessage, sizeof(VMMouseMessage));
	
	switch (mouseMessage->dragButton) {
	case LButtonDrag:
		if (mouseDownMessage != MOUSEEVENTF_LEFTDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, FALSE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = MOUSEEVENTF_LEFTDOWN;
		mouseUpMessage = MOUSEEVENTF_LEFTUP;
		break;

	case MButtonDrag:
		if (mouseDownMessage != MOUSEEVENTF_MIDDLEDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, FALSE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = MOUSEEVENTF_MIDDLEDOWN;
		mouseUpMessage = MOUSEEVENTF_MIDDLEUP;
		break;

	case RButtonDrag:
		if (mouseDownMessage != MOUSEEVENTF_RIGHTDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, FALSE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = MOUSEEVENTF_RIGHTDOWN;
		mouseUpMessage = MOUSEEVENTF_RIGHTUP;
		break;

	default:
		return FALSE;
	}

	SetFocus(mouseMessage->hTargetWnd);

	ActMoveMouse(mouseMessage->dragStartPos.x, mouseMessage->dragStartPos.y);
	ActMouse(mouseDownMessage);
	ActMoveMouse(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y);
	if (reduceCount == 1 || counter % reduceCount != 0) {
		ActMouse(mouseUpMessage);
		mouseDownMessage = 0;
	}

	counter++;
	return TRUE;
}

BOOL VMMouseDragWithPostMessage(const VMMouseMessage* mouseMessage, int reduceCount)
{
	static DWORD mouseDownMessage = 0;
	static DWORD mouseUpMessage = 0;
	static DWORD counter = 0;

	static VMMouseMessage lastMouseMessage = {0};

	if (mouseMessage == NULL) {	// reset
		if (mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, TRUE, mouseUpMessage, &lastMouseMessage.dragEndPos);
		}
		mouseDownMessage = 0;
		mouseUpMessage = 0;
		counter = 0;
		return TRUE;
	}

	// 保存
	CopyMemory(&lastMouseMessage, mouseMessage, sizeof(VMMouseMessage));

	switch (mouseMessage->dragButton) {
	case LButtonDrag:
		if (mouseDownMessage != WM_LBUTTONDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, TRUE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = WM_LBUTTONDOWN;
		mouseUpMessage = WM_LBUTTONUP;
		break;

	case MButtonDrag:
		if (mouseDownMessage != WM_MBUTTONDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, TRUE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = WM_MBUTTONDOWN;
		mouseUpMessage = WM_MBUTTONUP;
		break;

	case RButtonDrag:
		if (mouseDownMessage != WM_RBUTTONDOWN && mouseDownMessage != 0) {
			VMMouseButtonUp(lastMouseMessage.hTargetWnd, TRUE, mouseUpMessage, &lastMouseMessage.dragEndPos);
			counter = 0;
		}
		mouseDownMessage = WM_RBUTTONDOWN;
		mouseUpMessage = WM_RBUTTONUP;
		break;

	default:
		return FALSE;
	}

	if (reduceCount == 1 || counter % reduceCount == 0) {	// reduceCount == 1を特別に記しているのは、処理速度アップのためです。
		PostMessage(mouseMessage->hTargetWnd, mouseDownMessage,
			mouseMessage->uKeyState, MAKELPARAM(mouseMessage->dragStartPos.x, mouseMessage->dragStartPos.y));
	}
	PostMessage(mouseMessage->hTargetWnd, WM_MOUSEMOVE,
		mouseMessage->uKeyState, MAKELPARAM(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y));
	if (reduceCount == 1 || counter % reduceCount != 0) {
		PostMessage(mouseMessage->hTargetWnd, mouseUpMessage, 0, MAKELPARAM(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y));
		mouseDownMessage = 0;
	}
	counter++;
	return TRUE;
}


void ActMouse(DWORD type) {
	INPUT input[1] = {0};
	MOUSEINPUT mi[1] = {0};

	mi[0].dwFlags = type;
	input[0].type = INPUT_MOUSE;
	input[0].mi = mi[0];
	SendInput(1, input, sizeof(INPUT));
	Sleep(0);
}

void ActMoveMouse(int x, int y) {
	INPUT input[1] = {0};
	MOUSEINPUT mi[1] = {0};

	mi[0].dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	mi[0].dx = MOUSE_EVENT_X(x);
	mi[0].dy = MOUSE_EVENT_Y(y);
	input[0].type = INPUT_MOUSE;
	input[0].mi = mi[0];
	SendInput(1, input, sizeof(INPUT));
	Sleep(0);
}
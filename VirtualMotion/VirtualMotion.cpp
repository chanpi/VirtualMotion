/**
 * VirtualMotion.cpp
 * 
 * Copyright (c) 2008 by <your name/ organization here>
 */
// VirtualMotion.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include "VirtualMotion.h"

#define cRepeatON				(0x0001)
#define IBM_EXTENDED_KEYBOARD	(4)

static int g_isExtendedKey = -1;

static LPARAM MakeKeyEventLParam(BOOL bKeyUp, BOOL bPrevKeyState, BOOL bAltDown, USHORT uVirtualKey);
static UINT VirtualKeyDownWithPostMessage(HWND hTargetWnd, UINT uVirtualKey);
static UINT VirtualKeyDownWithSendInput(HWND hTargetWnd, UINT uVirtualKey);


/**
 * @brief
 * ���z�I�Ɏw�肳�ꂽ�L�[�R�[�h�̃L�[�������������܂��B
 * 
 * @param hTargetWnd
 * �L�[�����Ώۂ̃E�B���h�E�n���h���B
 * 
 * @param uVirtualKey
 * �Ώۂ̃L�[�R�[�h�B
 * 
 * @param bUsePostMessage
 * PostMessage�ɂ��L�[�������s���ꍇ��TRUE�ASendInput���g�p����ꍇ�ɂ�FALSE���w�肵�܂��B
 * �i�L�[���͂ƃ}�E�X���͂̈قȂ�E�B���h�E[RTT�n]�̏ꍇ��PostMessage�A����̏ꍇ[Autodesk�n]��SendInput���g�p����悤�ɂ��܂��j
 * 
 * @returns
 * PostMessage�܂���SendInput�̌��ʁB
 * 
 * ���z�I�Ɏw�肳�ꂽ�L�[�R�[�h�̃L�[�������������܂��B
 * 
 * @remarks
 * VMVirtualKeyUp()�ŃL�[�̉������������܂��B
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
 * ���z�I�Ɏw�肳�ꂽ�L�[�̉������������܂��B
 * 
 * @param hTargetWnd
 * �L�[���������Ώۂ̃E�B���h�E�n���h���B
 * 
 * @param uVirtualKey
 * �Ώۂ̃L�[�R�[�h�B
 * 
 * @returns
 * ���삪���������ꍇ�ɂ�1�A���s�����ꍇ�ɂ�0��Ԃ��܂��B
 * 
 * ���z�I�Ɏw�肳�ꂽ�L�[�̉������������܂��B
 * 
 * @remarks
 * VMVirtualKeyDown()�ɂ���ĉ����������z�L�[�R�[�h���ʂ̃L�[�𗣂��ꍇ�Ɏg�p���܂��B
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
 * �L�[�̖��O���牼�z�L�[�R�[�h���擾���܂��B
 * 
 * @param szKey
 * �擾�Ώۂ̃L�[���B
 * 
 * @returns
 * �L�[�R�[�h�A���s�����ꍇ�ɂ�0�B
 * 
 * �L�[�̖��O����L�[�R�[�h���擾���܂��B
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
 * ���z�I�ɃL�[[Alt�Ȃǂ̏C���L�[��������ʃL�[]���������܂��B
 * 
 * @param hTargetWnd
 * �L�[�����Ώۂ̃E�B���h�E�n���h���B
 * 
 * @param uKey
 * �Ώۂ̃L�[�R�[�h['A'��'1'�Ȃ�]�B
 * 
 * @param dwMilliseconds
 * ������̃X���[�v����[msec]�B
 * 
 * ���z�I�ɃL�[���������܂��B
 * 
 * @remarks
 * ��������������ɂ�VMKeyUp()���Ăяo���܂��B
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
 * ���z�L�[�̉������������܂��B
 * 
 * @param hTargetWnd
 * �L�[���������Ώۂ̃E�B���h�E�n���h���B
 * 
 * @param uKey
 * �Ώۂ̃L�[�R�[�h�B
 * 
 * ���z�L�[�̉������������܂��B
 * 
 * @remarks
 * ��������ɂ�VMKeyDown()���Ăяo���܂��B
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
 * ���z�I�Ƀ}�E�X�h���b�O���������܂��B
 * 
 * @param mouseMessage
 * �K�v�ȃ}�E�X���ւ̃|�C���^�B
 * 
 * @returns
 * �L�[����ƃ}�E�X���상�b�Z�[�W�̑��M���s�����ꍇ�ɂ�TRUE�A�s���Ȃ������ꍇ�ɂ�FALSE���Ԃ�܂��B
 * 
 * ���z�I�Ƀ}�E�X�h���b�O���������܂��B
 * �L�[����->�}�E�X�N���b�N/MOVE/�N���b�N����->�L�[�𗣂��B
 * 
 */
BOOL WINAPI VMMouseDrag(const VMMouseMessage* mouseMessage)
{
	UINT mouseDownMessage;
	UINT mouseButtonState;
	UINT mouseUpMessage;
	switch (mouseMessage->dragButton) {
	case LButtonDrag:
		mouseDownMessage = WM_LBUTTONDOWN;
		mouseButtonState = MK_LBUTTON;
		mouseUpMessage = WM_LBUTTONUP;
		break;

	case MButtonDrag:
		mouseDownMessage = WM_MBUTTONDOWN;
		mouseButtonState = MK_MBUTTON;
		mouseUpMessage = WM_MBUTTONUP;
		break;

	case RButtonDrag:
		mouseDownMessage = WM_RBUTTONDOWN;
		mouseButtonState = MK_RBUTTON;
		mouseUpMessage = WM_RBUTTONUP;
		break;

	default:
		return FALSE;
	}

	PostMessage(mouseMessage->hTargetWnd, mouseDownMessage,
		mouseMessage->uKeyState | mouseButtonState, MAKELPARAM(mouseMessage->dragStartPos.x, mouseMessage->dragStartPos.y));
	PostMessage(mouseMessage->hTargetWnd, WM_MOUSEMOVE,
		mouseMessage->uKeyState | mouseButtonState, MAKELPARAM(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y));
	PostMessage(mouseMessage->hTargetWnd, mouseUpMessage,
		(mouseUpMessage == WM_RBUTTONUP) ? mouseMessage->uKeyState : 0, MAKELPARAM(mouseMessage->dragEndPos.x, mouseMessage->dragEndPos.y));

	return TRUE;
}

// bKeyUp: WM_SYSKEYUP/WM_KEYUP�Ȃ�1
// bPrevKeyState: �O��L�[��������Ă����ԂȂ�1�iWM_SYSKEYUP/WM_KEYUP�̎��͕K��1�j
// bAltDown: Alt�L�[��������Ă��邩�iAlt�L�[��WM_SYSKEYUP/WM_KEYUP�̎��͕K��0�j
LPARAM MakeKeyEventLParam(BOOL bKeyUp, BOOL bPrevKeyState, BOOL bAltDown, USHORT uVirtualKey)
{
	LPARAM lParam = 0;
	if (g_isExtendedKey == -1) {
		if (GetKeyboardType(0) == IBM_EXTENDED_KEYBOARD) {	// 0:�L�[�{�[�h�̃^�C�v���擾
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

// �L�[���̓E�B���h�E�ƃ}�E�X���̓E�B���h�E��������Ă���ꍇ�Ɍ��ʓI�iRTT�n�Ȃǁj
UINT VirtualKeyDownWithPostMessage(HWND hTargetWnd, UINT uVirtualKey)
{
	if (uVirtualKey == VK_MENU) {
		PostMessage(hTargetWnd, WM_SYSKEYDOWN, uVirtualKey, MakeKeyEventLParam(FALSE, TRUE, TRUE, uVirtualKey));
	} else {
		PostMessage(hTargetWnd, WM_KEYDOWN, uVirtualKey, MakeKeyEventLParam(FALSE, TRUE, FALSE, uVirtualKey));
	}
	return 0;
}

// �L�[���̓E�B���h�E�ƃ}�E�X���̓E�B���h�E������̏ꍇ�Ɍ��ʓI�iAutodesk�n�Ȃǁj
UINT VirtualKeyDownWithSendInput(HWND hTargetWnd, UINT uVirtualKey)
{
	WORD wScan = (WORD)MapVirtualKey(uVirtualKey, 0);
	DWORD dwFlags = (GetKeyboardType(0) == IBM_EXTENDED_KEYBOARD) ? KEYEVENTF_EXTENDEDKEY : 0;
#if UNICODE || _UNICODE
	dwFlags |= KEYEVENTF_UNICODE;
#endif
	ULONG_PTR dwExtraInfo = GetMessageExtraInfo();

	INPUT input[2];
	input[0].type			= INPUT_KEYBOARD;
	input[0].ki.wVk			= (WORD)uVirtualKey;
	input[0].ki.wScan		= wScan;
	input[0].ki.dwFlags		= dwFlags;
	input[0].ki.time		= 0;
	input[0].ki.dwExtraInfo	= dwExtraInfo;

	input[1].type			= INPUT_KEYBOARD;
	input[1].ki.wVk			= (WORD)uVirtualKey;
	input[1].ki.wScan		= wScan;
	input[1].ki.dwFlags		= dwFlags;
	input[1].ki.time		= 0;
	input[1].ki.dwExtraInfo	= dwExtraInfo;

	//input[2].type			= INPUT_KEYBOARD;
	//input[2].ki.wVk			= (WORD)uVirtualKey;
	//input[2].ki.wScan		= wScan;
	//input[2].ki.dwFlags		= dwFlags;
	//input[2].ki.time		= 0;
	//input[2].ki.dwExtraInfo	= dwExtraInfo;

	//input[3].type			= INPUT_KEYBOARD;
	//input[3].ki.wVk			= (WORD)uVirtualKey;
	//input[3].ki.wScan		= wScan;
	//input[3].ki.dwFlags		= dwFlags;
	//input[3].ki.time		= 0;
	//input[3].ki.dwExtraInfo	= dwExtraInfo;

	SetFocus(hTargetWnd);

	return SendInput(sizeof(input)/sizeof(input[0]), input, sizeof(input[0]));
}
#ifndef _VIRTUAL_MOTION_
#define _VIRTUAL_MOTION_

#pragma comment(linker, "/export:VMGetVirtualKey=_VMGetVirtualKey@4")
#pragma comment(linker, "/export:VMKeyDown=_VMKeyDown@12")
#pragma comment(linker, "/export:VMKeyUp=_VMKeyUp@8")
//#pragma comment(linker, "/export:VMMouseButtonUp=_VMMouseButtonUp@16")
#pragma comment(linker, "/export:VMMouseDrag=_VMMouseDrag@8")
#pragma comment(linker, "/export:VMMouseClick=_VMMouseClick@8")
#pragma comment(linker, "/export:VMMouseMove=_VMMouseMove@4")
//#pragma comment(linker, "/export:VMMouseEvent=_VMMouseEvent@16")
#pragma comment(linker, "/export:VMVirtualKeyDown=_VMVirtualKeyDown@12")
#pragma comment(linker, "/export:VMVirtualKeyUp=_VMVirtualKeyUp@8")

#include <Windows.h>

#undef DLL_EXPORT
#ifdef VIRTUALMOTION_EXPORTS
#define DLL_EXPORT		__declspec(dllexport)
#else
#define DLL_EXPORT		__declspec(dllimport)
#endif

typedef enum {
	LButtonDrag,
	MButtonDrag,
	RButtonDrag,
	DragNONE,
} VMDragButton;

typedef struct {
	BOOL bUsePostMessage;
	HWND hTargetWnd;
	VMDragButton dragButton;
	UINT uKeyState;
	POINT dragStartPos;
	POINT dragEndPos;
} VMMouseMessage;


#ifdef __cplusplus
extern "C" {
#endif

	// 仮想キーを押す
	// アルファベットキーを押す/離す場合は、uVirtualKeyに大文字を指定してください。（大文字入力の場合は先にShiftを押下する必要があります）
	DLL_EXPORT UINT WINAPI VMVirtualKeyDown(HWND hTargetWnd, UINT uVirtualKey, BOOL bUsePostMessage);
	DLL_EXPORT UINT WINAPI VMVirtualKeyUp(HWND hTargetWnd, UINT uVirtualKey);

	// 文字列から仮想キーを取得する
	DLL_EXPORT UINT WINAPI VMGetVirtualKey(PCTSTR szKey);

	// 仮想キー以外を押す
	// アルファベットキーを押す/離す場合は、uKeyに大文字を指定してください。（大文字入力の場合は先にShiftを押下する必要があります）
	DLL_EXPORT void WINAPI VMKeyDown(HWND hTargetWnd, UINT uKey, DWORD dwMilliseconds);
	DLL_EXPORT void WINAPI VMKeyUp(HWND hTargetWnd, UINT uKey);
	
	// マウスクリック
	DLL_EXPORT void WINAPI VMMouseClick(const VMMouseMessage* mouseMessage, BOOL release);
	DLL_EXPORT void WINAPI VMMouseMove(const VMMouseMessage* mouseMessage/*, int reduceCount*/);
	// マウスドラッグ
	//DLL_EXPORT BOOL WINAPI VMMouseEvent(const VMMouseMessage* mouseMessage, int reduceCount, BOOL resetPostMessage, BOOL resetSendInput);
	DLL_EXPORT BOOL WINAPI VMMouseDrag(const VMMouseMessage* mouseMessage, int reduceCount = 1);

#ifdef __cplusplus
}
#endif

#endif /* _VIRTUAL_MOTION_ */
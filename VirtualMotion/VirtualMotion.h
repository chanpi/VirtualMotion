#ifndef _VIRTUAL_MOTION_
#define _VIRTUAL_MOTION_

#pragma comment(linker, "/export:VMGetVirtualKey=_VMGetVirtualKey@4")
#pragma comment(linker, "/export:VMKeyDown=_VMKeyDown@12")
#pragma comment(linker, "/export:VMKeyUp=_VMKeyUp@8")
#pragma comment(linker, "/export:VMMouseDrag=_VMMouseDrag@4")
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
} VMDragButton;

typedef struct {
	HWND hTargetWnd;
	VMDragButton dragButton;
	UINT uKeyState;
	POINT dragStartPos;
	POINT dragEndPos;
} VMMouseMessage;


#ifdef __cplusplus
extern "C" {
#endif

	// ���z�L�[������
	// �A���t�@�x�b�g�L�[������/�����ꍇ�́AuVirtualKey�ɑ啶�����w�肵�Ă��������B�i�啶�����͂̏ꍇ�͐��Shift����������K�v������܂��j
	DLL_EXPORT UINT WINAPI VMVirtualKeyDown(HWND hTargetWnd, UINT uVirtualKey, BOOL bUsePostMessage);
	DLL_EXPORT UINT WINAPI VMVirtualKeyUp(HWND hTargetWnd, UINT uVirtualKey);

	// �����񂩂牼�z�L�[���擾����
	DLL_EXPORT UINT WINAPI VMGetVirtualKey(PCTSTR szKey);

	// ���z�L�[�ȊO������
	// �A���t�@�x�b�g�L�[������/�����ꍇ�́AuKey�ɑ啶�����w�肵�Ă��������B�i�啶�����͂̏ꍇ�͐��Shift����������K�v������܂��j
	DLL_EXPORT void WINAPI VMKeyDown(HWND hTargetWnd, UINT uKey, DWORD dwMilliseconds);
	DLL_EXPORT void WINAPI VMKeyUp(HWND hTargetWnd, UINT uKey);
	
	// �}�E�X�h���b�O
	DLL_EXPORT BOOL WINAPI VMMouseDrag(const VMMouseMessage* mouseMessage);

#ifdef __cplusplus
}
#endif

#endif /* _VIRTUAL_MOTION_ */
#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>


#define PRINTL(text) std::cout << text << std::endl;
#define PRINT(text) std::cout << text;

#define PAUSE getchar();

struct Settings
{
	int BLinnPhong = false;
	int Phong = false;
	int CookTorrance = true;
	int na = false;
	int ShowGbuffer = 0;
	int ONDiffuse = 0;
	int Lambertian = 0;
};


namespace Keys
{
	int key(int iKey);
	int onekey(int iKey);
	extern char kp[256];
}

//Safe release from http://www.3dgep.com/introduction-to-directx-11/#DXGI
//Safely release a COM object.
template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}


typedef uint32_t uint32;
typedef uint8_t uint8;



//INPUTS//
#pragma once
#define VKEY_LEFT_BUTTON 0x01
#define VKEY_RIGHT_BUTTON 0x02
#define VKEY_MIDDLE_BUTTON 0x04

#define VKEY_CANCEL 0x03
#define VKEY_BACKSPACE 0x08
#define VKEY_TAB 0x09
#define VKEY_CLEAR 0x0C
#define VKEY_ENTER 0x0D
#define VKEY_SHIFT 0x10
#define VKEY_CONTROL 0x11
#define VKEY_ALT 0x12
#define VKEY_PAUSE 0x13
#define VKEY_CAPS_LOCK 0x14
#define VKEY_ESCAPE 0x1B
#define VKEY_SPACEBAR 0x20
#define VKEY_PAGE_UP 0x21
#define VKEY_PAGE_DOWN 0x22
#define VKEY_END 0x23
#define VKEY_HOME 0x24
#define VKEY_LEFT_ARROW 0x25
#define VKEY_UP_ARROW 0x26
#define VKEY_RIGHT_ARROW 0x27
#define VKEY_DOWN_ARROW 0x28
#define VKEY_SELECT 0x29
#define VKEY_PRINT 0x2A
#define VKEY_EXECUTE 0x2B
#define VKEY_PRINT_SCREEN 0x2C
#define VKEY_INSERT 0x2D
#define VKEY_DEL 0x2E
#define VKEY_HELP 0x2F
#define VKEY_SCROLL_LOCK 0x90
#define VKEY_NUM_LOCK 0x91
#define VKEY_LEFT_SHIFT 0xA0
#define VKEY_RIGHT_SHIFT 0xA1
#define VKEY_LEFT_CONTROL 0xA2
#define VKEY_RIGHT_CONTROL 0xA3
#define VKEY_LEFT_ALT 0xA4
#define VKEY_RIGHT_ALT 0xA5

#define VKEY_KEY0 0x30
#define VKEY_KEY1 0x31
#define VKEY_KEY2 0x32
#define VKEY_KEY3 0x33
#define VKEY_KEY4 0x34
#define VKEY_KEY5 0x35
#define VKEY_KEY6 0x36
#define VKEY_KEY7 0x37
#define VKEY_KEY8 0x38
#define VKEY_KEY9 0x39

#define VKEY_NUM0 0x60
#define VKEY_NUM1 0x61
#define VKEY_NUM2 0x62
#define VKEY_NUM3 0x63
#define VKEY_NUM4 0x64
#define VKEY_NUM5 0x65
#define VKEY_NUM6 0x66
#define VKEY_NUM7 0x67
#define VKEY_NUM8 0x68
#define VKEY_NUM9 0x69
#define VKEY_ASTERIX 0x6A
#define VKEY_PLUS 0x6B
#define VKEY_DASH 0x6C
#define VKEY_MINUS 0x6D
#define VKEY_DECIMAL 0x6E
#define VKEY_FSLASH 0x6F

#define VKEY_F1 0x70
#define VKEY_F2 0x71
#define VKEY_F3 0x72
#define VKEY_F4 0x73
#define VKEY_F5 0x74
#define VKEY_F6 0x75
#define VKEY_F7 0x76
#define VKEY_F8 0x77
#define VKEY_F9 0x78
#define VKEY_F10 0x79
#define VKEY_F11 0x7A
#define VKEY_F12 0x7B
#define VKEY_F13 0x7C
#define VKEY_F14 0x7D
#define VKEY_F15 0x7E
#define VKEY_F16 0x7F
#define VKEY_F17 0x80
#define VKEY_F18 0x81
#define VKEY_F19 0x82
#define VKEY_F20 0x83
#define VKEY_F21 0x84
#define VKEY_F22 0x85
#define VKEY_F23 0x86
#define VKEY_F24 0x87

#define VKEY_A 0x41
#define VKEY_B 0x42
#define VKEY_C 0x43
#define VKEY_D 0x44
#define VKEY_E 0x45
#define VKEY_F 0x46
#define VKEY_G 0x47
#define VKEY_H 0x48
#define VKEY_I 0x49
#define VKEY_J 0x4A
#define VKEY_K 0x4B
#define VKEY_L 0x4C
#define VKEY_M 0x4D
#define VKEY_N 0x4E
#define VKEY_O 0x4F
#define VKEY_P 0x50
#define VKEY_Q 0x51
#define VKEY_R 0x52
#define VKEY_S 0x53
#define VKEY_T 0x54
#define VKEY_U 0x55
#define VKEY_V 0x56
#define VKEY_W 0x57
#define VKEY_X 0x58
#define VKEY_Y 0x59
#define VKEY_Z 0x5A
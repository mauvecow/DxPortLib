using System;
using System.Runtime.InteropServices;

namespace DxLibDLL
{
	public static class DX
    {
		private const string libName = "DxPortLib.dll";

		public const int TRUE = 1;
		public const int FALSE = 0;

		public const int DXTRUE = 1;
		public const int DXFALSE = 0;

		public const int DX_SCREEN_FRONT         = -4;
		public const int DX_SCREEN_BACK          = -2;
		public const int DX_SCREEN_WORK          = -3;
		public const int DX_SCREEN_TEMPFRONT     = -5;

		public const int DX_NONE_GRAPH           = -5;

		public const int DX_BLENDMODE_NOBLEND    = (0);
		public const int DX_BLENDMODE_ALPHA      = (1);
		public const int DX_BLENDMODE_ADD        = (2);
		public const int DX_BLENDMODE_MUL        = (4);

		public const int DX_FONTTYPE_NORMAL                      = (0x00);
		public const int DX_FONTTYPE_EDGE                        = (0x01);
		public const int DX_FONTTYPE_ANTIALIASING                = (0x02);
		public const int DX_FONTTYPE_ANTIALIASING_4X4            = (0x12);
		public const int DX_FONTTYPE_ANTIALIASING_8X8            = (0x22);
		public const int DX_FONTTYPE_ANTIALIASING_EDGE           = (0x03);
		public const int DX_FONTTYPE_ANTIALIASING_EDGE_4X4       = (0x13);
		public const int DX_FONTTYPE_ANTIALIASING_EDGE_8X8       = (0x23);

		public const int DX_INPUT_PAD1           = (0x0001);
		public const int DX_INPUT_PAD2           = (0x0002);
		public const int DX_INPUT_PAD3           = (0x0003);
		public const int DX_INPUT_PAD4           = (0x0004);
		public const int DX_INPUT_PAD5           = (0x0005);
		public const int DX_INPUT_PAD6           = (0x0006);
		public const int DX_INPUT_PAD7           = (0x0007);
		public const int DX_INPUT_PAD8           = (0x0008);
		public const int DX_INPUT_PAD9           = (0x0009);
		public const int DX_INPUT_PAD10          = (0x000a);
		public const int DX_INPUT_PAD11          = (0x000b);
		public const int DX_INPUT_PAD12          = (0x000c);
		public const int DX_INPUT_PAD13          = (0x000d);
		public const int DX_INPUT_PAD14          = (0x000e);
		public const int DX_INPUT_PAD15          = (0x000f);
		public const int DX_INPUT_PAD16          = (0x0010);
		public const int DX_INPUT_KEY            = (0x1000);
		public const int DX_INPUT_KEY_PAD1       = (0x1001);

		public const int PAD_INPUT_DOWN          = (0x00000001);
		public const int PAD_INPUT_LEFT          = (0x00000002);
		public const int PAD_INPUT_RIGHT         = (0x00000004);
		public const int PAD_INPUT_UP            = (0x00000008);
		public const int PAD_INPUT_1             = (0x00000010);
		public const int PAD_INPUT_2             = (0x00000020);
		public const int PAD_INPUT_3             = (0x00000040);
		public const int PAD_INPUT_4             = (0x00000080);
		public const int PAD_INPUT_5             = (0x00000100);
		public const int PAD_INPUT_6             = (0x00000200);
		public const int PAD_INPUT_7             = (0x00000400);
		public const int PAD_INPUT_8             = (0x00000800);
		public const int PAD_INPUT_9             = (0x00001000);
		public const int PAD_INPUT_10            = (0x00002000);
		public const int PAD_INPUT_11            = (0x00004000);
		public const int PAD_INPUT_12            = (0x00008000);
		public const int PAD_INPUT_13            = (0x00010000);
		public const int PAD_INPUT_14            = (0x00020000);
		public const int PAD_INPUT_15            = (0x00040000);
		public const int PAD_INPUT_16            = (0x00080000);
		public const int PAD_INPUT_17            = (0x00100000);
		public const int PAD_INPUT_18            = (0x00200000);
		public const int PAD_INPUT_19            = (0x00400000);
		public const int PAD_INPUT_20            = (0x00800000);
		public const int PAD_INPUT_21            = (0x01000000);
		public const int PAD_INPUT_22            = (0x02000000);
		public const int PAD_INPUT_23            = (0x04000000);
		public const int PAD_INPUT_24            = (0x08000000);
		public const int PAD_INPUT_25            = (0x10000000);
		public const int PAD_INPUT_26            = (0x20000000);
		public const int PAD_INPUT_27            = (0x40000000);
		public const int PAD_INPUT_28            = -(0x80000000);

		public const int PAD_INPUT_A             = PAD_INPUT_1;
		public const int PAD_INPUT_B             = PAD_INPUT_2;
		public const int PAD_INPUT_C             = PAD_INPUT_3;
		public const int PAD_INPUT_X             = PAD_INPUT_4;
		public const int PAD_INPUT_Y             = PAD_INPUT_5;
		public const int PAD_INPUT_Z             = PAD_INPUT_6;
		public const int PAD_INPUT_L             = PAD_INPUT_7;
		public const int PAD_INPUT_R             = PAD_INPUT_8;
		public const int PAD_INPUT_START         = PAD_INPUT_9;
		public const int PAD_INPUT_M             = PAD_INPUT_10;
		public const int PAD_INPUT_D             = PAD_INPUT_11;
		public const int PAD_INPUT_F             = PAD_INPUT_12;
		public const int PAD_INPUT_G             = PAD_INPUT_13;
		public const int PAD_INPUT_H             = PAD_INPUT_14;
		public const int PAD_INPUT_I             = PAD_INPUT_15;
		public const int PAD_INPUT_J             = PAD_INPUT_16;
		public const int PAD_INPUT_K             = PAD_INPUT_17;
		public const int PAD_INPUT_LL            = PAD_INPUT_18;
		public const int PAD_INPUT_N             = PAD_INPUT_19;
		public const int PAD_INPUT_O             = PAD_INPUT_20;
		public const int PAD_INPUT_P             = PAD_INPUT_21;
		public const int PAD_INPUT_RR            = PAD_INPUT_22;
		public const int PAD_INPUT_S             = PAD_INPUT_23;
		public const int PAD_INPUT_T             = PAD_INPUT_24;
		public const int PAD_INPUT_U             = PAD_INPUT_25;
		public const int PAD_INPUT_V             = PAD_INPUT_26;
		public const int PAD_INPUT_W             = PAD_INPUT_27;
		public const int PAD_INPUT_XX            = PAD_INPUT_28;

		public const int XINPUT_BUTTON_DPAD_UP           = (0);
		public const int XINPUT_BUTTON_DPAD_DOWN         = (1);
		public const int XINPUT_BUTTON_DPAD_LEFT         = (2);
		public const int XINPUT_BUTTON_DPAD_RIGHT        = (3);
		public const int XINPUT_BUTTON_START             = (4);
		public const int XINPUT_BUTTON_BACK              = (5);
		public const int XINPUT_BUTTON_LEFT_THUMB        = (6);
		public const int XINPUT_BUTTON_RIGHT_THUMB       = (7);
		public const int XINPUT_BUTTON_LEFT_SHOULDER     = (8);
		public const int XINPUT_BUTTON_RIGHT_SHOULDER    = (9);
		public const int XINPUT_BUTTON_A                 = (10);
		public const int XINPUT_BUTTON_B                 = (11);
		public const int XINPUT_BUTTON_X                 = (12);
		public const int XINPUT_BUTTON_Y                 = (13);

		public const int KEY_INPUT_ESCAPE        = (0x01);
		public const int KEY_INPUT_1             = (0x02);
		public const int KEY_INPUT_2             = (0x03);
		public const int KEY_INPUT_3             = (0x04);
		public const int KEY_INPUT_4             = (0x05);
		public const int KEY_INPUT_5             = (0x06);
		public const int KEY_INPUT_6             = (0x07);
		public const int KEY_INPUT_7             = (0x08);
		public const int KEY_INPUT_8             = (0x09);
		public const int KEY_INPUT_9             = (0x0a);
		public const int KEY_INPUT_0             = (0x0b);
		public const int KEY_INPUT_MINUS         = (0x0c);
		public const int KEY_INPUT_EQUALS        = (0x0d);
		public const int KEY_INPUT_BACK          = (0x0e);
		public const int KEY_INPUT_TAB           = (0x0f);
		public const int KEY_INPUT_Q             = (0x10);
		public const int KEY_INPUT_W             = (0x11);
		public const int KEY_INPUT_E             = (0x12);
		public const int KEY_INPUT_R             = (0x13);
		public const int KEY_INPUT_T             = (0x14);
		public const int KEY_INPUT_Y             = (0x15);
		public const int KEY_INPUT_U             = (0x16);
		public const int KEY_INPUT_I             = (0x17);
		public const int KEY_INPUT_O             = (0x18);
		public const int KEY_INPUT_P             = (0x19);
		public const int KEY_INPUT_LBRACKET      = (0x1a);
		public const int KEY_INPUT_RBRACKET      = (0x1b);
		public const int KEY_INPUT_RETURN        = (0x1c);
		public const int KEY_INPUT_LCONTROL      = (0x1d);
		public const int KEY_INPUT_A             = (0x1e);
		public const int KEY_INPUT_S             = (0x1f);
		public const int KEY_INPUT_D             = (0x20);
		public const int KEY_INPUT_F             = (0x21);
		public const int KEY_INPUT_G             = (0x22);
		public const int KEY_INPUT_H             = (0x23);
		public const int KEY_INPUT_J             = (0x24);
		public const int KEY_INPUT_K             = (0x25);
		public const int KEY_INPUT_L             = (0x26);
		public const int KEY_INPUT_SEMICOLON     = (0x27);
		/* APOSTROPHE 0x28 */
		/* GRAVE 0x29 */
		public const int KEY_INPUT_LSHIFT        = (0x2a);
		public const int KEY_INPUT_BACKSLASH     = (0x2b);
		public const int KEY_INPUT_Z             = (0x2c);
		public const int KEY_INPUT_X             = (0x2d);
		public const int KEY_INPUT_C             = (0x2e);
		public const int KEY_INPUT_V             = (0x2f);
		public const int KEY_INPUT_B             = (0x30);
		public const int KEY_INPUT_N             = (0x31);
		public const int KEY_INPUT_M             = (0x32);
		public const int KEY_INPUT_COMMA         = (0x33);
		public const int KEY_INPUT_PERIOD        = (0x34);
		public const int KEY_INPUT_SLASH         = (0x35);
		public const int KEY_INPUT_RSHIFT        = (0x36);
		public const int KEY_INPUT_MULTIPLY      = (0x37);
		public const int KEY_INPUT_LALT          = (0x38);
		public const int KEY_INPUT_SPACE         = (0x39);
		public const int KEY_INPUT_CAPSLOCK      = (0x3a);
		public const int KEY_INPUT_F1            = (0x3b);
		public const int KEY_INPUT_F2            = (0x3c);
		public const int KEY_INPUT_F3            = (0x3d);
		public const int KEY_INPUT_F4            = (0x3e);
		public const int KEY_INPUT_F5            = (0x3f);
		public const int KEY_INPUT_F6            = (0x40);
		public const int KEY_INPUT_F7            = (0x41);
		public const int KEY_INPUT_F8            = (0x42);
		public const int KEY_INPUT_F9            = (0x43);
		public const int KEY_INPUT_F10           = (0x44);
		public const int KEY_INPUT_NUMLOCK       = (0x45);
		public const int KEY_INPUT_SCROLL        = (0x46);
		public const int KEY_INPUT_NUMPAD7       = (0x47);
		public const int KEY_INPUT_NUMPAD8       = (0x48);
		public const int KEY_INPUT_NUMPAD9       = (0x49);
		public const int KEY_INPUT_SUBTRACT      = (0x4a);
		public const int KEY_INPUT_NUMPAD4       = (0x4b);
		public const int KEY_INPUT_NUMPAD5       = (0x4c);
		public const int KEY_INPUT_NUMPAD6       = (0x4d);
		public const int KEY_INPUT_ADD           = (0x4e);
		public const int KEY_INPUT_NUMPAD1       = (0x4f);
		public const int KEY_INPUT_NUMPAD2       = (0x50);
		public const int KEY_INPUT_NUMPAD3       = (0x51);
		public const int KEY_INPUT_NUMPAD0       = (0x52);
		public const int KEY_INPUT_DECIMAL       = (0x53);

		public const int KEY_INPUT_F11           = (0x57);
		public const int KEY_INPUT_F12           = (0x58);

		public const int KEY_INPUT_NUMPADENTER   = (0x9c);
		public const int KEY_INPUT_RCONTROL      = (0x9d);
		public const int KEY_INPUT_NUMPADCOMMA   = (0xb3);
		public const int KEY_INPUT_DIVIDE        = (0xb5);
		public const int KEY_INPUT_SYSRQ         = (0xb7);
		public const int KEY_INPUT_RALT          = (0xb8);

		public const int KEY_INPUT_PAUSE         = (0xc5);
		public const int KEY_INPUT_HOME          = (0xc7);
		public const int KEY_INPUT_UP            = (0xc8);
		public const int KEY_INPUT_PGUP          = (0xc9);
		public const int KEY_INPUT_LEFT          = (0xcb);
		public const int KEY_INPUT_RIGHT         = (0xcd);
		public const int KEY_INPUT_END           = (0xcf);
		public const int KEY_INPUT_DOWN          = (0xd0);
		public const int KEY_INPUT_PGDN          = (0xd1);
		public const int KEY_INPUT_INSERT        = (0xd2);
		public const int KEY_INPUT_DELETE        = (0xd3);

		public const int MOUSE_INPUT_LEFT        = (0x01);
		public const int MOUSE_INPUT_MIDDLE      = (0x02);
		public const int MOUSE_INPUT_RIGHT       = (0x04);
		public const int MOUSE_INPUT_1           = (0x01);
		public const int MOUSE_INPUT_2           = (0x02);
		public const int MOUSE_INPUT_3           = (0x04);
		public const int MOUSE_INPUT_4           = (0x08);
		public const int MOUSE_INPUT_5           = (0x10);
		public const int MOUSE_INPUT_6           = (0x20);
		public const int MOUSE_INPUT_7           = (0x40);
		public const int MOUSE_INPUT_8            =(0x80);

		public const int DX_CHECKINPUT_KEY       = (0x01);
		public const int DX_CHECKINPUT_PAD       = (0x02);
		public const int DX_CHECKINPUT_MOUSE     = (0x04);
		public const int DX_CHECKINPUT_ALL       = (DX_CHECKINPUT_KEY | DX_CHECKINPUT_PAD | DX_CHECKINPUT_MOUSE);

		public const int DX_CHARSET_DEFAULT		 = (0x00);

		public const int DX_PLAYTYPE_LOOPBIT	 = (0x2);
		public const int DX_PLAYTYPE_BACKBIT	 = (0x1);
		public const int DX_PLAYTYPE_NORMAL		 = (0);
		public const int DX_PLAYTYPE_BACK		 = (DX_PLAYTYPE_BACKBIT);
		public const int DX_PLAYTYPE_LOOP		 = (DX_PLAYTYPE_LOOPBIT | DX_PLAYTYPE_BACKBIT);

		/* DxLib main */

		[DllImport(libName, EntryPoint = "DxLib_DxLib_Init", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DxLib_Init();

		[DllImport(libName, EntryPoint = "DxLib_DxLib_End", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DxLib_End();

		[DllImport(libName, EntryPoint = "DxLib_GlobalStructInitialize", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GlobalStructInitialize();

		[DllImport(libName, EntryPoint = "DxLib_IsInit", CallingConvention = CallingConvention.Cdecl)]
		public extern static int IsInit();

		[DllImport(libName, EntryPoint = "DxLib_ProcessMessage", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ProcessMessage();

		[DllImport(libName, EntryPoint = "DxLib_WaitTimer", CallingConvention = CallingConvention.Cdecl)]
		public extern static int WaitTimer(
			int msTime
		);
		[DllImport(libName, EntryPoint = "DxLib_WaitKey", CallingConvention = CallingConvention.Cdecl)]
		public extern static int WaitKey();

		[DllImport(libName, EntryPoint = "DxLib_GetNowCount", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetNowCount();

		[DllImport(libName, EntryPoint = "DxLib_GetRand", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetRand(
			int maxValue
		);

		[DllImport(libName, EntryPoint = "DxLib_SRand", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SRand(
			int randomSeed
		);

		[DllImport(libName, EntryPoint = "DxLib_SetOutApplicationLogValidFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetOutApplicationLogValidFlag(
			int logFlag
		);

		// SetUseCharSet deliberately omitted. UTF8 only.

		/* DxFile */
		/*
		extern DXCALL int DxLib_FileRead_open(const DXCHAR *filename);
		
		extern DXCALL long long DxLib_FileRead_size(int fileHandle);
		
		extern DXCALL int DxLib_FileRead_close(int fileHandle);
		
		extern DXCALL long long DxLib_FileRead_tell(int fileHandle);
		
		extern DXCALL int DxLib_FileRead_seek(int fileHandle,
                                      		long long position, int origin);
		
		extern DXCALL int DxLib_FileRead_read(void *data, int size, int fileHandle);
		
		extern DXCALL int DxLib_FileRead_eof(int fileHandle);
		*/

		/* DxArchive */

		[DllImport(libName, EntryPoint = "DxLib_SetUseDXArchiveFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetUseDXArchiveFlag(
			int flag
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDXArchiveKeyString", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDXArchiveKeyString(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string keyString
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDXArchiveExtension", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDXArchiveExtension(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string keyString
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDXArchivePriority", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDXArchivePriority(
			int priorityFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_DXArchivePreLoad", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DXArchivePreLoad(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string dxaFilename
		);
		[DllImport(libName, EntryPoint = "DxLib_DXArchiveCheckIdle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DXArchiveCheckIdle(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string dxaFilename
		);
		[DllImport(libName, EntryPoint = "DxLib_DXArchiveRelease", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DXArchiveRelease(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string dxaFilename
		);
		[DllImport(libName, EntryPoint = "DxLib_DXArchiveCheckFile", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DXArchiveCheckFile(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string dxaFilename,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename
		);

		/* Input */

		[DllImport(libName, EntryPoint = "DxLib_CheckHitKey", CallingConvention = CallingConvention.Cdecl)]
		public extern static int CheckHitKey(
			int keyCode
		);

		[DllImport(libName, EntryPoint = "DxLib_CheckHitKeyAll", CallingConvention = CallingConvention.Cdecl)]
		public extern static int CheckHitKeyAll(
			int checkType = DX_CHECKINPUT_ALL
		);

		//extern DXCALL int DxLib_GetHitKeyStateAll(char *table);

		[DllImport(libName, EntryPoint = "DxLib_GetJoypadNum", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetJoypadNum();

		[DllImport(libName, EntryPoint = "DxLib_GetJoypadInputState", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetJoypadInputState(
			int controllerIndex
		);

		// extern DXCALL int DxLib_GetJoypadDirectInputState(int controllerIndex, DINPUT_JOYSTATE *state);
		// extern DXCALL int DxLib_GetJoypadXInputState(int controllerIndex, XINPUT_STATE *state);

		[DllImport(libName, EntryPoint = "DxLib_SetJoypadInputToKeyInput", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetJoypadInputToKeyInput(
			int controllerIndex, int input,
			int key1, int key2 = 0,
			int key3 = 0, int key4 = 0
		);

		[DllImport(libName, EntryPoint = "DxLib_GetMousePoint", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetMousePoint(
			out int positionX, out int positionY
		);

		[DllImport(libName, EntryPoint = "DxLib_SetMousePoint", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetMousePoint(
			int positionX, int positionY
		);
		[DllImport(libName, EntryPoint = "DxLib_GetMouseInput", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetMouseInput();


		[DllImport(libName, EntryPoint = "DxLib_GetMouseWheelRotVol", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetMouseWheelRotVol(
			int clearFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetMouseHWheelRotVol", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetMouseHWheelRotVol(
			int clearFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetMouseWheelRotVolF", CallingConvention = CallingConvention.Cdecl)]
		public extern static float GetMouseWheelRotVolF(
			int clearFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetMouseHWheelRotVolF", CallingConvention = CallingConvention.Cdecl)]
		public extern static float GetMouseHWheelRotVolF(
			int clearFlag
		);

		/* DxWindow */

		[DllImport(libName, EntryPoint = "DxLib_SetGraphMode", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetGraphMode(
			int width, int height, int bitDepth, int FPS = 60
		);
		[DllImport(libName, EntryPoint = "DxLib_SetWindowSizeChangeEnableFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetWindowSizeChangeEnableFlag(
			int windowResizeFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_SetWindowText", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetWindowText(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string windowName
		);
		[DllImport(libName, EntryPoint = "DxLib_ScreenFlip", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ScreenFlip();
		[DllImport(libName, EntryPoint = "DxLib_ChangeWindowMode", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ChangeWindowMode(
			int fullscreenFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_SetDrawScreen", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDrawScreen(
			int flag
		);

		[DllImport(libName, EntryPoint = "DxLib_SetMouseDispFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetMouseDispFlag(
			int displayFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetMouseDispFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetMouseDispFlag();

		[DllImport(libName, EntryPoint = "DxLib_SetVSyncWaitFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetVSyncWaitFlag(
			int displayFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetVSyncWaitFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetVSyncWaitFlag();

		[DllImport(libName, EntryPoint = "DxLib_EXT_SetIconImageFile", CallingConvention = CallingConvention.Cdecl)]
		public extern static int EXT_SetIconImageFile(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename
		);
		[DllImport(libName, EntryPoint = "DxLib_SetAlwaysRunFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetAlwaysRunFlag(
			int displayFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_GetAlwaysRunFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetAlwaysRunFlag();

		/* DxGraphics */

		[DllImport(libName, EntryPoint = "DxLib_LoadGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int LoadGraph(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string name
		);
		[DllImport(libName, EntryPoint = "DxLib_DeleteGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DeleteGraph(
			int graphID
		);
		[DllImport(libName, EntryPoint = "DxLib_InitGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int InitGraph();
		[DllImport(libName, EntryPoint = "DxLib_DerivationGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DerivationGraph(
			int x, int y, int w, int h, int graphID
		);

		[DllImport(libName, EntryPoint = "DxLib_GetGraphSize", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetGraphSize(
			int graphID, out int width, out int height
		);

		[DllImport(libName, EntryPoint = "DxLib_SetTransColor", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetTransColor(
			int r, int g, int b
		);
		[DllImport(libName, EntryPoint = "DxLib_GetTransColor", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetTransColor(
			out int r, out int g, out int b
		);
		[DllImport(libName, EntryPoint = "DxLib_SetUseTransColor", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetUseTransColor(
			int useFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawLine", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawLine(
			int x1, int y1, int x2, int y2, int color, int thickness = 1
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawLineF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawLineF(
			float x1, float y1, float x2, float y2, int color, int thickness = 1
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawBox", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawBox(
			int x1, int y1, int x2, int y2, int color, int FillFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawBoxF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawBoxF(
			float x1, float y1, float x2, float y2, int color, int FillFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawFillBox", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawFillBox(
			int x1, int y1, int x2, int y2, int color
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawFillBoxF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawFillBoxF(
			float x1, float y1, float x2, float y2, int color
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawLineBox", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawLineBox(
			int x, int y, int w, int h, int color
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawLineBoxF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawLineBoxF(
			float x1, float y1, float x2, float y2, int color
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawCircle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawCircle(
			int x, int y, int radius, int color, int fillFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawCircleF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawCircleF(
			float x, float y, float radius, int color, int fillFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawOval", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawOval(
			int x, int y, int radiusX, int radiusY, int color, int fillFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawOvalF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawOvalF(
			float x, float y, float radiusX, float radiusY, int color, int fillFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawGraph(
			int x, int y, int graphID, int blendFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawGraphF(
			float x, float y, int graphID, int blendFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawExtendGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawExtendGraph(
			int x1, int y1, int x2, int y2, int graphID, int blendFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawExtendGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawExtendGraphF(
			float x1, float y1, float x2, float y2, int graphID, int blendFlag
		);
                     
		[DllImport(libName, EntryPoint = "DxLib_DrawRectGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectGraph(
			int dx, int dy, int sx, int sy, int sw, int sh,
            int graphID, int blendFlag, int turnFlag = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectGraphF(
			float dx, float dy, int sx, int sy, int sw, int sh,
            int graphID, int blendFlag, int turnFlag = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectExtendGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectExtendGraph(
			int dx1, int dy1, int dx2, int dy2,
            int sx, int sy, int sw, int sh,
            int graphID, int blendFlag, int turnFlag = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectExtendGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectExtendGraphF(
			float dx1, float dy1, float dx2, float dy2,
            int sx, int sy, int sw, int sh,
            int graphID, int blendFlag, int turnFlag = DXFALSE
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraph(
			int x, int y, double scaleFactor, double angle,
            int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraphF(
			float x, float y, double scaleFactor, double angle,
            int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraph2", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraph2(
			int x, int y, int cx, int cy,
			double scaleFactor, double angle,
			int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraph2F", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraph2F(
			float x, float y, float cx, float cy,
			double scaleFactor, double angle,
			int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraph3", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraph3(
			int x, int y, int cx, int cy,
			double xScaleFactor, double yScaleFactor,
            double angle, int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRotaGraph3F", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRotaGraph3F(
			float x, float y, float cx, float cy,
			double xScaleFactor, double yScaleFactor,
            double angle, int graphID, int blendFlag, int turn = DXFALSE
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraph(
			int x, int y,
			int sx, int sy, int sw, int sh, 
			double scaleFactor, double angle,
            int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraphF(
			float x, float y,
			int sx, int sy, int sw, int sh, 
			 double scaleFactor, double angle,
            int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraph2", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraph2(
			int x, int y,
			int sx, int sy, int sw, int sh, 
			int cx, int cy,
			double scaleFactor, double angle,
			int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraph2F", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraph2F(
			float x, float y,
			int sx, int sy, int sw, int sh, 
			float cx, float cy,
			double scaleFactor, double angle,
			int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraph3", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraph3(
			int x, int y,
			int sx, int sy, int sw, int sh, 
			int cx, int cy,
			double xScaleFactor, double yScaleFactor,
            double angle, int graphID, int blendFlag, int turn = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawRectRotaGraph3F", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawRectRotaGraph3F(
			float x, float y,
			int sx, int sy, int sw, int sh, 
			float cx, float cy,
			double xScaleFactor, double yScaleFactor,
            double angle, int graphID, int blendFlag, int turn = DXFALSE
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawTurnGraph", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawTurnGraph(
			int x, int y, int graphID, int blendFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawTurnGraphF", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawTurnGraphF(
			float x, float y, int graphID, int blendFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDrawArea", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDrawArea(
			int x1, int y1, int x2, int y2
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDrawBlendMode", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDrawBlendMode(
			int blendMode, int alpha
		);

		[DllImport(libName, EntryPoint = "DxLib_SetDrawBright", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDrawBright(
			int redBright, int greenBright, int blueBright
		);

		[DllImport(libName, EntryPoint = "DxLib_SetBasicBlendFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetBasicBlendFlag(
			int blendFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_GetColor", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetColor(
			int red, int green, int blue
		);

		/* Font */

		[DllImport(libName, EntryPoint = "DxLib_EXT_MapFontFileToName", CallingConvention = CallingConvention.Cdecl)]
		public extern static int EXT_MapFontFileToName(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string fontname,
			int thickness,
			int boldFlag
		);
		[DllImport(libName, EntryPoint = "DxLib_EXT_InitFontMappings", CallingConvention = CallingConvention.Cdecl)]
		public extern static int EXT_InitFontMappings();

		[DllImport(libName, EntryPoint = "DxLib_DrawStringToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawStringToHandle(
			int x, int y, 
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int color, int fontHandle,
			int edgeColor = 0
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawExtendStringToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawExtendStringToHandle(
			int x, int y, double ExRateX, double ExRateY,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int color, int fontHandle,
			int edgeColor = 0
		);
		[DllImport(libName, EntryPoint = "DxLib_GetDrawStringWidthToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetDrawStringWidthToHandle(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int strLen, int fontHandle
		);
		[DllImport(libName, EntryPoint = "DxLib_GetDrawExtendStringWidthToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetDrawExtendStringWidthToHandle(
			double ExRateX,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int strLen, int fontHandle
		);

		[DllImport(libName, EntryPoint = "DxLib_GetFontSizeToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetFontSizeToHandle(
			int fontHandle
		);
		[DllImport(libName, EntryPoint = "DxLib_GetFontCharInfo", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetFontSizeToHandle(
			int fontHandle,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			out int xPos, out int yPos, out int advanceX,
			out int width, out int height
		);
		[DllImport(libName, EntryPoint = "DxLib_SetFontSpaceToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetFontSpaceToHandle(
			int fontHandle,
			int fontSpacing
		);

		[DllImport(libName, EntryPoint = "DxLib_CreateFontToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int CreateFontToHandle(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string fontname,
			int size, int thickness,
			int fontType = -1, int charSet = -1,
			int edgeSize = -1, int Italic = DXFALSE
		);
		[DllImport(libName, EntryPoint = "DxLib_DeleteFontToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DeleteFontToHandle(
			int fontHandle
		);
		[DllImport(libName, EntryPoint = "DxLib_CheckFontHandleValid", CallingConvention = CallingConvention.Cdecl)]
		public extern static int CheckFontHandleValid(
			int fontHandle
		);
		[DllImport(libName, EntryPoint = "DxLib_InitFontToHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int InitFontToHandle(
			int fontHandle
		);

		[DllImport(libName, EntryPoint = "DxLib_DrawString", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawString(
			int x, int y, 
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int color,
			int edgeColor = 0
		);
		[DllImport(libName, EntryPoint = "DxLib_DrawExtendString", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DrawExtendString(
			int x, int y, double ExRateX, double ExRateY,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int color,
			int edgeColor = 0
		);
		[DllImport(libName, EntryPoint = "DxLib_GetDrawStringWidth", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetDrawStringWidth(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int strLen
		);
		[DllImport(libName, EntryPoint = "DxLib_GetDrawExtendStringWidth", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetDrawExtendStringWidth(
			double ExRateX,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string text,
			int strLen
		);

		[DllImport(libName, EntryPoint = "DxLib_ChangeFont", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ChangeFont(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string fontName,
			int charSet = -1
		);
		[DllImport(libName, EntryPoint = "DxLib_ChangeFontType", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ChangeFontType(
			int fontType
		);
		[DllImport(libName, EntryPoint = "DxLib_SetFontSize", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetFontSize(
			int fontSize
		);
		[DllImport(libName, EntryPoint = "DxLib_GetFontSize", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetFontSize();
		[DllImport(libName, EntryPoint = "DxLib_SetFontThickness", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetFontThickness(
			int fontThickness
		);
		[DllImport(libName, EntryPoint = "DxLib_SetFontSpace", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetFontSpace(
			int fontSpacing
		);
		[DllImport(libName, EntryPoint = "DxLib_SetDefaultFontState", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetDefaultFontState(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string fontName,
			int fontSize,
			int fontThickness
		);
		[DllImport(libName, EntryPoint = "DxLib_GetDefaultFontHandle", CallingConvention = CallingConvention.Cdecl)]
		public extern static int GetDefaultFontHandle();

		/* Sound */

		[DllImport(libName, EntryPoint = "DxLib_PlaySoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int PlaySoundMem(
			int soundID,
			int playType,
			int startPositionFlag = DXTRUE
		);
		[DllImport(libName, EntryPoint = "DxLib_StopSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int StopSoundMem(
			int soundID
		);
		[DllImport(libName, EntryPoint = "DxLib_CheckSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int CheckSoundMem(
			int soundID
		);

		[DllImport(libName, EntryPoint = "DxLib_SetVolumeSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetVolumeSoundMem(
			int volume,
			int soundID
		);
		[DllImport(libName, EntryPoint = "DxLib_ChangeVolumeSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int ChangeVolumeSoundMem(
			int volume,
			int soundID
		);
		[DllImport(libName, EntryPoint = "DxLib_SetUseOldVolumeCalcFlag", CallingConvention = CallingConvention.Cdecl)]
		public extern static int SetUseOldVolumeCalcFlag(
			int volumeFlag
		);

		[DllImport(libName, EntryPoint = "DxLib_LoadSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int LoadSoundMem(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename
		);
		[DllImport(libName, EntryPoint = "DxLib_LoadSoundMem2", CallingConvention = CallingConvention.Cdecl)]
		public extern static int LoadSoundMem2(
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename,
			[In()] [MarshalAs(UnmanagedType.LPStr)] string filename2
		);

		[DllImport(libName, EntryPoint = "DxLib_DeleteSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int DeleteSoundMem(
			int soundID
		);

		[DllImport(libName, EntryPoint = "DxLib_InitSoundMem", CallingConvention = CallingConvention.Cdecl)]
		public extern static int InitSoundMem();
	}
}


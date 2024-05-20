//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<ui/draw.h>
#include<ui/font.h>
#include<ui/framebuffer.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

extern FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Test Allocate

class TestX
{
	public:
	int i;
};

void TestAllocate()
{
	//Allocate
	TestX *Test = new TestX();
	Test->i = 5;
	LogFormatted("i = %d\r\n", Test->i);
	delete[] Test;
}

void TestAllocateAdvanced()
{
	DumpAllocationTableIntelligent();

	char *Test1 = (char *) ReserveMemory(25);
	char *Test2 = (char *) ReserveMemory(137);
	char *Test3 = (char *) ReserveMemory(295);
	char *Test4 = (char *) ReserveMemory(316);

	DumpAllocationTableIntelligent();

	FreeMemory(Test2);
	FreeMemory(Test3);

	DumpAllocationTableIntelligent();

	char *Test5 = (char *) ReserveMemory(17);
	char *Test6 = (char *) ReserveMemory(34);

	DumpAllocationTableIntelligent();

	FreeMemory(Test1);
	FreeMemory(Test4);

	DumpAllocationTableIntelligent();

	FreeMemory(Test6);
	FreeMemory(Test5);

	DumpAllocationTableIntelligent();

	DumpAllocationTableRaw();
}

//Test Alloc Advanced
//void TestAllocateAdvanced();
//TestAllocateAdvanced();
//asm("cli;hlt");

//-------------------------------------------------------------------------------------------------------------------------//
//Test Collections

#include<library/list.h>
#include<library/queue.h>

void TestCollections()
{
	/*List<int> TestList; // = new List<int>();

	TestList.AddTail(4);
	TestList.AddTail(5);
	TestList.AddTail(6);

	TestList.AddHead(1);
	TestList.AddHead(2);
	TestList.AddHead(3);

	PrintFormatted("\r\n");
	for(auto Element : TestList) PrintFormatted("List: %d\r\n", Element);

	int Head = TestList.RemoveHead();
	int Tail = TestList.RemoveTail();

	PrintFormatted("\r\n");
	for(auto Element : TestList) PrintFormatted("List: %d\r\n", Element);

	PrintFormatted("\r\n");
	PrintFormatted("HT: %d|%d\r\n", Head, Tail);

	TestList.RemoveHead();TestList.RemoveHead();TestList.RemoveHead();TestList.RemoveHead();

	PrintFormatted("\r\n");
	for(auto Element : TestList) PrintFormatted("List: %d\r\n", Element);

	TestList.RemoveHead();

	PrintFormatted("\r\n");
	for(auto Element : TestList) PrintFormatted("List: %d\r\n", Element);

	TestList.AddTail(10);

	PrintFormatted("\r\n");
	for(auto Element : TestList) PrintFormatted("List: %d\r\n", Element);*/

	List<int> TestList; // = new List<int>();

	TestList.AddTail(1);
	TestList.AddTail(3);
	TestList.AddTail(5);
	TestList.AddTail(7);

	for(auto Element : TestList)
	{
		LogFormatted("List: %d\r\n", Element);
	}

	Queue<int> TestQueue; // = new Queue<int>();

	TestQueue.Enqueue(2);
	TestQueue.Enqueue(4);
	TestQueue.Enqueue(6);
	TestQueue.Enqueue(8);

	for(auto Element : TestQueue)
	{
		LogFormatted("Queue: %d\r\n", Element);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test UI

UInt32 MousePointerBitmapTest[] =
{
	0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000
};

void TestUi()
{
	//Test
	PrintTo(&FramebufferUefi.FrontBuffer, nullptr, &FramebufferUefi.Console, 0x00000000, 0x00000000, "Hello !\r\n"); //0x00FCFCFC
	LogFormatted("Test %s %d wow !\r\n", "Hello", 123);
	LogFormatted("Size %d %d %d %d\r\n", sizeof(Int8), sizeof(Int16), sizeof(Int32), sizeof(Int64));
	LogFormatted("Size %d %d %d %d %d\r\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long long));

	//Draw Lines
	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 100, 0,   0x00FF0000);
	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 0, 100,   0x00FF0000);

	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 100, 100, 0x000000FF);

	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 200, 100, 0x000000FF);
	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 100, 200, 0x000000FF);

	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 350, 100, 0x000000FF);
	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 100, 350, 0x000000FF);

	DrawLine(&FramebufferUefi.FrontBuffer, nullptr, 0, 0, 0, 0,     0x0000FF00);

	//Draw Rectangle
	int Height = 100;
	int Width = 100;
	int PosY = FramebufferUefi.FrontBuffer.Height / 2 - Height / 2;
	int PosX = FramebufferUefi.FrontBuffer.Width / 2 - Width / 2;
	DrawSelection SelectionRectangle
	{
		.Y = PosY,
		.X = PosX,
		.H = Height,
		.W = Width
	};
	DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionRectangle, 2, 0x002C2C2C, 0x00007ACC); //0x00784343, 0x00EFF0F1

	//Copy Bitmap
	{
		DrawSelection SelectionFrom
		{
			.Y = PosY,
			.X = PosX,
			.H = Height,
			.W = Width
		};
		DrawSelection SelectionTo
		{
			.Y = PosY + 150,
			.X = PosX + 250,
			.H = Height,
			.W = Width
		};
		CopyBitmap(&FramebufferUefi.FrontBuffer, &FramebufferUefi.FrontBuffer, &SelectionFrom, &SelectionTo);
	}

	//Mouse
	{
		DrawSelection SelectionFrom
		{
			.Y = 0,
			.X = 0,
			.H = 20,
			.W = 12
		};
		DrawSelection SelectionTo
		{
			.Y = PosY - 5,
			.X = PosX - 3,
			.H = 20,
			.W = 12
		};
		DrawSurface Mouse
		{
			.Buffer = MousePointerBitmapTest,
			.BitsPerPixel = 32,
			.Height = 20,
			.Width = 12
		};
		CopyBitmapAlpha(&Mouse, &FramebufferUefi.FrontBuffer, &SelectionFrom, &SelectionTo);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test Desktop

void TestDesktop()
{
	//Desktop      0xFF83ACC8
	//Window Area  0xFFEFF4F9
	//Window Frame 0xFF7B919F
	//Text         0xFF191A1A
	//Button       0xFF0067C0
	//Box          0xFFFAFCFD

	//Console
	ConsoleState Console = FramebufferUefi.Console;

	//Desktop Background
	ClearSurface(&FramebufferUefi.FrontBuffer, 0xFF83ACC8);

	//Taskbar
	DrawSelection TaskbarSelection
	{
		.Y = FramebufferUefi.FrontBuffer.Height - 46,
		.X = 0,
		.H = 46,
		.W = FramebufferUefi.FrontBuffer.Width
	};
	DrawSelection StartSelection
	{
		.Y = FramebufferUefi.FrontBuffer.Height - 46,
		.X = 0,
		.H = 46,
		.W = 46
	};
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 51, 0, FramebufferUefi.FrontBuffer.Height - 1 - 51, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFEFEFE);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 50, 0, FramebufferUefi.FrontBuffer.Height - 1 - 50, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFDFDFD);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 49, 0, FramebufferUefi.FrontBuffer.Height - 1 - 49, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFBFBFB);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 48, 0, FramebufferUefi.FrontBuffer.Height - 1 - 48, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFAFAFA);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 47, 0, FramebufferUefi.FrontBuffer.Height - 1 - 47, FramebufferUefi.FrontBuffer.Width - 1, 0xFFF6F6F6);
	DrawRectangle(&FramebufferUefi.FrontBuffer, &TaskbarSelection, 0, 0xFF000000, 0xFFDFE0E1);
	DrawRectangle(&FramebufferUefi.FrontBuffer, &StartSelection, 0, 0xFFEFF0F1, 0xFFEFF0F1);

	//Window
	{
		//Window Surface + Selection
		Int32 WindowHeight = 250;
		Int32 WindowWidth = 400;
		UInt32 *WindowBuffer = (UInt32 *) ReserveMemory(WindowHeight * WindowWidth);
		DrawSurface WindowSurface
		{
			.Buffer = WindowBuffer,
			.BitsPerPixel = 32,
			.Height = WindowHeight,
			.Width = WindowWidth
		};
		DrawSelection WindowSelection
		{
			.Y = 0,
			.X = 0,
			.H = WindowHeight,
			.W = WindowWidth
		};

		//Window Frame
		DrawRectangle(&WindowSurface, &WindowSelection, 1, 0xFF7B919F, 0xFFEFF4F9);

		//Window Titlebar
		DrawSelection TitlebarSelection
		{
			.Y = 1,
			.X = 1,
			.H = 16,
			.W = WindowWidth - 2
		};
		DrawRectangle(&WindowSurface, &TitlebarSelection, 1, 0xFFFFFFFF, 0xFF000000);
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00FFFFFF, " Editor\r\n");

		//Window Text
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 1 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 2 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 3 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 4 Lorem ipsum dolor sit amet.\r\n");

		//Copy to Screen
		DrawSelection ScreenSelection
		{
			.Y = 300,
			.X = 300,
			.H = WindowHeight,
			.W = WindowWidth
		};
		CopyBitmap(&WindowSurface, &FramebufferUefi.FrontBuffer, &WindowSelection, &ScreenSelection);

		//Free WindowBuffer
		FreeMemory(WindowBuffer);

		//DrawRectangle(&FramebufferUefi.FrontBuffer, &WindowSelection, 1, 0xFF7B919F, 0xFFEFF4F9);
	}

	//Mouse
	{
		DrawSelection SelectionFrom
		{
			.Y = 0,
			.X = 0,
			.H = 20,
			.W = 12
		};
		DrawSelection SelectionTo
		{
			.Y = 300 + 255,
			.X = 300,
			.H = 20,
			.W = 12
		};
		DrawSurface Mouse =
		{
			.Buffer = MousePointerBitmapTest,
			.BitsPerPixel = 32,
			.Height = 20,
			.Width = 12
		};
		CopyBitmapAlpha(&Mouse, &FramebufferUefi.FrontBuffer, &SelectionFrom, &SelectionTo);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Logo

/*
#include<logo.h>

void Logo()
{
	DrawSurface LogoSurface
	{
		.Buffer = (UInt32 *) LogoImage.Data,
		.BitsPerPixel = 32,
		.Height = LogoImage.Height,
		.Width = LogoImage.Width
	};

	DrawSelection LogoSelection =
	{
		.Y = 0,
		.X = 0,
		.H = LogoImage.Height,
		.W = LogoImage.Width
	};

	DrawSelection SelectionTo =
	{
		.Y = 768 / 2 - LogoImage.Height / 2,
		.X = 1024 / 2 - LogoImage.Width / 2,
		.H = LogoImage.Height,
		.W = LogoImage.Width
	};

	CopyBitmapAlphaFromRGBA(&LogoSurface, &FramebufferUefi.FrontBuffer, &LogoSelection, &SelectionTo);
}
*/

//-------------------------------------------------------------------------------------------------------------------------//
//Test BSP

void TestBsp()
{
	//Test Allocate
	//TestAllocate();

	//Test UI
	//TestUi();

	//Test Desktop
	//TestDesktop();

	//Logo
	//Logo();

	//Crash DevideByZero
	//int i = 100 / 0;

	//Crash Pagefault
	//int *Test = (int *) 0xFFFFFFFFFFFFFFFF;
	//*Test = 1;

	//Test Collections
	//TestCollections();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test AP

#define AP_DRAW 1

void TestAp(UInt64 Core)
{
	//Draw Loop
	#ifdef AP_DRAW
	bool Growth = 0;
	UInt8 Counter = 0xFF;
	UInt64 Offset = Core * 25;
	while(true)
	{
		//Draw Direct
		UInt32 Color = Counter << (((Core - 1) % 3) * 8);
		for(int y = 0; y < 25; y++)
		{
			for(int x = 0; x < 25; x++)
			{
				FramebufferUefi.FrontBuffer.Buffer[((y) * FramebufferUefi.FrontBuffer.Width + (x + Offset))] = Color;
			}
		}

		//Sleep
		for(int i = 0; i < 2000000; i++) asm("nop");

		//Alternate
		Counter += Growth ? 1 : -1;
		if(Counter == 128 || Counter == 255) Growth = !Growth;
	}
	#endif
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test Thread

#include<kernel/ps/syscalls.h>
#include<kernel/api/sysapi.h>

void TestSyscalls()
{
	//Software Interrupts
	UInt64 Result0 = ApiNull();
	UInt64 Result1 = ApiAvg(0, 1, 2, 3);
	UInt64 Result2 = ApiMax(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	LogFormatted("Called: %d %d %d\r\n", Result0, Result1, Result2);
}

void TestThread()
{
	//Identify
	UInt64 Thread = ApiGetProcessId();

	//Draw Loop
	bool Growth = 0;
	UInt8 Counter = 0xFF;
	while(true)
	{
		//Draw Direct
		//UInt64 Offset = 200 + Thread * 25;
		//UInt32 Color = Counter << (((Thread - 1) % 3) * 8);
		//for(int y = 0; y < 25; y++)
		//{
		//	for(int x = 0; x < 25; x++)
		//	{
		//		FramebufferUefi.FrontBuffer.Buffer[((y) * FramebufferUefi.FrontBuffer.Width + (x + Offset))] = Color;
		//	}
		//}

		//Draw Syscall
		UInt64 Result = ApiProcessDebugBlink(Thread, Counter);

		//Test Syscalls
		//if(Counter == 128 || Counter == 255) TestSyscalls();

		//Sleep
		for(int i = 0; i < 500000; i++) asm("nop");

		//Alternate
		Counter += Growth ? 1 : -1;
		if(Counter == 128 || Counter == 255) Growth = !Growth;
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test Paging

#include<kernel/mm/paging.h>

void TestPaging()
{
	//Size
	int Size = sizeof(VirtualAddress);
	LogFormatted("%d\r\n", Size);

	//Partition Address To
	VirtualAddress VaTo;
	VaTo.Value = 0;
	//VaTo.Offset = 4095;
	VaTo.IndexL1 = 511;

	//Debug
	LogFormatted("%d = %d(%d:%d:%d:%d:%d)-%d\r\n"
		, VaTo.Value
		, VaTo.Page
		, VaTo.IndexL5
		, VaTo.IndexL4
		, VaTo.IndexL3
		, VaTo.IndexL2
		, VaTo.IndexL1
		, VaTo.Offset
	);

	//VA From
	UInt64 VirtualFrom = 2093056; //GB(4);

	//Partition Address From
	VirtualAddress VaFrom;
	VaFrom.Value = VirtualFrom;
	//VaFrom.Reserved = 0;
	//VaFrom.Offset = 0;

	//Debug
	LogFormatted("%d = %d(%d:%d:%d:%d:%d)-%d\r\n"
		, VaFrom.Value
		, VaFrom.Page
		, VaFrom.IndexL5
		, VaFrom.IndexL4
		, VaFrom.IndexL3
		, VaFrom.IndexL2
		, VaFrom.IndexL1
		, VaFrom.Offset
	);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test IPI

#include<kernel/int/types/ipi.h>

void TestIpi()
{
	//Test IPI
	for(int i = 0; i < 20000000; i++) asm("nop");
	SendTestIpi();
}

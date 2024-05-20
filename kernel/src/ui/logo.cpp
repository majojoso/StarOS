//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "logo.h"

#include<kernel/ps/tasks.h>
#include<kernel/ps/scheduler.h>

#include<kernel/ps/syscalls.h>
#include<kernel/api/apis.h>

#include<ui/draw.h>
#include<ui/framebuffer.h>
#include<ui/image.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

char PathLogo[] = "logo.tga";
char PathProgress[] = "progress.tga";

const UInt32 Background = 0xFFE3E3E3;
const UInt32 Foreground = 0xFF0088FF;

const Int32 BannerOffset = 150;

const Int32 BannerOriginY = 5;
const Int32 BannerOriginX = 81;

const Int32 DotSpacing = 17;
const Int32 DotHeight = 10;
const Int32 DotWidth = 10;
const bool DotFilled = true;

const Int32 LogoSteps = 10;

const Int32 LogoSyncRuns = 2;
const Int32 LogoSyncDelay = 25000000;

DrawSurface *SurfaceLogo = nullptr;
Image *ImageProgress = nullptr;
volatile bool LogoRun = false;
volatile bool LogoRunning = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation General

void DisplayBootBackground(DrawSurface *SurfaceDisplay)
{
	//Clear
	ClearSurface(SurfaceDisplay, Background);
}

void DisplayBootLogo(DrawSurface *SurfaceDisplay)
{
	//Load
	Image *ImageLogo = LoadImage(PathLogo);

	//Success
	if(ImageLogo != nullptr)
	{
		//Display
		DisplayImageAt(ImageLogo, SurfaceDisplay, SurfaceDisplay->Height / 2 - ImageLogo->Height / 2, SurfaceDisplay->Width / 2 - ImageLogo->Width / 2);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation Sync

void DisplayBootProgressSync(DrawSurface *SurfaceDisplay)
{
	//Load
	Image *ImageProgress = LoadImage(PathProgress);

	//Success
	if(ImageProgress != nullptr)
	{
		//Display
		DisplayImageAt(ImageProgress, SurfaceDisplay, SurfaceDisplay->Height - BannerOffset - ImageProgress->Height / 2, SurfaceDisplay->Width / 2 - ImageProgress->Width / 2);
	}

	//Progress Meter
	for(int r = 0; r < LogoSyncRuns; r++)
	{
		//Build Up/Down
		for(int i = 0; i < 2; i++)
		{
			//Build Up/Down
			for(int j = 0; j < LogoSteps; j++)
			{
				//Delay
				for(int i = 0; i < LogoSyncDelay; i++) asm("nop");

				//Selection
				DrawSelection SelectionTo
				{
					.Y = SurfaceDisplay->Height - BannerOffset - BannerOriginY,
					.X = SurfaceDisplay->Width / 2 - BannerOriginX + j * DotSpacing,
					.H = DotHeight,
					.W = DotWidth
				};

				//Dot
				if(DotFilled)
				{
					//Dot Filled
					if(i == 0) DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 0, Foreground, Foreground);
					else DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 0, Background, Background);
				}
				else
				{
					//Dot Hollow
					if(i == 0) DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 3, Foreground, Background);
					else DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 3, Background, Background);
				}
			}
		}
	}

	//Success
	if(ImageProgress != nullptr)
	{
		//Clear
		ClearImageAt(ImageProgress, SurfaceDisplay, SurfaceDisplay->Height - BannerOffset - ImageProgress->Height / 2, SurfaceDisplay->Width / 2 - ImageProgress->Width / 2, Background);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation Async

void DisplayBootProgressAsync()
{
	//Surface
	DrawSurface *SurfaceDisplay = SurfaceLogo;

	//Progress Meter
	while(LogoRun)
	{
		//Build Up/Down
		for(int i = 0; i < 2; i++)
		{
			//Build Up/Down
			for(int j = 0; j < LogoSteps; j++)
			{
				//Delay
				ApiSleep(50);

				//Selection
				DrawSelection SelectionTo
				{
					.Y = SurfaceDisplay->Height - BannerOffset - BannerOriginY,
					.X = SurfaceDisplay->Width / 2 - BannerOriginX + j * DotSpacing,
					.H = DotHeight,
					.W = DotWidth
				};

				//Dot
				if(DotFilled)
				{
					//Dot Filled
					if(i == 0) DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 0, Foreground, Foreground);
					else DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 0, Background, Background);
				}
				else
				{
					//Dot Hollow
					if(i == 0) DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 3, Foreground, Background);
					else DrawRectangle(&FramebufferUefi.FrontBuffer, &SelectionTo, 3, Background, Background);
				}
			}
		}
	}

	//Stop
	LogoRunning = false;

	//Exit
	ApiExitProcess();
}

void DisplayBootProgressStart(DrawSurface *SurfaceDisplay)
{
	//Load
	ImageProgress = LoadImage(PathProgress);

	//Success
	if(ImageProgress != nullptr)
	{
		//Display
		DisplayImageAt(ImageProgress, SurfaceDisplay, SurfaceDisplay->Height - BannerOffset - ImageProgress->Height / 2, SurfaceDisplay->Width / 2 - ImageProgress->Width / 2);
	}

	//Initialize
	SurfaceLogo = SurfaceDisplay;
	LogoRun = true;
	LogoRunning = true;

	//Start
	CreateTask("logo", DisplayBootProgressAsync, false, false, 0);
}

void DisplayBootProgressStop(DrawSurface *SurfaceDisplay)
{
	//Stop
	LogoRun = false;

	//Wait
	while(LogoRunning) asm("nop");

	//Success
	if(ImageProgress != nullptr)
	{
		//Clear
		ClearImageAt(ImageProgress, SurfaceDisplay, SurfaceDisplay->Height - BannerOffset - ImageProgress->Height / 2, SurfaceDisplay->Width / 2 - ImageProgress->Width / 2, Background);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeLogo()
{
	//
}

void DeinitializeLogo()
{
	//
}

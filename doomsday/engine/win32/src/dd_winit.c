/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <skyjake@dengine.net>
 *\author Copyright © 2005-2007 Daniel Swanson <danij@dengine.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

/*
 * dd_winit.h: Win32 Initialization
 *
 * Create windows, load DLLs, setup APIs.
 */

// HEADER FILES ------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM
#define STRICT

#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"

#include "de_base.h"
#include "de_console.h"
#include "de_system.h"
#include "de_play.h"
#include "de_refresh.h"
#include "de_network.h"
#include "de_misc.h"
#include "de_ui.h"

#include "dd_winit.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

uint windowIDX = 0; // Main window.

// PRIVATE DATA DEFINITIONS ------------------------------------------------

application_t app;

// CODE --------------------------------------------------------------------

BOOL InitApplication(application_t *app)
{
    WNDCLASSEX  wcex;

    if(GetClassInfoEx(app->hInstance, app->className, &wcex))
        return TRUE; // Already registered a window class.

    // Initialize a window class for our window.
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = (WNDPROC) WndProc;
    wcex.hInstance = app->hInstance;
    wcex.hIcon =
        (HICON) LoadImage(app->hInstance, MAKEINTRESOURCE(IDI_DOOMSDAY),
                          IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    wcex.hIconSm =
        (HICON) LoadImage(app->hInstance, MAKEINTRESOURCE(IDI_DOOMSDAY),
                          IMAGE_ICON, 16, 16, 0);
    wcex.hCursor = LoadCursor(app->hInstance, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wcex.lpszClassName = app->className;

    // Register our window class.
    return RegisterClassEx(&wcex);
}

static BOOL loadGamePlugin(application_t *app)
{
    char       *dllName = NULL; // Pointer to the filename of the game DLL.

    // First we need to locate the dll name among the command line arguments.
    DD_CheckArg("-game", &dllName);

    // Was a game dll specified?
    if(!dllName)
    {
        DD_ErrorBox(true, "InitGameDLL: No game DLL was specified.\n");
        return FALSE;
    }

    // Now, load the DLL and get the API/exports.
    app->hInstGame = LoadLibrary(dllName);
    if(!app->hInstGame)
    {
        DD_ErrorBox(true, "InitGameDLL: Loading of %s failed (error %d).\n",
                    dllName, GetLastError());
        return FALSE;
    }

    // Get the function.
    app->GetGameAPI = (GETGAMEAPI) GetProcAddress(app->hInstGame, "GetGameAPI");
    if(!app->GetGameAPI)
    {
        DD_ErrorBox(true,
                    "InitGameDLL: Failed to get proc address of "
                    "GetGameAPI (error %d).\n", GetLastError());
        return FALSE;
    }

    // Do the API transfer.
    DD_InitAPI();

    // Everything seems to be working...
    return TRUE;
}

/**
 * Loads the given plugin.
 *
 * @return              <code>true</code> if the plugin was loaded succesfully.
 */
static int loadPlugin(application_t *app, const char *filename)
{
    int         i;

    // Find the first empty plugin instance.
    for(i = 0; app->hInstPlug[i]; ++i);

    // Try to load it.
    if(!(app->hInstPlug[i] = LoadLibrary(filename)))
        return FALSE;           // Failed!

    // That was all; the plugin registered itself when it was loaded.
    return TRUE;
}

/**
 * Loads all the plugins from the startup directory.
 */
static int loadAllPlugins(application_t *app)
{
    long        hFile;
    struct _finddata_t fd;
    char        plugfn[256];

    sprintf(plugfn, "%sdp*.dll", ddBinDir.path);
    if((hFile = _findfirst(plugfn, &fd)) == -1L)
        return TRUE;

    do
        loadPlugin(app, fd.name);
    while(!_findnext(hFile, &fd));

    return TRUE;
}

static int initTimingSystem(void)
{
	// Nothing to do.
    return TRUE;
}

static int initPluginSystem(void)
{
	// Nothing to do.
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    char        path[256];
    char        buf[256];
    BOOL        doShutdown = TRUE;
    int         exitCode = 0;
    int         lnCmdShow = nCmdShow;

    memset(&app, 0, sizeof(app));
    app.hInstance = hInstance;
    app.className = MAINWCLASS;
    DD_ComposeMainWindowTitle(buf);

    if(!InitApplication(&app))
    {
        DD_ErrorBox(true, "Couldn't initialize application.");
    }
    else
    {
        // Initialize COM.
        CoInitialize(NULL);

        // Where are we?
        GetModuleFileName(hInstance, path, 255);
        Dir_FileDir(path, &ddBinDir);

        // Prepare the command line arguments.
        DD_InitCommandLine(GetCommandLine());

        if(!DD_EarlyInit())
        {
            DD_ErrorBox(true, "Error during early init.");
        }
        else if(!initTimingSystem())
        {
            DD_ErrorBox(true, "Error initalizing timing system.");
        }
        else if(!initPluginSystem())
        {
            DD_ErrorBox(true, "Error initializing plugin system.");
        }
        // Load the rendering DLL.
        else if(!DD_InitDGL())
        {
            DD_ErrorBox(true, "Error loading rendering library.");
        }
        // Load the game plugin.
        else if(!loadGamePlugin(&app))
        {
            DD_ErrorBox(true, "Error loading game library.");
        }
        // Load all other plugins that are found.
        else if(!loadAllPlugins(&app))
        {
            DD_ErrorBox(true, "Error loading plugins.");
        }
        // Initialize the memory zone.
        else if(!Z_Init())
        {
            DD_ErrorBox(true, "Error initializing memory zone.");
        }
        else if(0 == (windowIDX =
                Sys_CreateWindow(&app, 0, 0, 0, 640, 480, 32, 0, buf, &lnCmdShow)))
        {
            DD_ErrorBox(true, "Error creating main window.");
        }
        else
        {   // All initialization complete.
            doShutdown = FALSE;

            // Append the main window title with the game name and ensure it
            // is the at the foreground, with focus.
            DD_ComposeMainWindowTitle(buf);
            Sys_SetWindowTitle(windowIDX, buf);

           // SetForegroundWindow(win->hWnd);
           // SetFocus(win->hWnd);
        }
    }

    if(!doShutdown)
    {   // Fire up the engine. The game loop will also act as the message pump.
        exitCode = DD_Main();
    }
    DD_Shutdown();

    // No more use of COM beyond this point.
    CoUninitialize();

    // Unregister our window class.
    UnregisterClass(app.className, app.hInstance);

    // Bye!
    return exitCode;
}

/**
 * All messages go to the default window message processor.
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    boolean forwardMsg = true;
    LRESULT result = 0;
    static PAINTSTRUCT ps;

    switch(msg)
    {
    case WM_SIZE:
		if(!appShutdown)
        {
            switch(wParam)
            {
            case SIZE_MAXIMIZED:
                Sys_SetWindow(windowIDX, 0, 0, 0, 0, 0, DDWF_FULLSCREEN,
                             DDSW_NOBPP|DDSW_NOSIZE|DDSW_NOMOVE|DDSW_NOCENTER);
                forwardMsg = false;
                break;

            default:
                break;
            }
        }
        break;

	case WM_NCLBUTTONDOWN:
		switch(wParam)
		{
		case HTMINBUTTON:
			ShowWindow(hWnd, SW_MINIMIZE);
			break;

		case HTCLOSE:
            PostQuitMessage(0);
			return 0;

        default:
            break;
		}
        // Un-acquire device when entering menu or re-sizing the mouse
        // cursor again.
        //g_bActive = FALSE;
        //SetAcquire();
        break;

    case WM_CLOSE:
        PostQuitMessage(0);
        ignoreInput = true;
        forwardMsg = false;
        break;

    case WM_PAINT:
        if(BeginPaint(hWnd, &ps))
            EndPaint(hWnd, &ps);
        forwardMsg = false;
        result = FALSE;
        break;

    case WM_HOTKEY: // A hot-key combination we have registered has been used.
        // Used to override alt+enter and other easily misshit combinations,
        // at the user's request.
        forwardMsg = false;
        break;

    case WM_SYSCOMMAND:
        switch(wParam)
        {
        case SC_SCREENSAVE: // Screensaver about to begin.
        case SC_MONITORPOWER: // Monitor trying to enter power save.
            forwardMsg = false;
            break;

        default:
            break;
        }
        break;

    case WM_ACTIVATE:
        if(!appShutdown)
		{
			if(LOWORD(wParam) == WA_ACTIVE ||
               (!HIWORD(wParam) && LOWORD(wParam) == WA_CLICKACTIVE))
			{
		        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
                DD_ClearEvents(); // For good measure.
                ignoreInput = false;
            }
			else
			{
		        SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
                ignoreInput = true;
            }
		}
        forwardMsg = false;
        break;

    default:
        break;
    }

    return (forwardMsg? DefWindowProc(hWnd, msg, wParam, lParam) : result);
}

/**
 * Shuts down the engine.
 */
void DD_Shutdown(void)
{
    int         i;

    // Shutdown all subsystems.
    DD_ShutdownAll();

    FreeLibrary(app.hInstGame);
    for(i = 0; app.hInstPlug[i]; ++i)
        FreeLibrary(app.hInstPlug[i]);

    app.hInstGame = NULL;
    memset(app.hInstPlug, 0, sizeof(app.hInstPlug));
}

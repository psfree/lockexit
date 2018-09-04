#include <string.h>
#include <stdio.h>

#include <switch.h>
#include "lockexit.h"

int main(int argc, char **argv)
{
    gfxInitDefault();

    //Initialize console. Using NULL as the second argument tells the console library to use the internal console structure as current one.
    consoleInit(NULL);

    //Move the cursor to row 16 and column 20 and then prints "Hello World!"
    //To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
    //the row and column where you want your cursor to move
	
    printf("\x1b[16;20HHello World!");
	
	Result rc = appletInitialize();
	printf("appletInitialize: %x\n", rc);
	
	_appletDisallowToEnterSleep();
	printf("sleep is disallowed\n");
	
	u64 value = 0;
	rc= appletBeginBlockingHomeButton(value);
	printf("appletBeginBlockingHomeButton: %x\n", rc);

    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		if(kDown & KEY_A) {
			_appletAllowToEnterSleep();
			printf("sleep is allowed\n");
		}

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}


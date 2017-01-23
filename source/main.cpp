#include <stdio.h>
#include <string.h>

#include <3ds.h>
#include "json/json.h"

Result ret=0;
u32 pos;
FILE *f;
size_t tmpsize;

NFC_TagState prevstate, curstate;
NFC_AmiiboConfig amiiboconfig;
PrintConsole topScreen, bottomScreen;
bool dataLoaded = 0;

Result nfc()
{

    ret = nfcGetTagState(&curstate);
    if(R_FAILED(ret))
    {
        printf("nfcGetTagState() failed.\n");
    }
    if(curstate!=prevstate)
    {
        printf("TagState changed from %d to %d.\n", prevstate, curstate);
        prevstate = curstate;
    }

    if(curstate==NFC_TagState_Uninitialized)
    {

        nfcInit(NFC_OpType_NFCTag);

    }

    else if(curstate==NFC_TagState_ScanningStopped)
    {

        	ret = nfcStartScanning(NFC_STARTSCAN_DEFAULTINPUT);
            if(R_FAILED(ret))
            {
                printf("nfcStartScanning() failed.\n");
                return ret;
            }
            else
            {
                printf("Currently Scanning please tap an amiibo\n");
            }

            dataLoaded = 0;

    }

    else if(curstate==NFC_TagState_InRange)
    {

                ret = nfcLoadAmiiboData();
				if(R_FAILED(ret))
				{
					printf("nfcLoadAmiiboData() failed.\n");
				}

				dataLoaded = 0;

    }

    else if(curstate==NFC_TagState_DataReady&&!dataLoaded)
    {

        memset(&amiiboconfig, 0, sizeof(NFC_AmiiboConfig));
        ret = nfcGetAmiiboConfig(&amiiboconfig);
        if(R_FAILED(ret))
        {
            printf("nfcGetAmiiboConfig() failed.\n");
        }

        printf("Data loaded and printed on top screen.\n");

        consoleSelect(&topScreen);
        consoleInit(GFX_TOP, NULL);

        printf("Collection : %02x\n\n", amiiboconfig.characterID[0]);

        printf("Character : %02x\n\n", amiiboconfig.characterID[1]);

        printf("Variant : %02x\n\n", amiiboconfig.characterID[2]);

        printf("Series : %02x\n\n", amiiboconfig.series);

        printf("Amiibo ID : %04x\n\n", amiiboconfig.amiiboID);

        printf("Type : ");
        if(amiiboconfig.type==0)
        {
            printf("Figure\n\n");
        }
        else if(amiiboconfig.type==1)
        {
            printf("Card\n\n");
        }
        else if(amiiboconfig.type==2)
        {
            printf("Plush\n\n");
        }
        else
        {
            printf("Unknown(%02x)",amiiboconfig.type);
        }

        dataLoaded = 1;


    }

    else if(curstate==NFC_TagState_OutOfRange)
    {

        printf("NFC tag is now out of range");
        dataLoaded = 0;
        nfcStopScanning();

    }

    return 0;
}

int main()
{

	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);

	romfsInit();

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

        consoleSelect(&bottomScreen);

        nfc();
	}

	gfxExit();
	return 0;
}

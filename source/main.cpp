#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
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

Json::Reader reader;
Json::Value collectDB;
Json::Value seriesDB;
Json::Value typesDB;

std::string getCollection(NFC_AmiiboConfig aConfig)
{

    u8 subcollec = aConfig.characterID[1]>>4;
    u16 collectID = aConfig.characterID[0];

    collectID = collectID<<4;
    collectID = collectID + subcollec;

    for (unsigned int i=0; i<collectDB.size(); i++) {

            if(collectDB[i]["id"] == collectID)
            {
                return collectDB[i]["name"].asString();
            }

    }

    return "unknown (" + std::to_string(collectID) + ")";
}

std::string getSerie(NFC_AmiiboConfig aConfig)
{

    for (unsigned int i=0; i<seriesDB.size(); i++) {

            if(seriesDB[i]["id"] == aConfig.series)
            {
                return seriesDB[i]["name"].asString();
            }

    }

    return "unknown (" + std::to_string(aConfig.series) + ")";
}

std::string getType(NFC_AmiiboConfig aConfig)
{

    for (unsigned int i=0; i<typesDB.size(); i++) {

            if(typesDB[i]["id"] == aConfig.type)
            {
                return typesDB[i]["name"].asString();
            }

    }

    return "unknown (" + std::to_string(aConfig.type) + ")";
}

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

        printf("Collection : %s\n\n", getCollection(amiiboconfig).c_str());

        printf("Character : %02x\n\n", amiiboconfig.characterID[1]);

        printf("Variant : %02x\n\n", amiiboconfig.characterID[2]);

        printf("Series : %s\n\n", getSerie(amiiboconfig).c_str());

        printf("Amiibo ID : %04x\n\n", amiiboconfig.amiiboID);

        printf("Type : : %s\n\n", getType(amiiboconfig).c_str());

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
    consoleSelect(&bottomScreen);

	Result rc = romfsInit();

	if (rc)
		printf("romfsInit: %08lX\n", rc);
	else
	{
		printf("romfs Init Successful!\n");
	}


	//Load database

	std::ifstream ifsCollection("romfs:/collections.json");
	bool parsingSuccessful = reader.parse(ifsCollection, collectDB);
	if(parsingSuccessful)
    {
        printf("collections.json loaded\n\n");
    }
    else
    {
        printf("error while parsing collections.json\n\n");
    }

    std::ifstream ifsSeries("romfs:/series.json");
	parsingSuccessful = reader.parse(ifsSeries, seriesDB);
	if(parsingSuccessful)
    {
        printf("series.json loaded\n\n");
    }
    else
    {
        printf("series.json\n\n");
    }

    std::ifstream ifsTypes("romfs:/types.json");
	parsingSuccessful = reader.parse(ifsTypes, typesDB);
	if(parsingSuccessful)
    {
        printf("types.json loaded\n\n");
    }
    else
    {
        printf("error while parsing types.json\n\n");
    }

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

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <3ds.h>

#include "json/json.h"
#include "amiiboData.hpp"

Json::Reader reader;
Json::Value collectDB;
Json::Value seriesDB;
Json::Value typesDB;
Json::Value charactersDB;

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

std::string getSeries(NFC_AmiiboConfig aConfig)
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

std::string getCharacter(NFC_AmiiboConfig aConfig)
{

    for (unsigned int i=0; i<charactersDB.size(); i++) {

            if(charactersDB[i]["id"] == aConfig.characterID[0])
            {
                for (unsigned int u=0; u<charactersDB[i]["CHARACTERS"].size(); u++)
                {
                    if(charactersDB[i]["CHARACTERS"][u]["id"] == aConfig.characterID[1])
                    {
                        return charactersDB[i]["CHARACTERS"][u]["name"].asString();
                    }
                }
            }

    }

    return "unknown (" + std::to_string(aConfig.characterID[1]) + ")";
}

std::string getVariant(NFC_AmiiboConfig aConfig)
{

    if(aConfig.characterID[2]==0)
    {
        return "";
    }

    for (unsigned int i=0; i<charactersDB.size(); i++)
        {

            if(charactersDB[i]["id"] == aConfig.characterID[0])
            {
                for (unsigned int u=0; u<charactersDB[i]["CHARACTERS"].size(); u++)
                {
                    if(charactersDB[i]["CHARACTERS"][u]["id"] == aConfig.characterID[1])
                    {
                        for (unsigned int v=0; v<charactersDB[i]["CHARACTERS"][u]["VARIANTS"].size(); v++)
                        {
                            if(charactersDB[i]["CHARACTERS"][u]["VARIANTS"][v]["variant"] == aConfig.characterID[2])
                            {
                                return charactersDB[i]["CHARACTERS"][u]["VARIANTS"][v]["name"].asString();
                            }
                        }
                    }
                }

            }
        }

    return "unknown (" + std::to_string(aConfig.characterID[2]) + ")";
}

std::string getColor(NFC_AmiiboConfig aConfig)
{

    for (unsigned int i=0; i<charactersDB.size(); i++)
        {

            if(charactersDB[i]["id"] == aConfig.characterID[0])
            {
                for (unsigned int u=0; u<charactersDB[i]["CHARACTERS"].size(); u++)
                {
                    if(charactersDB[i]["CHARACTERS"][u]["id"] == aConfig.characterID[1])
                    {
                        for (unsigned int v=0; v<charactersDB[i]["CHARACTERS"][u]["COLORS"].size(); v++)
                        {
                            if(charactersDB[i]["CHARACTERS"][u]["COLORS"][v]["amiiboID"] == aConfig.amiiboID)
                            {
                                return charactersDB[i]["CHARACTERS"][u]["COLORS"][v]["name"].asString();
                            }
                        }
                    }
                }

            }
        }

    return "";
}

void loadData()
{
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

    std::ifstream ifsCharacters("romfs:/characters.json");
	parsingSuccessful = reader.parse(ifsCharacters, charactersDB);
	if(parsingSuccessful)
    {
        printf("characters.json loaded\n\n");
    }
    else
    {
        printf("error while parsing characters.json\n\n");
    }
}

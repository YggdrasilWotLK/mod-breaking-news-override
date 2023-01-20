/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "BreakingNews.h"

bool BreakingNewsServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (packet.GetOpcode() == SMSG_CHAR_ENUM)
    {
        WardenWin* warden = (WardenWin*)session->GetWarden();
        if (!warden)
        {
            return true;
        }

        if (bn_Formatted == "")
        {
            return true;
        }

        warden->SendChunkedPayload(bn_Formatted, 128);
    }

    return true;
}

bool BreakingNewsWorldScript::TryReadFile(std::string& path, std::string& bn_Result)
{
    std::ifstream bn_File(path);

    std::string bn_Buffer = "";

    if (!bn_File.is_open())
    {
        return false;
    }

    while (std::getline(bn_File, bn_Buffer))
    {
        bn_Result = bn_Result + (bn_Buffer);
    }

    return true;
}

void BreakingNewsWorldScript::OnAfterConfigLoad(bool reload)
{
    bn_Enabled = sConfigMgr->GetOption<bool>("BreakingNews.Enable", false);

    if (!bn_Enabled)
    {
        return;
    }

    bn_Title = sConfigMgr->GetOption<std::string>("BreakingNews.Title", "Breaking News");

    std::string htmlPath = sConfigMgr->GetOption<std::string>("BreakingNews.HtmlPath", "./Updates.html");
    if (htmlPath == "")
    {
        LOG_ERROR("module", "Failed to read 'BreakingNews.HtmlPath'.");
        return;
    }

    if (!TryReadFile(htmlPath, bn_Body))
    {
        LOG_ERROR("module", "Failed to read file '{}'.", htmlPath);
        return;
    }

    bn_Formatted = Acore::StringFormatFmt("local saf = ServerAlertFrame;saf:SetParent(CharacterSelect);ServerAlertTitle:SetText('{}');ServerAlertText:SetText('{}');saf:Show(); ", bn_Title, bn_Body);
}

// Add all scripts in one
void AddBreakingNewsScripts()
{
    new BreakingNewsWorldScript();
    new BreakingNewsServerScript();
}

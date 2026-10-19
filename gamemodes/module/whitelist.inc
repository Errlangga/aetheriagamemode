DCMD:w(user, channel, params[])
{
    new id[21];
    if(channel != DCC_FindChannelById("1189539472698781776"))
        return 1;

    if(isnull(params)) 
        return DCC_SendChannelMessage(channel, "**```\nGunakan: !whitelist[Nama_Nama]\n```**");
    
    DCC_GetUserId(user, id, sizeof id);

    new characterQuery[178];
    mysql_format(mMysql, characterQuery, sizeof(characterQuery), "SELECT * FROM `whitelist` WHERE `playername` = '%s'", params);
    mysql_tquery(mMysql, characterQuery, "CheckPlayerNama", "s", params);
    return 1;
}

epublic: CheckPlayerNama(Nama_Player[])
{
    new DCC_Channel: SendChannel;
    SendChannel = DCC_FindChannelById("1189540101844377622");

    new rows = cache_num_rows(), dc[500];
    if(rows > 0)
    {
        format(dc, sizeof(dc),  "❌***Nickname***``%s``***sudah ada di whitelist!***", Nama_Player);
        DCC_SendChannelMessage(SendChannel, dc);
    }
    else 
    {
        format(MySQLStr, 144, "INSERT INTO `whitelist`(`playername`) VALUES ('%s')", Nama_Player);
        mysql_tquery(mMysql, MySQLStr, "", "");

        format(dc, sizeof(dc),  "✅ ***Nickname***``%s``***Berhasil terdaftar di whitelist***!", Nama_Player);
        DCC_SendChannelMessage(SendChannel, dc);
    }
    return 1;
}
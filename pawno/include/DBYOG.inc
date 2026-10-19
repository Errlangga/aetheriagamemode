//=========================
//YOG SYSTEM DATABASE
//=========================


#pragma dynamic 25000


#if defined MAX_STRING
	#define YOG_MAX_VALUE MAX_STRING
#else
	#define YOG_MAX_VALUE 128
#endif
#define YOG_MAX_KEY 24
#define YOG_MAX_LINE (YOG_MAX_KEY + YOG_MAX_VALUE + 3)
#define YOG_MAX_FILENAME 256
#define YOG_MAX_LINES 128


enum Y_CACHE
{
	Y_KEY[YOG_MAX_KEY],
	Y_VALUE[YOG_MAX_VALUE]
}

enum Y_FILE
{
	Y_FILENAME[YOG_MAX_FILENAME],
	bool: Y_OPEN
}

static yCache[YOG_MAX_LINES][Y_CACHE];
static yEmptyCache[Y_CACHE];
static yFile[Y_FILE];
static yNoFile[Y_FILE];


#define YOG_Exist fexist


stock YOG_Open(const filename[])
{
	if(!filename[0]) return false;
	if(yFile[Y_OPEN])
	{
		if(!strcmp(yFile[Y_FILENAME], filename, true)) return true;
		return false;
	}
	new File: h;
	h = fopen(filename, io_readwrite);
	if(h)
	{
		YOG_strcpy(yFile[Y_FILENAME], filename, YOG_MAX_FILENAME, YOG_MAX_FILENAME);
		new line[YOG_MAX_LINE];
		new ln = -1;
		new separator;
		while (((ln + 1) < YOG_MAX_LINES) && (fread(h, line)))
		{
			ln ++;
			YOG_StripLine(line);
			separator = strfind(line, "=", false);
			if((line[0] == ';') || (line[0] == '=') || (separator == -1) || (separator > YOG_MAX_KEY))
			{
				if(line[0] == ';')
				{
					YOG_strcpy(yCache[ln][Y_VALUE], line, YOG_MAX_VALUE, YOG_MAX_VALUE);
				}
				else
				{
					YOG_strcpy(yCache[ln][Y_VALUE][1], line, YOG_MAX_VALUE, YOG_MAX_VALUE);
					yCache[ln][Y_VALUE][0] = ';';
				}
				continue;
			}
			YOG_strcpy(yCache[ln][Y_KEY], line, separator, YOG_MAX_KEY);
			YOG_strcpy(yCache[ln][Y_VALUE], line[separator + 1], YOG_MAX_VALUE, YOG_MAX_VALUE);
			if(!yCache[ln][Y_VALUE][0]) yCache[ln][Y_VALUE][0] = ' ';
		}
		fclose(h);
		yFile[Y_OPEN] = true;
		return 1;
	}
	return 0;
}


stock YOG_Save()
{
	if(!yFile[Y_OPEN]) return false;
	new File: h;
	h = fopen(yFile[Y_FILENAME], io_write);
	if(h)
	{
		new line[YOG_MAX_LINE];
		new ln = -1;
		while (((ln + 1) < YOG_MAX_LINES) && (yCache[ln + 1][Y_VALUE][0]))
		{
			ln ++;
			if(yCache[ln][Y_VALUE][0] == ';')
			{
				if(yCache[ln][Y_VALUE][1])
				{
					format(line, sizeof(line), "%s\r\n", yCache[ln][Y_VALUE]);
					fwrite(h, line);
					continue;
				}
				fwrite(h, "\r\n");
				continue;
			}
			format(line, sizeof(line), "%s=%s\r\n", yCache[ln][Y_KEY], yCache[ln][Y_VALUE]);
			fwrite(h, line);
		}
		fclose(h);
		return true;
	}
	return false;
}


stock YOG_Close()
{
	if(!yFile[Y_OPEN]) return false;
	for (new ln; ln < YOG_MAX_LINES; ln++) yCache[ln] = yEmptyCache;
	yFile = yNoFile;
	return true;
}


stock YOG_Create(filename[]) {
	if(fexist(filename)) return false;
	new File:fhnd;
	fhnd=fopen(filename,io_write);
	if(fhnd) {
		fclose(fhnd);
		return true;
	}
	return false;
}


stock YOG_ReadString(dest[], const key[], maxlength = sizeof(dest))
{
	if((!yFile[Y_OPEN]) || (!key[0])) return false;
	new ln = -1;
	while (((ln + 1) < YOG_MAX_LINES) && (yCache[ln + 1][Y_VALUE][0]))
	{
		ln ++;
		if(yCache[ln][Y_VALUE][0] == ';') continue;
		if(!strcmp(yCache[ln][Y_KEY], key, false))
		{
			YOG_strcpy(dest, yCache[ln][Y_VALUE], YOG_MAX_VALUE, maxlength);
			return true;
		}
	}
	return false;
}


stock YOG_ReadInt(const key[])
{
	new dest[11];
	if(YOG_ReadString(dest, key)) return strval(dest);
	return false;
}


stock Float: YOG_ReadFloat(const key[])
{
	new dest[11];
	if(YOG_ReadString(dest, key)) return floatstr(dest);
	return 0.0;
}


stock YOG_WriteString(const key[], const value[])
{
	if((!yFile[Y_OPEN]) || (!key[0]) || (key[0] == ';') || (strfind(key, "=", false) != -1)) return false;
	new ln = -1;
	while (((ln + 1) < YOG_MAX_LINES) && (yCache[ln + 1][Y_VALUE][0]))
	{
		ln ++;
		if(yCache[ln][Y_VALUE][0] == ';') continue;
		if(!strcmp(yCache[ln][Y_KEY], key, false))
		{
			YOG_strcpy(yCache[ln][Y_VALUE], value, YOG_MAX_VALUE, YOG_MAX_VALUE);
			return true;
		}
	}
	ln ++;
	if(ln < YOG_MAX_LINES)
	{
		YOG_strcpy(yCache[ln][Y_KEY], key, YOG_MAX_KEY, YOG_MAX_KEY);
		YOG_strcpy(yCache[ln][Y_VALUE], value, YOG_MAX_VALUE, YOG_MAX_VALUE);
		return true;
	}
	return false;
}


stock YOG_WriteInt(const key[], value)
{
	new dest[11];
	format(dest, sizeof(dest), "%i", value);
	return YOG_WriteString(key, dest);
}


stock YOG_WriteFloat(const key[], Float: value)
{
	new dest[11];
	format(dest, sizeof(dest), "%0.4f", value);
	return YOG_WriteString(key, dest);
}


stock YOG_RemoveEntry(const key[])
{
	if((!yFile[Y_OPEN]) || (!key[0]) || (strfind(key, "=", false) != -1)) return false;
	new ln = -1;
	while (((ln + 1) < YOG_MAX_LINES) && (yCache[ln + 1][Y_VALUE][0]))
	{
		ln ++;
		if(yCache[ln][Y_VALUE][0] == ';') continue;
		if(!strcmp(yCache[ln][Y_KEY], key, false))
		{
			for (; ln < (YOG_MAX_LINES - 1); ln ++)
			{
				YOG_strcpy(yCache[ln][Y_KEY], yCache[ln + 1][Y_KEY], YOG_MAX_KEY, YOG_MAX_KEY);
				YOG_strcpy(yCache[ln][Y_VALUE], yCache[ln + 1][Y_VALUE], YOG_MAX_VALUE, YOG_MAX_VALUE);
			}
			return true;
		}
	}
	return false;
}


stock YOG_Remove(const filename[])
{
	if(!filename[0]) return false;
	if((yFile[Y_OPEN]) && (!strcmp(yFile[Y_FILENAME], filename, true))) yFile = yNoFile;
	return fremove(filename);
}


stock YOG_strcpy(dest[], const source[], numcells = sizeof(source), maxlength = sizeof(dest))
{
	new i;
	while ((source[i]) && (i < numcells) && (i < maxlength))
	{
		dest[i] = source[i];
		i ++;
	}
	dest[(i == maxlength) ? (i - 1) : (i)] = '\0';
}


stock YOG_StripLine(string[])
{
	new l;
	l = strlen(string);
	if(string[l - 2] == '\r') string[l - 2] = '\0';
	if(string[l - 1] == '\n') string[l - 1] = '\0';
}
#include <Windows.h>
#include <iostream>

#pragma warning(disable:4996)

const char* Key = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const char* absKey = "\\HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";

int AddRun(const char* KeyName, const char* exePath, const char* exeParameter = "");
int DeleteRun(const char* KeyName);

int main(int argc, char* argv[])
{
	//RegAutoRun.exe -add "AutoRun" C:\\path\\program.exe "-p -s"
	if (argc == 4 && !strcmp(argv[1], "-add"))
	{
		//添加无启动参数应用程序
		return AddRun(argv[2], argv[3]);
	}
	else if (argc == 5 && !strcmp(argv[1], "-add"))
	{
		//添加有参数的应用程序
		return AddRun(argv[2], argv[3], argv[4]);
	}
	//RegAutoRun.exe -del "AutoRun"
	else if (argc == 3 && !strcmp(argv[1], "-del"))
	{
		//删除应用程序
		return DeleteRun(argv[2]);
	}
}

int AddRun(const char* KeyName, const char* exePath, const char* exeParameter)
{
	HKEY hKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, Key, 0, KEY_WRITE, &hKey))
	{
		//失败
		return -1;
	}

	char cmd[MAX_PATH * 2] = "";
	strcpy(cmd, exePath);
	strcat(cmd, " ");
	strcat(cmd, exeParameter);
	
	if (ERROR_SUCCESS != RegSetValueEx(hKey, KeyName, 0, REG_SZ, (BYTE*)cmd, (int)strlen(cmd)))
	{
		return -1;
	}
	RegCloseKey(hKey);
	printf("[Success]Add Key \"%s\"(Value=\"%s\") to \"%s\"", KeyName, cmd, absKey);

	return 0;
}

int DeleteRun(const char* KeyName)
{
	HKEY hKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, Key, 0, KEY_WRITE, &hKey))
	{
		//失败
		return -1;
	}
	if (ERROR_SUCCESS != RegDeleteValue(hKey, KeyName))
	{
		//失败
		return -1;
	}
	RegCloseKey(hKey);
	printf("[Success]Delete Key \"%s\" from \"%s\"", KeyName, absKey);

	return 0;
}

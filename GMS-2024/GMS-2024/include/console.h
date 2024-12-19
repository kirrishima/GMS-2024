#ifndef CONSOLE_H

#define byte win_byte_override
#include <Windows.h>
#undef byte
#include <thread>

using namespace std;

struct PersistentConsole
{
	HANDLE hWritePipeIn, hReadPipeOut;
	HANDLE hProcess;
	bool initialized;

	PersistentConsole() : hWritePipeIn(nullptr), hReadPipeOut(nullptr), hProcess(nullptr), initialized(false) {}

	bool initialize() {
		SECURITY_ATTRIBUTES sa{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
		HANDLE hReadPipeIn, hWritePipeOut;

		if (!CreatePipe(&hReadPipeIn, &hWritePipeIn, &sa, 0) ||
			!CreatePipe(&hReadPipeOut, &hWritePipeOut, &sa, 0)) {
			return false;
		}

		STARTUPINFOA si{};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = hReadPipeIn;
		si.hStdOutput = hWritePipeOut;
		si.hStdError = hWritePipeOut;

		PROCESS_INFORMATION pi{};
		if (!CreateProcessA(
			nullptr,
			const_cast<char*>("cmd.exe /Q"),
			nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
			CloseHandle(hReadPipeIn);
			CloseHandle(hWritePipeIn);
			CloseHandle(hWritePipeOut);
			CloseHandle(hReadPipeOut);
			return false;
		}

		CloseHandle(hReadPipeIn);
		CloseHandle(hWritePipeOut);

		hWritePipeIn = hWritePipeIn;
		hReadPipeOut = hReadPipeOut;
		hProcess = pi.hProcess;
		initialized = true;

		CloseHandle(pi.hThread);

		return true;
	}

	bool executeCommand(const  string& command, string& output, DWORD timeoutMs = 5000) {
		if (!initialized) return false;

		string cmd = command + "\r\n";
		DWORD written;
		if (!WriteFile(hWritePipeIn, cmd.c_str(), static_cast<DWORD>(cmd.size()), &written, nullptr)) {
			return false;
		}

		char buffer[4096];
		DWORD bytesRead;
		ostringstream oss;
		auto start = chrono::high_resolution_clock::now();

		while (true) {
			auto now = chrono::high_resolution_clock::now();
			if (chrono::duration_cast<chrono::milliseconds>(now - start).count() > timeoutMs) {
				break;
			}

			DWORD available = 0;
			if (!PeekNamedPipe(hReadPipeOut, nullptr, 0, nullptr, &available, nullptr) || available == 0) {
				this_thread::sleep_for(chrono::milliseconds(50));
				continue;
			}

			if (ReadFile(hReadPipeOut, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
				buffer[bytesRead] = '\0';
				oss << buffer;
				if (oss.str().find(">") != string::npos) {
					break;
				}
			}
		}

		output = oss.str();

		if (output.find("error") != string::npos || output.find("fatal") != string::npos) {
			return false;
		}

		return true;
	}

	~PersistentConsole() {
		if (hWritePipeIn) CloseHandle(hWritePipeIn);
		if (hReadPipeOut) CloseHandle(hReadPipeOut);
		if (hProcess) {
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}
};
#endif // !CONSOLE_H

#include <imtwidgets/CWindowSystem.h>

// Windows includes
#ifdef Q_OS_WINDOWS
	#include <windows.h>
	#include <psapi.h>
#endif


namespace imtwidgets
{


class HandleInfo
{
public:
	HandleInfo()
		:handle(nullptr)

	{
	}

	Qt::HANDLE handle;
	QString title;
};



#ifdef Q_OS_WINDOWS
BOOL CALLBACK FindHandleCallback(HWND windowHandle, LPARAM pParam)
{
	static const int maxBufferSize = 256;

	HandleInfo* handleInfoPtr = (HandleInfo*)pParam;
	char name[maxBufferSize];

	if (handleInfoPtr != nullptr){
		QString title = handleInfoPtr->title;

		GetWindowTextA(windowHandle, name, maxBufferSize - 1);

		QString windowName(name);
		if (title.compare(windowName, Qt::CaseInsensitive) == 0){
			handleInfoPtr->handle = windowHandle;

			return FALSE;
		}
	}

	return TRUE;
}
#endif


qint64 CWindowSystem::FindProcessIdByTitle(const QString& title)
{
	qint64 foundProcessId = 0;

#ifdef Q_OS_WINDOWS
	DWORD processList[1024];
	DWORD bytes = 0;

	if (EnumProcesses(processList, sizeof(processList), &bytes)){
		int count = bytes / sizeof(bytes);

		for (int index = 0; index < count; ++index){
			DWORD processId = processList[index];
			if (processId != 0){
				HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
				if (processHandle){
					HMODULE moduleHandle;
					DWORD currentBytes = 0;

					if (EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &currentBytes)){
						char process[MAX_PATH] = "";
						GetModuleBaseNameA(processHandle, moduleHandle, process, sizeof(process) / sizeof(TCHAR));

						QString processName(process);
						if (processName.compare(title, Qt::CaseInsensitive) == 0){
							CloseHandle(processHandle);

							foundProcessId = processId;

							break;
						}
					}

					CloseHandle(processHandle);
				}
			}
		}
	}

#endif // Q_OS_WINDOWS

	return foundProcessId;
}


Qt::HANDLE CWindowSystem::FindHandleByProcessId(qint64 processId)
{
	Qt::HANDLE handle = 0;

#ifdef Q_OS_WINDOWS
	HWND topWindowHandle = ::GetTopWindow(nullptr);

	while(topWindowHandle != nullptr){
		DWORD currentProcessId = 0;
		::GetWindowThreadProcessId(topWindowHandle, &currentProcessId);

		if (processId == currentProcessId){
			HWND parentWindowHandle = ::GetParent(topWindowHandle);
			HWND lastWindowHandle = topWindowHandle;

			while(topWindowHandle != nullptr){
				lastWindowHandle = parentWindowHandle;

				parentWindowHandle = ::GetParent(lastWindowHandle);
			}

			if (::GetWindowLong(lastWindowHandle, GWL_ID) != 0){
				handle = lastWindowHandle;

				break;
			}
		}

		topWindowHandle = ::GetNextWindow(topWindowHandle, GW_HWNDNEXT);
	}
#endif

	return handle;
}


Qt::HANDLE CWindowSystem::FindHandleByTitle(const QString& title)
{
	Qt::HANDLE handle = nullptr;

#ifdef Q_OS_WINDOWS
	HandleInfo handleInfo;
	handleInfo.title = title;

	if (!EnumWindows((WNDENUMPROC)FindHandleCallback, (LPARAM)&handleInfo)){
		handle = handleInfo.handle;
	}

#endif

	return handle;
}


void CWindowSystem::RaiseWindowByProcessId(qint64 processId)
{
#ifdef Q_OS_WINDOWS
	HWND windowHandle = (HWND)FindHandleByProcessId(processId);
	if (windowHandle){
		::SetForegroundWindow(windowHandle);

		if (::IsIconic(windowHandle)){
			::ShowWindow(windowHandle, SW_RESTORE);
		}
	}
#endif
}


void CWindowSystem::RaiseWindowByTitle(const QString& title)
{
#ifdef Q_OS_WINDOWS
	HWND mainWindowHandle = (HWND)FindHandleByTitle(title);
	HWND lastActiveWindowHandle = mainWindowHandle;

	if (mainWindowHandle != nullptr){
		lastActiveWindowHandle = ::GetLastActivePopup(mainWindowHandle);
	}

	if (lastActiveWindowHandle != nullptr){
		::SetForegroundWindow(lastActiveWindowHandle);

		if (::IsIconic(lastActiveWindowHandle)){
			::ShowWindow(lastActiveWindowHandle, SW_RESTORE);
		}
	}
#endif
}


} // namespace imtwidgets


// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtwidgets/CWindowSystem.h>

// Windows includes
#ifdef Q_OS_WIN
	#include <windows.h>
	#include <psapi.h>
#endif


#include <QtCore/QDebug>


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



#ifdef Q_OS_WIN
BOOL CALLBACK FindHandleCallback(HWND windowHandle, LPARAM pParam)
{
	static const int maxBufferSize = 256;

	HandleInfo* handleInfoPtr = (HandleInfo*)pParam;
	wchar_t name[maxBufferSize];

	if (handleInfoPtr != nullptr){
		QString title = handleInfoPtr->title;

		GetWindowTextW(windowHandle, name, maxBufferSize - 1);

		QString windowName(QString::fromStdWString(name));

		if (!windowName.isEmpty() && windowName.contains(title, Qt::CaseInsensitive)){
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

#ifdef Q_OS_WIN
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

#endif // Q_OS_WIN

	return foundProcessId;
}


Qt::HANDLE CWindowSystem::FindHandleByProcessId(qint64 processId)
{
	Qt::HANDLE handle = 0;

#ifdef Q_OS_WIN
	HWND topWindowHandle = ::GetTopWindow(nullptr);

	while(topWindowHandle != nullptr){
		DWORD currentProcessId = 0;
		::GetWindowThreadProcessId(topWindowHandle, &currentProcessId);

		if (processId == currentProcessId){
			HWND parentWindowHandle = ::GetParent(topWindowHandle);
			HWND lastWindowHandle = topWindowHandle;

			while(parentWindowHandle != nullptr){
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

#ifdef Q_OS_WIN
	HandleInfo handleInfo;
	handleInfo.title = title;

	if (!EnumWindows((WNDENUMPROC)FindHandleCallback, (LPARAM)&handleInfo)){
		handle = handleInfo.handle;
	}

#endif

	return handle;
}


bool CWindowSystem::RaiseWindowByProcessId(qint64 processId)
{
	bool ok = false;
#ifdef Q_OS_WIN
	HWND windowHandle = (HWND)FindHandleByProcessId(processId);
	HWND lastActiveWindowHandle = windowHandle;

	if (windowHandle != nullptr){
		lastActiveWindowHandle = ::GetLastActivePopup(windowHandle);
	}

	if (windowHandle){
		::SetForegroundWindow(windowHandle);
		ok = ::ShowWindow(windowHandle, SW_RESTORE);
		::ShowWindow(windowHandle, SW_SHOW);
		
		if (::IsIconic(lastActiveWindowHandle) && (windowHandle != lastActiveWindowHandle)){
			::ShowWindow(lastActiveWindowHandle, SW_RESTORE);
			::ShowWindow(lastActiveWindowHandle, SW_SHOW);
		}
	}
#endif
	return ok;
}


bool CWindowSystem::RaiseWindowByTitle(const QString& title)
{
#ifdef Q_OS_WIN
	HWND mainWindowHandle = (HWND)FindHandleByTitle(title);
	HWND lastActiveWindowHandle = mainWindowHandle;

	if (mainWindowHandle != nullptr){
		lastActiveWindowHandle = ::GetLastActivePopup(mainWindowHandle);
	}

	if (lastActiveWindowHandle != nullptr){
		bool ok = ::SetForegroundWindow(mainWindowHandle);
		::ShowWindow(mainWindowHandle, SW_RESTORE);
		::ShowWindow(mainWindowHandle, SW_SHOW);

		if ((lastActiveWindowHandle!= mainWindowHandle) && (::IsIconic(lastActiveWindowHandle))){
			ok = ::SetForegroundWindow(lastActiveWindowHandle);
			::ShowWindow(lastActiveWindowHandle, SW_RESTORE);
			::ShowWindow(lastActiveWindowHandle, SW_SHOW);
			return ok;
		}

		return ok;
	}
#endif

	return false;
}


bool CWindowSystem::RaiseWindowByProcessTitle(const QString& title)
{
#ifdef Q_OS_WIN
	quint64 processId = FindProcessIdByTitle(title);
	if (processId != 0){
		return RaiseWindowByProcessId(processId);
	}

	return false;
#endif

	return false;
}


} // namespace imtwidgets


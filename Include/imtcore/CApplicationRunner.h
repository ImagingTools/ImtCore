#pragma once


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLockFile>
#include <QtCore/QProcess>

// STL includes
#include <memory>

// ACF includes
#include <ibase/IApplication.h>
#include <icomp/CCompositeComponent.h>


namespace imtcore
{


class CApplicationRunner
{
public:
	CApplicationRunner() = delete;
	template <class T>
	[[nodiscard]] static int Run(int argc, char** argv, T& applicationComponent, bool autoInit = false);

	/**
		Start a new instance of the running application with the same arguments and quit the current one.
		The new instance waits in Run() until this process has terminated, so it never competes for ports or files.
	*/
	static bool Restart();

private:
	static void WaitForPreviousInstance(int& argc, char** argv);

	static constexpr const char* s_restartLockArgument = "--restart-lock";
	static constexpr int s_previousInstanceTimeout = 120000;

	// released by static destruction, i.e. only after the application component in main() is gone
	static inline std::unique_ptr<QLockFile> s_restartLockPtr;
};


template <class T>
int CApplicationRunner::Run(int argc, char** argv, T& applicationComponent, bool autoInit)
{
	WaitForPreviousInstance(argc, argv);

	if (autoInit) {
		if (!applicationComponent.EnsureAutoInitComponentsCreated()) {
			qCritical() << "[App Runner] Auto-initialize components failed.";
			return -1;
		}
	}

	auto* applicationPtr = applicationComponent.template GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr) {
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}


inline bool CApplicationRunner::Restart()
{
	const QString applicationFilePath = QCoreApplication::applicationFilePath();
	const QString lockFilePath = QDir::temp().filePath(
				QStringLiteral("%1-%2.restart.lock")
							.arg(QFileInfo(applicationFilePath).completeBaseName())
							.arg(QCoreApplication::applicationPid()));

	s_restartLockPtr = std::make_unique<QLockFile>(lockFilePath);
	if (!s_restartLockPtr->tryLock()) {
		qCritical() << "[App Runner] Unable to create restart lock file" << lockFilePath;
		s_restartLockPtr.reset();

		return false;
	}

	QStringList arguments = QCoreApplication::arguments().mid(1);
	arguments << QString::fromLatin1(s_restartLockArgument) << lockFilePath;

	if (!QProcess::startDetached(applicationFilePath, arguments, QDir::currentPath())) {
		qCritical() << "[App Runner] Unable to start a new instance of" << applicationFilePath;
		s_restartLockPtr.reset();

		return false;
	}

	QCoreApplication::quit();

	return true;
}


inline void CApplicationRunner::WaitForPreviousInstance(int& argc, char** argv)
{
	for (int i = 1; i + 1 < argc; ++i) {
		if (qstrcmp(argv[i], s_restartLockArgument) != 0) {
			continue;
		}

		QLockFile previousInstanceLock(QString::fromLocal8Bit(argv[i + 1]));
		previousInstanceLock.setStaleLockTime(s_previousInstanceTimeout);
		if (previousInstanceLock.tryLock(s_previousInstanceTimeout)) {
			previousInstanceLock.unlock();
		}
		else {
			qWarning() << "[App Runner] Previous instance did not terminate in time, starting anyway";
		}

		for (int j = i + 2; j < argc; ++j) {
			argv[j - 2] = argv[j];
		}

		argc -= 2;
		argv[argc] = nullptr;

		return;
	}
}


} // namespace imtcore
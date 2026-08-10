#pragma once


// Qt includes
#include <QtCore/QDirIterator>

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
};




template <class T>
int CApplicationRunner::Run(int argc, char** argv, T& applicationComponent, bool autoInit)
{
	QDirIterator dirIter(":/Style");
	while (dirIter.hasNext())
	{
		qDebug() << dirIter.filePath();

		dirIter.next();
	}

	QDirIterator dirIter3(":/Icons");
	while (dirIter3.hasNext())
	{
		qDebug() << dirIter3.filePath();

		dirIter3.next();
	}

	QDirIterator dirIter2(":/Styles");
	while (dirIter2.hasNext())
	{
		qDebug() << dirIter2.filePath();

		dirIter2.next();
	}

	if (autoInit)
	{
		if (!applicationComponent.EnsureAutoInitComponentsCreated())
		{
			qCritical() << "[App Runner] Auto-initialize components failed.";
			return -1;
		}
	}

	auto* applicationPtr = applicationComponent.template GetInterface<ibase::IApplication>();
	if (applicationPtr != nullptr)
	{
		return applicationPtr->Execute(argc, argv);
	}

	return -1;
}

} // namespace imtcore
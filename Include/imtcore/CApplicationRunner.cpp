#include "ibase/IApplication.h"
#include "icomp/CCompositeComponent.h"
#include <imtcore/CApplicationRunner.h>


// Qt includes
#include <QtCore/QDirIterator>


namespace imtcore
{
int CApplicationRunner::Run(int argc, char** argv, icomp::IComponent& applicationComponent, bool autoInit)
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


	try
	{
		auto& compositeRef = dynamic_cast<icomp::CCompositeComponent&>(applicationComponent);
		if (autoInit)
		{
			if (!compositeRef.EnsureAutoInitComponentsCreated())
			{
				
			}
		}

		ibase::IApplication* applicationPtr = compositeRef.GetComponentInterface<ibase::IApplication>();
		if (applicationPtr != nullptr)
		{
			return applicationPtr->Execute(argc, argv);
		}
	}
	catch (std::bad_cast&)
	{
		qWarning() <<
			"[App Runner] the provided component is NOT an instance of icomp::CCompositeComponent. Auto init skipped.";
	}

	return -1;
}


int CApplicationRunner::Run(int argc, char** argv, icomp::IComponent& applicationComponent, bool autoInit,
							IApplicationInitializer& initializer)
{
	initializer.Initialize();

	return Run(argc, argv, applicationComponent, autoInit);
}
} // namespace imtcore

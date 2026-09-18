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


} // namespace imtcore
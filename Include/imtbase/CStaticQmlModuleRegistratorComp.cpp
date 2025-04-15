#include<imtbase/CStaticQmlModuleRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlModuleRegistratorComp::OnComponentCreated()
{
	static void* iPtr = nullptr;
	Q_ASSERT_X(iPtr == nullptr, __func__, "You are attempting to create a new instance of the component, however, another instance already exists!");
	iPtr = this;

	BaseClass::OnComponentCreated();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
	qmlRegisterModule("QtQuick.Dialogs", 6, 2);
	qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
#endif
}


} // namespace imtbase


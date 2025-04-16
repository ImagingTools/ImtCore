#include<imtbase/CStaticQmlModuleRegistratorComp.h>


// Qt includes
#include<QtQml/QtQml>


namespace imtbase
{


CStaticQmlModuleRegistratorComp::Initializer::Initializer()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qmlRegisterModule("QtGraphicalEffects", 1, 12);
	qmlRegisterModule("QtGraphicalEffects", 1, 0);
	qmlRegisterModule("QtQuick.Dialogs", 1, 3);
#else
	qmlRegisterModule("QtQuick.Dialogs", 6, 2);
	qmlRegisterModule("Qt5Compat.GraphicalEffects", 6, 0);
#endif
}


CStaticQmlModuleRegistratorComp::Initializer* CStaticQmlModuleRegistratorComp::s_initializerPtr = nullptr;


// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlModuleRegistratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (s_initializerPtr == nullptr) {
		s_initializerPtr = new Initializer();
	}

	m_initializerPtr.reset(s_initializerPtr);
}


void CStaticQmlModuleRegistratorComp::OnComponentDestroyed()
{
	m_initializerPtr.reset();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


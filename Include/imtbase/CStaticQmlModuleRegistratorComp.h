#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtbase
{


/**
	\brief This component registers dummy Qt modules in order to ensure QML compatibility with Qt5 and Qt6 applications simultaneously for QQmlEngine.
	\details Registers the following types:
		\a For Qt6 and newer
			\b QtGraphicalEffects.1.12
			\b QtGraphicalEffects.1.0
			\b QtQuick.Dialogs.1.3
		\a For Qt versions below 6.0
			\b QtQuick.Dialogs.6.2
			\b Qt5Compat.GraphicalEffects.6.0

	\warning There MUST be exactly ONE instance of the component per application! Otherwise, the operation of the application will be unpredictable!
	\note This component SHOULD be initialized before application loop will starts. I.e. BEFORE \c qApp->exec();
	\note if you using \c iqtgui:CApplicationCompBase you MUST add this component to \b ComponentsToPreInitialize property
*/
class CStaticQmlModuleRegistratorComp: public icomp::CComponentBase
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStaticQmlModuleRegistratorComp)
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	struct Initializer
	{
		Initializer();
	};

	std::shared_ptr<Initializer> m_initializerPtr;

	static Initializer* s_initializerPtr;
};



} // namespace imtbase






// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtqml
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

	\note It is not appropriate to create multiple instances. Modules will be initialized only after first instance of this component has been created.
	\note This component MUST be initialized \b before application loop will starts. I.e. BEFORE \c qApp->exec();
	\note If you using \c iqtgui:CApplicationCompBase you MUST add this component to \b ComponentsToPreInitialize property
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

private:
	static bool s_isInitialized;
};



} // namespace imtqml






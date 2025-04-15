#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtbase
{


/**
	\brief This component registrates all types of the imtbase module for QQmlEngine.
	\details Registers the following types:
		\c imtbase::CTreeItemModel as \b com.imtcore.imtbase.1.0.TreeItemModel
	\sa imtbase::CTreeItemModel
	\warning There MUST be exactly ONE instance of the component per application! Otherwise, the operation of the application will be unpredictable!
	\note This component SHOULD be initialized before application loop will starts. I.e. BEFORE \c qApp->exec();
	\note if you using \c iqtgui:CApplicationCompBase you MUST add this component to \b ComponentsToPreInitialize property
*/
class CStaticQmlTypeRegistratorComp: public icomp::CComponentBase
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStaticQmlTypeRegistratorComp)
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

};



} // namespace imtbase






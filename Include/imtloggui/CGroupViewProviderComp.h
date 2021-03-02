#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CGroupViewProviderComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupViewProviderComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "GroupViewIds", "Group view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "GroupViewNames", "Group view names", false);
		I_ASSIGN_MULTI_0(m_groupViewCompPtr, "GroupViews", "Groups", false);
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IObjectCollection, m_groupViewCompPtr);
};


} // namespace imtloggui



#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CGroupProviderComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupProviderComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "GroupIds", "Group ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "GroupNames", "Group names", false);
		I_ASSIGN_MULTI_0(m_groupCompPtr, "Groups", "Groups", false);
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_groupCompPtr);
};


} // namespace imtloggui



#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerProvider.h>
#include <imtloggui/IGroupProvider.h>


namespace imtloggui
{


class CGroupProviderComp:
			public icomp::CComponentBase,
			virtual public IGroupProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupProviderComp)
		I_REGISTER_INTERFACE(IGroupProvider);
		I_ASSIGN_MULTI_0(m_groupsCompPtr, "EventGroups", "Event group components", false);
		I_ASSIGN(m_generalGroupCompPtr, "GeneralEventGroup", "General event group component", false, "General");
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupProvider)
	virtual IGroup* GetGroup(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IGroup, m_groupsCompPtr);
	I_REF(IGroup, m_generalGroupCompPtr);

	QMap<QByteArray, IGroup*> m_groups;
};


} // namespace imtloggui



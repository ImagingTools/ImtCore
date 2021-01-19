#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroupView.h>
#include <imtloggui/CProviderBase.h>


namespace imtloggui
{


class CGroupViewProviderComp:
			public icomp::CComponentBase,
			public CProviderBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupViewProviderComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "GroupIds", "Group ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "GroupViewNames", "Group view names", false);
		I_ASSIGN_MULTI_0(m_groupViewCompPtr, "LayerViewProviders", "Layer view providers", false);
	I_END_COMPONENT

	// reimplemented (imtbase::IObjectCollection)
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IGroupView, m_groupViewCompPtr);
};


} // namespace imtloggui



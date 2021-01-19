#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroupView.h>
#include <imtloggui/CProviderBase.h>


namespace imtloggui
{


class CGroupViewComp:
			public icomp::CComponentBase,
			public CProviderBase,
			virtual public IGroupView
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(IGroupView);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN(m_colorAttrPtr, "GroupBgColor", "Group view background color", false, "");
		I_ASSIGN_MULTI_0(m_layerViewCompPtr, "RepresentationViewProviders", "Representation factory view providers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupView)
	virtual QColor GetBackgroundColor() const override;

	// reimplemented (imtbase::IObjectCollection)
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual imtbase::ICollectionInfo::Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_ATTR(QString, m_colorAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_layerViewCompPtr);
};


} // namespace imtloggui



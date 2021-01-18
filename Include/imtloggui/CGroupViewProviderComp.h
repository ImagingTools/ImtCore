#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroupViewProvider.h>


namespace imtloggui
{


class CGroupViewProviderComp:
			public icomp::CComponentBase,
			virtual public IGroupViewProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupViewProviderComp)
		I_REGISTER_INTERFACE(IGroupViewProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "GroupIds", "Group ids", false);
		I_ASSIGN_MULTI_0(m_colourAttrPtr, "GroupBgColors", "Group background colors", false);
		I_ASSIGN_MULTI_0(m_providersCompPtr, "LayerViewProviders", "Layer view providers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupViewProvider)
	virtual ILayerViewProvider* GetLayerViewProvider(const QByteArray& id) const override;

		// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QByteArray, m_colourAttrPtr);
	I_MULTIREF(ILayerViewProvider, m_providersCompPtr);
};


} // namespace imtloggui



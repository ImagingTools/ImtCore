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
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "GroupIds", "Group ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "GroupNames", "Group names", false);
		I_ASSIGN_MULTI_0(m_colourAttrPtr, "GroupBgColor", "Group background color", false);
		I_ASSIGN_MULTI_0(m_layerProvidersCompPtr, "LayerProviders", "Layer providers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroupProvider)
	virtual ILayerProvider* GetLayerProvider(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIATTR(QByteArray, m_colourAttrPtr);
	I_MULTIREF(ILayerProvider, m_layerProvidersCompPtr);
};


} // namespace imtloggui



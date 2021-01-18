#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerViewProvider.h>


namespace imtloggui
{


class CGroupViewComp:
			public icomp::CComponentBase,
			virtual public ILayerViewProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_REGISTER_INTERFACE(ILayerViewProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_providerCompPtr, "RepresentationViewProviders", "Representation factory view providers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayerProvider)
	virtual IRepresentationViewProvider* GetRepresentationViewProvider(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIREF(IRepresentationViewProvider, m_providerCompPtr);
};


} // namespace imtloggui



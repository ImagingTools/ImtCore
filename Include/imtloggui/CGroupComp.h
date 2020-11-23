#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerProvider.h>
#include <imtloggui/IRepresentationFactoryProvider.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			virtual public ILayerProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(ILayerProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_messageIdListAttrPtr, "MessageIdList", "Supported message id's. Empty list for any", false);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_factoryProviderCompPtr, "RepresentationFactoryProviders", "Representation factory providers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayerProvider)
	virtual IRepresentationFactoryProvider* GetRepresentationFactoryProvider(const QByteArray& id) const override;
	virtual QList<int> GetMessageIdList() const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(int, m_messageIdListAttrPtr);
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IRepresentationFactoryProvider, m_factoryProviderCompPtr);
};


} // namespace imtloggui



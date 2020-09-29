#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationFactoryProvider.h>
#include <imtloggui/IRepresentationFactory.h>


namespace imtloggui
{


class CRepresentationFactoryProviderComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactoryProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationFactoryProviderComp)
		I_REGISTER_INTERFACE(IRepresentationFactoryProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationFactoryIds", "Representation factory ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationFactoryNames", "Representation factory names", false);
		I_ASSIGN_MULTI_0(m_factoriesCompPtr, "RepresentationFactories", "Representation factories", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationFactoryProvider)
	virtual IRepresentationFactory* GetRepresentationFactory(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IRepresentationFactory, m_factoriesCompPtr);
};


} // namespace imtloggui



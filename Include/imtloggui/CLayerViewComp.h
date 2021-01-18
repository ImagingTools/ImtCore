#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationViewProvider.h>


namespace imtloggui
{


class CLayerViewComp:
			public icomp::CComponentBase,
			virtual public IRepresentationViewProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerViewComp)
		I_REGISTER_INTERFACE(IRepresentationViewProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationViewIds", "Representation view ids", false);
		I_ASSIGN_MULTI_0(m_representationViewCompPtr, "RepresentationView", "Representation views", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationFactoryProvider)
	virtual IRepresentationView* GetRepresentationView(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIREF(IRepresentationView, m_representationViewCompPtr);
};


} // namespace imtloggui



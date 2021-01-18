#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationProvider.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			virtual public IRepresentationProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(IRepresentationProvider);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationIds", "Representation ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationNames", "Representation names", false);
		I_ASSIGN_MULTI_0(m_representationCompPtr, "Representations", "Representations", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationFactoryProvider)
	virtual IRepresentation* GetRepresentation(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IRepresentation, m_representationCompPtr);
};


} // namespace imtloggui



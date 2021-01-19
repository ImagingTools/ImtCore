#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayer.h>
#include <imtloggui/IRepresentation.h>
#include <imtloggui/CProviderBase.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			public CProviderBase,
			virtual public ILayer			
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(ILayer);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationIds", "Representation ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationNames", "Representation names", false);
		I_ASSIGN(m_minTimespanAttrPtr, "MinimumTimespan", "Minimum visual timespan for layer", true, 1000);
		I_ASSIGN_MULTI_0(m_representationCompPtr, "Representations", "Representations", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayer)
	virtual uint64_t GetMinimumTimespan() const override;

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
	I_ATTR(double, m_minTimespanAttrPtr);
	I_MULTIREF(IRepresentation, m_representationCompPtr);
};


} // namespace imtloggui



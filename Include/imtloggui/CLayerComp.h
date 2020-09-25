#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayer.h>
#include <imtloggui/IRepresentationProvider.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			virtual public ILayer,
			virtual public IRepresentationProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(ILayer);
		I_REGISTER_INTERFACE(IRepresentationProvider);
		I_ASSIGN_MULTI_0(m_representationsCompPtr, "Representations", "Representation components", false);
		I_ASSIGN(m_idAttrPtr, "LayerId", "Layer id", true, "");
		I_ASSIGN(m_nameAttrPtr, "LayerName", "Layer name", true, "Default name");
		I_ASSIGN(m_descriptionAttrPtr, "LayerDescription", "Layer description", true, "");
		I_ASSIGN(m_minTimeSpanAttrPtr, "MinimumTimeSpan", "Minimum timespan", true, 0);
		I_ASSIGN(m_maxTimeSpanAttrPtr, "MaximumTimeSpan", "Maximum timespan", true, 0);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayer)
	virtual QByteArray GetId() const override;
	virtual QString GetName() const override;
	virtual QString GetDescription() const override;
	virtual quint64 GetMinimumVisualTimeSpan() const override;
	virtual quint64 GetMaximumVisualTimeSpan() const override;

	// reimplemented (imtloggui::IRepresentationProvider)
	virtual IRepresentation* GetRepresentation(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IRepresentation, m_representationsCompPtr);
	I_ATTR(QByteArray, m_idAttrPtr);
	I_ATTR(QString, m_nameAttrPtr);
	I_ATTR(QString, m_descriptionAttrPtr);
	I_ATTR(quint64, m_minTimeSpanAttrPtr);
	I_ATTR(quint64, m_maxTimeSpanAttrPtr);

	QMap<QByteArray, IRepresentation*> m_representations;
};


} // namespace imtloggui



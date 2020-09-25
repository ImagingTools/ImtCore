#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentation.h>


namespace imtloggui
{


class CRepresentationComp:
			public icomp::CComponentBase,
			virtual public IRepresentation
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationComp)
		I_REGISTER_INTERFACE(IRepresentation);
		I_ASSIGN(m_idAttrPtr, "RepresentationId", "Representation id", true, "");
		I_ASSIGN(m_nameAttrPtr, "RepresentationName", "Representation name", true, "Default name");
		I_ASSIGN(m_descriptionAttrPtr, "RepresentationDescription", "Representation description", true, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentation)
	virtual QByteArray GetId() const override;
	virtual QString GetName() const override;
	virtual QString GetDescription() const override;

	virtual RepresentationObjectPtr GetRepresentation(
				const ilog::IMessageContainer::Messages& messages,
				const TimeRange& timeRange) const override;

private:
	I_MULTIREF(IRepresentation, m_representationsCompPtr);
	I_ATTR(QByteArray, m_idAttrPtr);
	I_ATTR(QString, m_nameAttrPtr);
	I_ATTR(QString, m_descriptionAttrPtr);
};


} // namespace imtloggui



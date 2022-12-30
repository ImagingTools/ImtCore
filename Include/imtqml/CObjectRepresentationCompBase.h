#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtqml
{


/**
	Basic implementation for object representation.
*/
class CObjectRepresentationCompBase: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationCompBase);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component", false, "");
	I_END_COMPONENT;

protected:
	const imtbase::CTreeItemModel* GetElementModel(const QByteArray& elemementId, const imtbase::CTreeItemModel* elementsModelPtr) const;

	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_qmlPathAttrPtr);
};


} // namespace imtqml



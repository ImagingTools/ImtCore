#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtqml
{


/**
	Returns object view path for a collection of objects.
*/
class CObjectViewDataProviderComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObjectViewDataProviderComp);
		I_ASSIGN(m_objectViewModelIdAttrPtr, "CommandId", "Command ID for the object view", true, "CommandId");
		I_ASSIGN(m_objectViewAttrPtr, "ObjectViewPath", "Object view path", true, "ObjectViewPath");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_objectViewModelIdAttrPtr);
	I_ATTR(QByteArray, m_objectViewAttrPtr);
};


} // namespace imtqml



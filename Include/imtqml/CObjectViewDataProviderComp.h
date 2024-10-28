#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtqml
{


/**
	Returns object view path for a collection of objects.
*/
class CObjectViewDataProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectViewDataProviderComp);
		I_ASSIGN(m_objectViewModelIdAttrPtr, "CommandId", "Command ID for the object view", true, "CommandId");
		I_ASSIGN(m_objectViewPathAttrPtr, "ObjectViewPath", "Object view path", true, "ObjectViewPath");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_objectViewModelIdAttrPtr);
	I_ATTR(QByteArray, m_objectViewPathAttrPtr);
};


} // namespace imtqml



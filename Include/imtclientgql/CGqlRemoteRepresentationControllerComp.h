#pragma once


// ImtCore includes
#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>


namespace imtclientgql
{


class CGqlRemoteRepresentationControllerComp:
			public CGqlRemoteRepresentationControllerCompBase
{
public:
	typedef CGqlRemoteRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtclientgql



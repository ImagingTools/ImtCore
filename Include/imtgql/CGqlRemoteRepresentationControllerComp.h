#pragma once


// ImtCore includes
#include <imtgql/CGqlRemoteRepresentationControllerCompBase.h>


namespace imtgql
{


class CGqlRemoteRepresentationControllerComp:
			public imtgql::CGqlRemoteRepresentationControllerCompBase
{
public:
	typedef imtgql::CGqlRemoteRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtgql



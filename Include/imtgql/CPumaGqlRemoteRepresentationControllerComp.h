#pragma once


// ImtCore includes
#include <imtgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


class CPumaGqlRemoteRepresentationControllerComp:
			public imtgql::CGqlRemoteRepresentationControllerComp
{
public:
	typedef imtgql::CGqlRemoteRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPumaGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtgql



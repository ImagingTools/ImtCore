#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;

protected:
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtauth



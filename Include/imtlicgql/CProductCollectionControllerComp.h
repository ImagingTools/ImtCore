#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtlicgql



#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CAddressCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressCollectionControllerComp);
	I_END_COMPONENT;

protected:

	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgeo




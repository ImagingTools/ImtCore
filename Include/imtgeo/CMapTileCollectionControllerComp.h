#pragma once

#include <QtPositioning/QGeoCoordinate>


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgeo/CTileProvider.h>


namespace imtgeo
{


class CMapTileCollectionControllerComp: public imtservergql::CObjectCollectionControllerCompBase, CTileProvider
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

    I_BEGIN_COMPONENT(CMapTileCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;


};


} // namespace imtgeo




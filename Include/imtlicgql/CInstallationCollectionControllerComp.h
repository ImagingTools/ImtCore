#pragma once

// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CInstallationCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;

};


} // namespace imtlicgql



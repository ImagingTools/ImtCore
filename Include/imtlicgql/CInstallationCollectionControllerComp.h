#pragma once

// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CInstallationCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;
	virtual imtbase::CHierarchicalItemModelPtr GetMetaInfo(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;
};


} // namespace imtlicgql



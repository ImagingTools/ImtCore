#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtguigql
{


class CObjectMetaInfoCollectionControllerComp:
			public CObjectCollectionControllerCompBase
{
public:
	typedef CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectMetaInfoCollectionControllerComp);
	I_END_COMPONENT;


protected:
	virtual imtbase::CHierarchicalItemModelPtr ListObjects(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, const imtgql::IGqlContext* gqlContext, QString& errorMessage) const;


protected:
};


} // namespace imtguigql



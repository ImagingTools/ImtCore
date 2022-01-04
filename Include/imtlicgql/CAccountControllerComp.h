#pragma once


// ImtCore includes
//#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtlicgql/CAccountCollectionControllerComp.h>


namespace imtlicgql
{


class CAccountControllerComp: public imtlicgql::CAccountCollectionControllerComp
{
public:
	typedef imtlicgql::CAccountCollectionControllerComp BaseClass;

	I_BEGIN_COMPONENT(CAccountControllerComp);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const override;
};


} // namespace imtlicgql



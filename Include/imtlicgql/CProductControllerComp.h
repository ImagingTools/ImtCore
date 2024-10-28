#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicgql
{


class CProductControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
	I_END_COMPONENT;

protected:
	virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& objectId, QString& name, QString& description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtlicgql



#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtlicgql
{


class CProductControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
		I_ASSIGN(m_productFactCompPtr, "ProductFactory", "Factory used for creation of the new product instance", true, "ProductFactory");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* ListObjects(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetDependencies(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;

private:
	I_FACT(imtlic::IProductLicensingInfo, m_productFactCompPtr);
};


} // namespace imtlicgql



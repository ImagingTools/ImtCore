#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtbase/IMetaInfoCreator.h>


namespace imtlicgql
{


class CInstallationControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationControllerComp)
		I_ASSIGN(m_productInstanceFactCompPtr, "InstallationFactory", "Factory used for creation of the new product instance", true, "InstallationFactory");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta information creator for the product instance", true, "MetaInfoCreator");
	I_END_COMPONENT

protected:
	virtual imtbase::CHierarchicalItemModelPtr GetObject(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, const imtgql::IGqlContext* gqlContext, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const;

private:
	I_FACT(imtlic::IProductInstanceInfo, m_productInstanceFactCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
};


} // namespace imtlicgql



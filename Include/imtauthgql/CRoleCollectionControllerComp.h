#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
	I_END_COMPONENT;

protected:

	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* ListObjects(const QList<imtgql::CGqlObject>& inputParams, const imtgql::CGqlObject& gqlObject, QString& errorMessage) const;

	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtauth



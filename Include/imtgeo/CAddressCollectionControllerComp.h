#pragma once


// ImtCore includes
#include <imtservergql/CLegacyObjectCollectionControllerCompBase.h>


namespace imtgeo
{


class CAddressCollectionControllerComp: public imtservergql::CLegacyObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CLegacyObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressCollectionControllerComp);
		I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "The string address type collection", true, "AddressTypeCollection");
	I_END_COMPONENT;

protected:
	I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);

	// reimplemented (imtservergql::CLegacyObjectCollectionControllerCompBase)
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgeo




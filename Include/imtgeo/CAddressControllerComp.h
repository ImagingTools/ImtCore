#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtgeo/IAddressElementInfo.h>

#undef GetObject

namespace imtgeo
{



class CAddressControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAddressControllerComp);
        I_ASSIGN(m_addressInfoFactCompPtr, "AddressFactory", "Factory used for creation of the new address element", true, "AddressFactory");
		I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "The string address type collection", true, "AddressTypeCollection");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_FACT(imtgeo::IAddressElementInfo, m_addressInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);
};


} // namespace npgql



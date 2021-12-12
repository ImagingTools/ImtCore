#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IAddress;


/**
	Provider of addresses.
	\ingroup Authentification
*/
class IAddressProvider: virtual public istd::IChangeable
{
public:
	/**
	*	Type of the address.
	*/
	enum AddressCategory
	{
		/**
		*	Address where the product will be shipped.
		*/
		AC_SHIPPING,

		/**
		*	Address where the invoce will be sent.
		*/
		AC_BILLING,

		/**
		*	Primary/first contact in a organization.
		*/
		AC_CONTACT
	};

	I_DECLARE_ENUM(AddressCategory, AC_SHIPPING, AC_BILLING, AC_CONTACT);
	
	virtual const imtbase::ICollectionInfo& GetAddressList() const = 0;
	virtual const IAddress* GetAddress(const QByteArray& addressId) const = 0;
	virtual AddressCategory GetAddressCategory(const QByteArray& addressId) const = 0;
};


} // namespace imtauth



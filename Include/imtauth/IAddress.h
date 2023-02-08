#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>


namespace imtauth
{


/**
	Interface descibing location of an object.
	\ingroup Authentification
*/
class IAddress: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		/**
			Country given as QString.
		*/
		MIT_COUNTRY = 1024,

		/**
			City given as QString.
		*/
		MIT_CITY,

		/**
			Postal code given as int.
		*/
		MIT_POSTAL_CODE,

		/**
			Street given as QString.
		*/
		MIT_STREET
	};

	virtual QString GetCountry() const = 0;
	virtual void SetCountry(const QString& country) = 0;
	virtual QString GetCity() const = 0;
	virtual void SetCity(const QString& city) = 0;
	virtual int GetPostalCode() const = 0;
	virtual void SetPostalCode(int postalCode) = 0;
	virtual QString GetStreet() const = 0;
	virtual void SetStreet(const QString& street) = 0;
};


} // namespace imtauth



#pragma once


// ACF includes
#include <iser/IObject.h>


class QDate;


namespace imtauth
{



/**
	Interface descibing location of an object.
	\ingroup Authentification
*/
class IAddress: virtual public iser::IObject
{
public:
	virtual QString GetCity() const = 0;
	virtual void SetCity(const QString& city) = 0;
	virtual QString GetCountry() const = 0;
	virtual void SetCountry(const QString& country) = 0;
	virtual int GetPostalCode() const = 0;
	virtual void SetPostalCode(int postalCode) = 0;
};


} // namespace imtauth



#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtgeo/IAddressTypeInfo.h>


namespace imtgeo
{


/**
    Interface for describing a address element regions.
	\ingroup Handbooks
*/
class IAddressElementInfo: virtual public iser::IObject
{
public:

    /**
        Get id of the address element.
    */
    virtual QByteArray GetId() const = 0;

    /**
        Set id of the address element.
    */
    virtual void SetId(QByteArray id) = 0;

    /**
        Get id of parent the address element.
    */
    virtual QByteArray GetParentId() const = 0;

    /**
        Set id of parent the address element.
    */
    virtual void SetParentId(QByteArray parentId) = 0;

    /**
        Get id of type the address element.
    */
    virtual const QByteArray GetTypeId() const = 0;

    /**
        Get name of the address element.
	*/
	virtual QString GetName() const = 0;

    /**
        Get description of the address element.
    */
    virtual QString GetDescription() const = 0;

	/**
		Get string address.
	*/
	virtual QString GetFullAddress() const = 0;

	/**
		Get latitude address.
	*/
	virtual double GetLatitude() const = 0;

	/**
		Get longitude address.
	*/
	virtual double GetLongitude() const = 0;
};


} // namespace imtgeo



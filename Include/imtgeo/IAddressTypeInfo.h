#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtgeo
{


/**
    Interface for describing a address element regions.
	\ingroup Handbooks
*/
class IAddressTypeInfo: virtual public iser::IObject
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
        Get name of type the address element.
    */
    virtual QString GetName() const = 0;

    /**
        Get name of the address element.
	*/
	virtual QString GetShortName() const = 0;


    /**
        Get description of the address type.
    */
    virtual QString GetDescription() const = 0;

   

};


} // namespace imtgeo



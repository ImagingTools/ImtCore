#pragma once

// ACF includes
#include <iser/IObject.h>
#include <imtbase/IObjectCollection.h>

// ImtCore includes
#include <imtgeo/IPosition.h>

namespace imtgeo
{
/**
    Interface for describing a address element regions.
	\ingroup Handbooks
*/
class IAddressElementInfo: virtual public iser::IObject, virtual public imtgeo::IPosition
{
public:

    /**
		Get ids of parent the address element.
	*/
    virtual QList<QByteArray> GetParentIds() const = 0;

	/**
		Set ids of parent the address element.
	*/
    virtual void SetParentIds(QList<QByteArray> parentId) = 0;
	/**
		Get id address type of element.
	*/
	virtual QByteArray GetAddressTypeId() const = 0;

	/**
		Set id address type of element.
	*/
	virtual void SetAddressTypeId(QByteArray typeId) = 0;

    /**
        Get name of type the address element.
    */
    virtual QString GetTypeName() const = 0;

    /**
        Get name of the address element.
	*/
	virtual QString GetName() const = 0;

	/**
        Set name of the address element.
	*/
	virtual void SetName(QString name) = 0;

    /**
        Get description of the address element.
    */
    virtual QString GetDescription() const = 0;

    /**
        Set description of the address element.
    */
    virtual void SetDescription(QString description) = 0;

	/**
		Get string address.
	*/
	virtual QString GetAddress() const = 0;

	/**
		Set string address.
	*/
	virtual void SetAddress(QString adr) = 0;

};


} // namespace nphandbooksgql



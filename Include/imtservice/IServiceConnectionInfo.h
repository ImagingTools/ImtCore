#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <iser/ISerializable.h>


namespace imtservice
{


/**
	Interface for describing an service connections info.
	\ingroup Service
*/
class IServiceConnectionInfo: virtual public iser::ISerializable
{
public:

	enum ConnectionType
	{
		CT_INPUT,
		CT_OUTPUT
	};

	I_DECLARE_ENUM(ConnectionType, CT_INPUT, CT_OUTPUT);
	/**
		Get connection input/output type.
	*/
	virtual ConnectionType GetConnectionType() const = 0;
	/**
		Get service type name.
	*/
	virtual QByteArray GetServiceTypeName() const = 0;
	/**
		Get connection usage id.
	*/
	virtual QByteArray GetUsageId() const = 0;
	/**
		Get default url params.
	*/
	virtual QUrl GetDefaultUrl() const = 0;
};


} // namespace imtservice



#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>


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
		Get default service interface.
	*/
	virtual const imtcom::IServerConnectionInterface& GetDefaultInterface() const = 0;
};


} // namespace imtservice



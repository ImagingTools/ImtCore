// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>


namespace imtservice
{


/**
	Interface for describing a service connections info.
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
		Get service type ID.
	*/
	virtual QByteArray GetServiceTypeId() const = 0;

	/**
		Get default service interface.
	*/
	virtual const imtcom::IServerConnectionInterface& GetDefaultInterface() const = 0;
};


} // namespace imtservice



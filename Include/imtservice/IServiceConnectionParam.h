// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtservice/CIncomingConnectionParam.h>


namespace imtservice
{


/**
	Interface for describing a service connection param.
	\ingroup Service
*/
class IServiceConnectionParam: virtual public iser::ISerializable
{
public:
	typedef imtbase::TIdentifiableWrap<CIncomingConnectionParam> IncomingConnectionParam;

	typedef QList<IncomingConnectionParam> IncomingConnectionList;

	virtual QList<IncomingConnectionParam> GetIncomingConnections() const = 0;
};


} // namespace imtservice



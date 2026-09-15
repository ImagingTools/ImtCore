// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


/**
	Interface describing the transport layer for sending already-serialized data to the client.
*/
class ITransport: virtual public istd::IPolymorphic
{
public:
	/*!
		Send a piece of already-serialized data to the client.
	*/
	virtual bool SendData(QByteArray& data) const = 0;
};


} // namespace imtrest



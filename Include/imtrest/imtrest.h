// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMap>


/**
	Namespace containing the implementation of REST-based communication
	This package is system independent.
*/
namespace imtrest
{
typedef QMap<QByteArray,QByteArray> QueryParams;

} // namespace imtrest



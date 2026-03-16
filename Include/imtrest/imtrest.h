// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>


/**
	Namespace containing the implementation of REST-based communication
	This package is system independent.
*/
namespace imtrest
{
typedef QMap<QByteArray,QByteArray> QueryParams;


/**
	Masks password values in a GQL request body to prevent credential exposure in logs.
	Handles GQL inline query format: *password: \"value\" (e.g. password, oldPassword, newPassword).
*/
inline QByteArray MaskPasswordValues(const QByteArray& body)
{
	static const QByteArray passwordSuffix("assword: \\\"");
	static const QByteArray endQuote("\\\"");

	QByteArray result = body;
	int pos = 0;
	int keyPos;
	while ((keyPos = result.indexOf(passwordSuffix, pos)) >= 0){
		int valueStart = keyPos + passwordSuffix.length();
		int valueEnd = result.indexOf(endQuote, valueStart);
		if (valueEnd < 0){
			break;
		}
		result.replace(valueStart, valueEnd - valueStart, "***");
		pos = valueStart + 3 + endQuote.length();
	}
	return result;
}


} // namespace imtrest



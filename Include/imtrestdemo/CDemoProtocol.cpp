// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoProtocol.h>


namespace imtrestdemo
{


namespace DemoProtocol
{


QByteArray CreatePatternData(qint64 offset, qint64 length)
{
	if (length <= 0){
		return QByteArray();
	}

	QByteArray retVal(qsizetype(length), Qt::Uninitialized);
	for (qint64 i = 0; i < length; ++i){
		retVal[qsizetype(i)] = char(GetPatternByte(offset + i));
	}

	return retVal;
}


} // namespace DemoProtocol


} // namespace imtrestdemo

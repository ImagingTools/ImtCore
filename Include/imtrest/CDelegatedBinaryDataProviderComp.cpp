// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CDelegatedBinaryDataProviderComp.h>


namespace imtrest
{


// reimplemented (imtrest::IBinaryDataProvider)

bool CDelegatedBinaryDataProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 readFromPosition,
			qint64 readMaxLength) const
{
	for (int index = 0; index < m_slaveBinaryDataProvidersCompPtr.GetCount(); index++){
		if (m_slaveBinaryDataProvidersCompPtr[index]->GetData(data, dataId, readFromPosition, readMaxLength)){
			return true;
		}
	}

	return false;
}


} // namespace imtrest



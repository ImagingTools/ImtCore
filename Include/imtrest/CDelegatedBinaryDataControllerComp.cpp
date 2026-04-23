// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CDelegatedBinaryDataControllerComp.h>


namespace imtrest
{


// reimplemented (imtrest::IBinaryDataController)

bool CDelegatedBinaryDataControllerComp::SetData(const QByteArray& data, QByteArray& dataId) const
{
	for (int index = 0; index < m_slaveBinaryDataControllersCompPtr.GetCount(); index++){
		if (m_slaveBinaryDataControllersCompPtr[index]->SetData(data, dataId)){
			return true;
		}
	}

	return false;
}


bool CDelegatedBinaryDataControllerComp::RemoveData(const QByteArray& dataId) const
{
	for (int index = 0; index < m_slaveBinaryDataControllersCompPtr.GetCount(); index++){
		if (m_slaveBinaryDataControllersCompPtr[index]->RemoveData(dataId)){
			return true;
		}
	}

	return false;
}


bool CDelegatedBinaryDataControllerComp::EnsureDataExists(const QByteArray& dataId) const
{
	for (int index = 0; index < m_slaveBinaryDataControllersCompPtr.GetCount(); index++){
		if (m_slaveBinaryDataControllersCompPtr[index]->EnsureDataExists(dataId)){
			return true;
		}
	}

	return false;
}


} // namespace imtrest



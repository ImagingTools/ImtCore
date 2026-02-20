// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CServerLogDataProviderComp.h>


namespace imtserverapp
{


// public methods

// reimplemented (imtserverapp::IBinaryDataProvider)

bool CServerLogDataProviderComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
	if (*m_commandIdAttrPtr != dataId){
		return false;
	}

	QString filePath;
	if (m_homeDirPathAttrPtr.IsValid()){
		filePath = m_homeDirPathAttrPtr->GetPath();
	}

	QFile destinationFile(filePath);
	if (!destinationFile.open(QFile::ReadOnly)){
		return false;
	}

	data = destinationFile.readAll();

	destinationFile.close();

	return true;
}


} // namespace imtdb



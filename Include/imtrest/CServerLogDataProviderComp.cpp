#include <imtrest/CServerLogDataProviderComp.h>


namespace imtrest
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

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



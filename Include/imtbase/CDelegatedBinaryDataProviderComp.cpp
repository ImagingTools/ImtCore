#include <imtbase/CDelegatedBinaryDataProviderComp.h>


namespace imtbase
{


// reimplemented (IBinaryDataProvider)

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


} // namespace imtbase



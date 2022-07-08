#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

QByteArrayList CGqlRepresentationDataControllerCompBase::GetModelIds() const
{
	QByteArrayList retVal;

	if (m_modelIdsCompPtr.IsValid()){
		for (int i = 0; i < m_modelIdsCompPtr.GetCount(); i++){
			retVal << m_modelIdsCompPtr[i];
		}
	}

	return retVal;
}


QByteArrayList CGqlRepresentationDataControllerCompBase::GetContextIds() const
{
	QByteArrayList retVal;

	if (m_contextIdsCompPtr.IsValid()){
		for (int i = 0; i < m_contextIdsCompPtr.GetCount(); i++){
			retVal << m_contextIdsCompPtr[i];
		}
	}

	return retVal;
}


} // namespace imtgql



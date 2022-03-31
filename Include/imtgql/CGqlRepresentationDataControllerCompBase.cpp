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


} // namespace imtgql



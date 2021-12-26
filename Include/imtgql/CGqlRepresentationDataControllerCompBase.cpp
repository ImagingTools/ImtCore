#include "CGqlRepresentationDataControllerCompBase.h"


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


imtbase::CTreeItemModel* CGqlRepresentationDataControllerCompBase::CreateResponse(const CGqlRequest &gqlRequest, QString &errorMessage) const
{
	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlRepresentationDataControllerCompBase::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
}


void CGqlRepresentationDataControllerCompBase::OnComponentDestroyed()
{
	ilog::CLoggerComponentBase::OnComponentDestroyed();
}


} // namespace imtgql



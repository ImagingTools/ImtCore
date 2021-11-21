#include "CGqlRepresentationDataControllerCompBase.h"


namespace imtgql
{


// reimplemented (ilog::CLoggerComponentBase)
void CGqlRepresentationDataControllerCompBase::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
}


void CGqlRepresentationDataControllerCompBase::OnComponentDestroyed()
{
	ilog::CLoggerComponentBase::OnComponentDestroyed();
}


// reimplemented (imtgql::IGqlRepresentationDataController)

QByteArray CGqlRepresentationDataControllerCompBase::GetModelId() const
{
	QByteArray retVal;
	if (m_modelIdCompPtr.IsValid()){
		retVal = *m_modelIdCompPtr;
	}
	return retVal;
}


imtbase::CTreeItemModel* CGqlRepresentationDataControllerCompBase::CreateResponse(const CGqlRequest &gqlRequest, QString &errorMessage) const
{
	return nullptr;
}


} // namespace imtgql



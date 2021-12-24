#include "CGqlRepresentationDataControllerCompBase.h"


namespace imtgql
{


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



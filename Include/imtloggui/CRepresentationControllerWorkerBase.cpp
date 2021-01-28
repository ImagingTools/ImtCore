#include <imtloggui/CRepresentationControllerWorkerBase.h>


namespace imtloggui
{


// public methods

CRepresentationControllerWorkerBase::CRepresentationControllerWorkerBase()
{
	connect(this, &CRepresentationControllerWorkerBase::EmitNewJobAdded, this, &CRepresentationControllerWorkerBase::OnNewJobAdded, Qt::QueuedConnection);
	connect(this, &CRepresentationControllerWorkerBase::EmitResultReady, this, &CRepresentationControllerWorkerBase::OnResultReady, Qt::QueuedConnection);
}


} // namespace imtloggui



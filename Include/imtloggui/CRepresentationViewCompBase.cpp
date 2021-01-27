#include <imtloggui/CRepresentationViewCompBase.h>


namespace imtloggui
{


// public methods

CRepresentationViewCompBase::CRepresentationViewCompBase()
{
	connect(this, &CRepresentationViewCompBase::EmitRepresentationUpdated, this, &CRepresentationViewCompBase::OnRepresentationUpdated, Qt::QueuedConnection);
}


// private slots:

void CRepresentationViewCompBase::OnRepresentationUpdated()
{
	istd::CChangeNotifier notifier(this);
}


} // namespace imtloggui


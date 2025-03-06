#include <imtqml/CQmlModelEditorCompBase.h>


namespace imtqml
{


// protected methods

// reimplemented (CQuickObjectCompBase)

void CQmlModelEditorCompBase::OnItemCreated()
{
	bool retVal = connect(
				BaseClass::GetQuickItem(),
				SIGNAL(representationChanged()),
				this,
				SLOT(OnRepresentationChanged()));

	Q_ASSERT(retVal);
}


// protected slots

void CQmlModelEditorCompBase::OnRepresentationChanged()
{
	Q_ASSERT(false);
}


} // namespace imtqml



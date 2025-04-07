#include <imtqml/CQmlModelEditorCompBase.h>


namespace imtqml
{


// protected methods

// reimplemented (CQuickObjectCompBase)

void CQmlModelEditorCompBase::OnGuiCreated()
{
	Q_ASSERT(m_quickWidget != nullptr);
	Q_ASSERT(m_quickWidget->rootObject() != nullptr);

	bool retVal = connect(
				m_quickWidget->rootObject(),
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



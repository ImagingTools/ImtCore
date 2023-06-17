#include <imtbase/CSelectionAdapterBase.h>


namespace imtbase
{


// public methods

CSelectionAdapterBase::CSelectionAdapterBase()
	:m_isUpdateBlocked(false)
{
}


// protected methods

void CSelectionAdapterBase::OnConstraintsChanged()
{
}


void CSelectionAdapterBase::OnSelectionChanged()
{
}


CSelectionAdapterBase::UpdateBlockerPtr CSelectionAdapterBase::CreateUpdateBlocker()
{
	return UpdateBlockerPtr(new UpdateBlocker(m_isUpdateBlocked));
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CSelectionAdapterBase::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	switch (modelId)
	{
	case MI_CONSTRAINTS:
		OnConstraintsChanged();
		break;
	case MI_SELECTION:
		if (!m_isUpdateBlocked){
			OnSelectionChanged();
		}
		break;
	default:
		Q_ASSERT(false);
		break;
	}
}


// reimplemented public methods of the embedded class UpdateBlocker

CSelectionAdapterBase::UpdateBlocker::UpdateBlocker(bool& flag)
	:m_flag(flag)
{
	Q_ASSERT(flag == false);

	m_flag = true;
}

CSelectionAdapterBase::UpdateBlocker::~UpdateBlocker()
{
	m_flag = false;
}


} // namespace imtbase



#include <imtloggui/CGroupComp.h>


// ImtCore includes
#include <imtbase/IObjectProvider.h>


namespace imtloggui
{


// public methods

// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CGroupComp::GetSelectionConstraints() const
{
	return this;
}


int CGroupComp::GetSelectedOptionIndex() const
{
	return GetElementIds().indexOf(m_activeLayerId);
}


bool CGroupComp::SetSelectedOptionIndex(int index)
{
	return false;
}


iprm::ISelectionParam* CGroupComp::GetSubselection(int index) const
{
	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CGroupComp::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CGroupComp::GetOptionsCount() const
{
	return GetElementIds().count();
}


QString CGroupComp::GetOptionName(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return GetElementInfo(ids[index], EIT_NAME).toString();
	}

	return QString();
}


QString CGroupComp::GetOptionDescription(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return GetElementInfo(ids[index], EIT_DESCRIPTION).toString();
	}

	return QString();
}


QByteArray CGroupComp::GetOptionId(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return ids[index];
	}

	return QByteArray();
}


bool CGroupComp::IsOptionEnabled(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return true;
	}

	return false;
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CGroupComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_arrangedIds.isEmpty()){
		if (!m_activeLayerId.isEmpty()){
			istd::CChangeNotifier notifier(this);
			m_activeLayerId.clear();
		}

		return;
	}

	imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();
	if (!timeRange.IsClosed()){
		if (!m_activeLayerId.isEmpty()){
			istd::CChangeNotifier notifier(this);
			m_activeLayerId.clear();
		}

		return;
	}

	uint64_t timespan = timeRange.GetEndTime().toMSecsSinceEpoch() - timeRange.GetBeginTime().toMSecsSinceEpoch();

	QList<uint64_t> keys = m_arrangedIds.keys();
	uint64_t foundKey = m_arrangedIds.firstKey();
	for (int i = 0; i < keys.count(); i++){
		if (keys[i] < timespan){
			foundKey = keys[i];
		}
	}

	QByteArray newLayerId = m_arrangedIds[foundKey];

	if (m_activeLayerId != newLayerId){
		istd::CChangeNotifier notifier(this);
		m_activeLayerId = newLayerId;
	}
}


// reimplemented (icomp::CComponentBase)

void CGroupComp::OnComponentCreated()
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_minTimespanAttrPtr.GetCount());
	count = qMin(count, m_layerCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_arrangedIds.contains(m_minTimespanAttrPtr[i]));

		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_layerCompPtr[i]);
		m_arrangedIds[m_minTimespanAttrPtr[i]] = m_idAttrPtr[i];
	}

	if (!m_arrangedIds.isEmpty()){
		QByteArray id = m_arrangedIds.first();
		m_arrangedIds.remove(m_arrangedIds.firstKey());
		m_arrangedIds[0] = id;

		m_activeLayerId = id;
	}
}


} // namespace imtloggui



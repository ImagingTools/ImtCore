#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// public methods

CLayerComp::CLayerComp()
	:m_isEnabled(false)
{
}


// reimplemented (iprm::IEnableableParam)

bool CLayerComp::IsEnabled() const
{
	return m_isEnabled;
}


bool CLayerComp::IsEnablingAllowed() const
{
	return true;
}


bool CLayerComp::SetEnabled(bool isEnabled)
{
	if (m_isEnabled == isEnabled){
		return true;
	}

	m_isEnabled = isEnabled;

	ICollectionInfo::Ids ids = GetElementIds();
	for (int i = 0; i < ids.count(); i++){
		iprm::IEnableableParam* enableableParamPtr = m_representationControllerCompPtr[i];
		if (enableableParamPtr != nullptr){
			if (ids[i] == m_activeRepresentationId && m_isEnabled){
				enableableParamPtr->SetEnabled(true);
			}
			else{
				enableableParamPtr->SetEnabled(false);
			}
		}
	}

	return true;
}


// reimplemented (imtloggui::ILayerController)

QByteArray CLayerComp::GetActiveRepresentationId() const
{
	return m_activeRepresentationId;
}


bool CLayerComp::SetActiveRepresentationId(const QByteArray& representationId)
{
	if (GetElementIds().contains(representationId)){
		if (m_activeRepresentationId != representationId){
			istd::IChangeable::ChangeSet changeSet(iprm::ISelectionParam::CF_SELECTION_CHANGED);
			istd::CChangeNotifier notifier(this, &changeSet);
			m_activeRepresentationId = representationId;

			ICollectionInfo::Ids ids = GetElementIds();
			for (int i = 0; i < ids.count(); i++){
				iprm::IEnableableParam* enableableParamPtr = m_representationControllerCompPtr[i];
				if (enableableParamPtr != nullptr){
					if (ids[i] == m_activeRepresentationId && m_isEnabled){
						enableableParamPtr->SetEnabled(true);
					}
					else{
						enableableParamPtr->SetEnabled(false);
					}
				}
			}

			return true;
		}
	}

	return false;
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CLayerComp::GetSelectionConstraints() const
{
	return this;
}


int CLayerComp::GetSelectedOptionIndex() const
{
	return GetElementIds().indexOf(m_activeRepresentationId);
}


bool CLayerComp::SetSelectedOptionIndex(int /*index*/)
{
	return false;
}


iprm::ISelectionParam* CLayerComp::GetSubselection(int /*index*/) const
{
	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CLayerComp::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CLayerComp::GetOptionsCount() const
{
	return GetElementIds().count();
}


QString CLayerComp::GetOptionName(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return GetElementInfo(ids[index], EIT_NAME).toString();
	}

	return QString();
}


QString CLayerComp::GetOptionDescription(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return GetElementInfo(ids[index], EIT_DESCRIPTION).toString();
	}

	return QString();
}


QByteArray CLayerComp::GetOptionId(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return ids[index];
	}

	return QByteArray();
}


bool CLayerComp::IsOptionEnabled(int index) const
{
	ICollectionInfo::Ids ids = GetElementIds();
	if (index >= 0 && index < ids.count()){
		return true;
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLayerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationCompPtr.GetCount());
	count = qMin(count, m_representationControllerCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_representationCompPtr[i]);
	}

	if (count > 0){
		SetActiveRepresentationId(m_idAttrPtr[0]);
	}
}


} // namespace imtloggui



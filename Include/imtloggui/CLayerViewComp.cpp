#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// protected methods

// reimplemented (imtloggui::CScenographerBase)

IScenographer* CLayerViewComp::GetActiveElement()
{
	if (IsModelAttached()){
		const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
		QByteArray activeLayerId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

		if (GetElementIds().contains(activeLayerId)){
			return dynamic_cast<IScenographer*>(
				const_cast<istd::IChangeable*>(BaseClass2::BaseClass::GetObjectPtr(activeLayerId)));
		}
	}

	return nullptr;
}


// reimplemented (icomp::CComponentBase)

void CLayerViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_representationViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		istd::IChangeable* objectPtr = dynamic_cast<istd::IChangeable*>(m_representationViewCompPtr[i]);
		if (objectPtr != nullptr){
			RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", objectPtr);
		}
	}
}


} // namespace imtloggui



#include <imtloggui/CGroupViewComp.h>


// Acf includes
#include <imod/IModel.h>
#include <iprm/IEnableableParam.h>


namespace imtloggui
{


// public methods

CGroupViewComp::CGroupViewComp()
	:m_updateBridge(this)
{
}


// reimplemented (iprm::IIdParam)

QByteArray CGroupViewComp::GetId() const
{
	return m_groupId;
}


void CGroupViewComp::SetId(const QByteArray& id)
{
	m_groupId = id;

	ICollectionInfo::Ids ids = GetElementIds();
	for (QByteArray& objectId : ids){
		iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(
					const_cast<istd::IChangeable*>(BaseClass2::GetObjectPtr(objectId)));
		Q_ASSERT(idParamPtr != nullptr);

		idParamPtr->SetId(id);
	}
}


// protected methods

QByteArray CGroupViewComp::GetSelectedLayerId()
{
	if (IsModelAttached()){
		const iprm::IOptionsList* optionListPtr = GetObservedObject()->GetSelectionConstraints();
		QByteArray selectedLayerId = optionListPtr->GetOptionId(GetObservedObject()->GetSelectedOptionIndex());

		if (GetElementIds().contains(selectedLayerId)){
			return selectedLayerId;
		}
	}

	return QByteArray();
}


// reimplemented (imod::CSingleModelObserverBase)

void CGroupViewComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(istd::IChangeable::CF_DELEGATED)){
		m_updateBridge.EnsureModelsDetached();

		QByteArray selectedLayerId = GetSelectedLayerId();

		ICollectionInfo::Ids ids = GetElementIds();
		for (QByteArray& objectId : ids){
			iprm::IEnableableParam* enableableParamPtr = dynamic_cast<iprm::IEnableableParam*>(
						const_cast<istd::IChangeable*>(BaseClass2::GetObjectPtr(objectId)));
			Q_ASSERT(enableableParamPtr != nullptr);

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(enableableParamPtr);
			Q_ASSERT(modelPtr != nullptr);

			if (objectId == selectedLayerId && enableableParamPtr->IsEnablingAllowed()){
				enableableParamPtr->SetEnabled(true);
				modelPtr->AttachObserver(&m_updateBridge);
			}
			else{
				enableableParamPtr->SetEnabled(false);
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CGroupViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_layerViewsCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_idAttrPtr[i].isEmpty());
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));

		istd::IChangeable* objectPtr = dynamic_cast<istd::IChangeable*>(m_layerViewsCompPtr[i]);
		if (objectPtr != nullptr){
			RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", objectPtr);
		}
	}
}


} // namespace imtloggui



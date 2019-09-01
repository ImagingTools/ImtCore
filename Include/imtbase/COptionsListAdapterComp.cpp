#include <imtbase/COptionsListAdapterComp.h>


namespace imtbase
{


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void COptionsListAdapterComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet & /*changeSet*/)
{
	UpdateList();
}


// reimplemented (icomp::CComponentBase)

void COptionsListAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_collectionInfoModelCompPtr.IsValid()){
		RegisterModel(m_collectionInfoModelCompPtr.GetPtr());
	}
}


void COptionsListAdapterComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();

	BaseClass2::UnregisterAllModels();
}


// private methods

void COptionsListAdapterComp::UpdateList()
{
	static ChangeSet changes(iprm::COptionsManager::CF_OPTIONS_CHANGED);

	istd::CChangeNotifier changeNotfier(this, &changes);

	ResetOptions();

	if (m_collectionInfoCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids elementIds = m_collectionInfoCompPtr->GetElementIds();
		for (const QByteArray& id : elementIds){
			InsertOption(
				m_collectionInfoCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString(),
				id,
				m_collectionInfoCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
		}
	}
}


} // namespace imtbase



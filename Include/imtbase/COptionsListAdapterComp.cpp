#include <imtbase/COptionsListAdapterComp.h>


namespace imtbase
{


// public methods

// reimplemented (iprm::IOptionsList)

int COptionsListAdapterComp::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int COptionsListAdapterComp::GetOptionsCount() const
{
	return m_options.GetOptionsCount();
}


QString COptionsListAdapterComp::GetOptionName(int index) const
{
	return m_options.GetOptionName(index);
}


QString COptionsListAdapterComp::GetOptionDescription(int index) const
{
	return m_options.GetOptionDescription(index);
}


QByteArray COptionsListAdapterComp::GetOptionId(int index) const
{
	return m_options.GetOptionId(index);
}


bool COptionsListAdapterComp::IsOptionEnabled(int index) const
{
	return m_options.IsOptionEnabled(index);
}


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
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void COptionsListAdapterComp::UpdateList()
{
	static ChangeSet changes(iprm::COptionsManager::CF_OPTIONS_CHANGED);

	istd::CChangeNotifier changeNotfier(this, &changes);

	m_options.ResetOptions();

	if (m_collectionInfoCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids elementIds = m_collectionInfoCompPtr->GetElementIds();
		for (const QByteArray& id : elementIds){
			if (m_typeIdsAttrPtr.IsValid()){
				QByteArray typeId = m_collectionInfoCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_TYPE_ID).toByteArray();
				
				int index = m_typeIdsAttrPtr.FindValue(typeId);
				if (index < 0){
					continue;
				}
			}

			m_options.InsertOption(
						m_collectionInfoCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString(),
						id,
						m_collectionInfoCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
		}
	}
}


} // namespace imtbase



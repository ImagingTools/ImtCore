#include <imtbase/CParamsManagerAdapterComp.h>


// ACF includes
#include <iprm/IParamsSet.h>


namespace imtbase
{


	// public methods

// reimplemented (iprm::IParamsManager)

int CParamsManagerAdapterComp::GetIndexOperationFlags(int /*index*/) const
{
	return MF_SUPPORT_EDIT;

}


bool CParamsManagerAdapterComp::SetIndexOperationFlags(int /*index*/, int /*flags*/)
{
	return false;
}


int CParamsManagerAdapterComp::GetParamsSetsCount() const
{
	return m_options.GetOptionsCount();
}


const iprm::IOptionsList* CParamsManagerAdapterComp::GetParamsTypeConstraints() const
{
	if (m_collectionCompPtr.IsValid()){
		return m_collectionCompPtr->GetObjectTypesInfo();
	}

	return nullptr;
}


int CParamsManagerAdapterComp::InsertParamsSet(int /*typeIndex*/, int /*index*/)
{
	return -1;
}


bool CParamsManagerAdapterComp::RemoveParamsSet(int /*index*/)
{
	return false;
}


bool CParamsManagerAdapterComp::SwapParamsSet(int /*index1*/, int /*index2*/)
{
	return false;
}


iprm::IParamsSet* CParamsManagerAdapterComp::GetParamsSet(int index) const
{
	if (m_collectionCompPtr.IsValid()){
		QByteArray id = FindOptionFromIndex(index);
		if (!id.isEmpty()){
			if (!m_objectCache.contains(id)){
				imtbase::IObjectCollection::DataPtr objectPtr;

				if (m_collectionCompPtr->GetObjectData(id, objectPtr)){
					iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(objectPtr.GetPtr());
					if (paramsSetPtr != nullptr){
						m_objectCache[id] = objectPtr;

						return paramsSetPtr;
					}
				}
			}
			else{
				iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(m_objectCache[id].GetPtr());
				if (paramsSetPtr != nullptr){
					return paramsSetPtr;
				}
			}
		}
	}

	return nullptr;
}


iprm::IParamsSet* CParamsManagerAdapterComp::CreateParameterSet(int /*typeIndex*/, int /*index*/) const
{
	return nullptr;
}


QString CParamsManagerAdapterComp::GetParamsSetName(int index) const
{
	if (m_collectionCompPtr.IsValid()){
		QByteArray id = FindOptionFromIndex(index);
		if (!id.isEmpty()){
			return m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
		}
	}

	return QString();
}


bool CParamsManagerAdapterComp::SetParamsSetName(int /*index*/, const QString& /*name*/)
{
	return false;
}


QString CParamsManagerAdapterComp::GetParamsSetDescription(int index) const
{
	if (m_collectionCompPtr.IsValid()){
		QByteArray id = FindOptionFromIndex(index);
		if (!id.isEmpty()){
			return m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		}
	}

	return QString();
}


void CParamsManagerAdapterComp::SetParamsSetDescription(int /*index*/, const QString& /*description*/)
{
}


// reimplemented (iprm::ISelectionParam)

const iprm::IOptionsList* CParamsManagerAdapterComp::GetSelectionConstraints() const
{
	return this;
}


int CParamsManagerAdapterComp::GetSelectedOptionIndex() const
{
	return -1;
}


bool CParamsManagerAdapterComp::SetSelectedOptionIndex(int /*index*/)
{
	return false;
}


iprm::ISelectionParam* CParamsManagerAdapterComp::GetSubselection(int /*index*/) const
{
	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CParamsManagerAdapterComp::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CParamsManagerAdapterComp::GetOptionsCount() const
{
	return m_options.GetOptionsCount();
}


QString CParamsManagerAdapterComp::GetOptionName(int index) const
{
	return m_options.GetOptionName(index);
}


QString CParamsManagerAdapterComp::GetOptionDescription(int index) const
{
	return m_options.GetOptionDescription(index);
}


QByteArray CParamsManagerAdapterComp::GetOptionId(int index) const
{
	return m_options.GetOptionId(index);
}


bool CParamsManagerAdapterComp::IsOptionEnabled(int index) const
{
	return m_options.IsOptionEnabled(index);
}


// reimplemented (iser::ISerializable)

bool CParamsManagerAdapterComp::Serialize(iser::IArchive& /*archive*/)
{
	bool retVal = true;

	return retVal;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CParamsManagerAdapterComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet & /*changeSet*/)
{
	UpdateList();
}


// reimplemented (icomp::CComponentBase)

void CParamsManagerAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_collectionModelCompPtr.IsValid()){
		RegisterModel(m_collectionModelCompPtr.GetPtr());
	}
}


void CParamsManagerAdapterComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CParamsManagerAdapterComp::UpdateList()
{
	static ChangeSet changes(iprm::COptionsManager::CF_OPTIONS_CHANGED);

	istd::CChangeNotifier changeNotfier(this, &changes);

	m_options.ResetOptions();

	m_objectCache.clear();

	if (m_collectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids elementIds = m_collectionCompPtr->GetElementIds();
		for (const QByteArray& id : elementIds){
			if (m_typeIdsAttrPtr.IsValid()){
				QByteArray typeId = m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_TYPE_ID).toByteArray();
				
				int index = m_typeIdsAttrPtr.FindValue(typeId);
				if (index < 0){
					continue;
				}
			}

			m_options.InsertOption(
						m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString(),
						id,
						m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
		}
	}
}


QByteArray CParamsManagerAdapterComp::FindOptionFromIndex(int index) const
{
	if ((index >= 0) && index < m_options.GetOptionsCount()){
		return m_options.GetOptionId(index);
	}

	return QByteArray();
}


} // namespace imtbase



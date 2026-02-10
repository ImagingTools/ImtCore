// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskCollectionComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imthype
{


// public methods

// reimplemented (CTaskCollectionCompBase)

QString CTaskCollectionComp::GetTaskTypeName(const QByteArray& taskTypeId) const
{
	int index = m_taskTypeIdsAttrPtr.FindValue(taskTypeId);
	if ((index >= 0) && (index < m_taskTypeNamesAttrPtr.GetCount())){
		return m_taskTypeNamesAttrPtr[index];
	}

	return QString();
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CTaskCollectionComp::GetObjectTypesInfo() const
{
	return &m_taskFactories;
}


// protected methods

// reimplemented (CTaskCollectionCompBase)

iinsp::ISupplierUniquePtr CTaskCollectionComp::CreateTaskInstance(const QByteArray& taskTypeId) const
{
	int index = m_taskTypeIdsAttrPtr.FindValue(taskTypeId);
	if ((index >= 0) && (index < m_taskFactCompPtr.GetCount()) && (index < m_taskTypeIdsAttrPtr.GetCount())){
		return m_taskFactCompPtr.CreateInstance(index);
	}

	return nullptr;
}


// reimplemented (icomp::CComponentBase)

void CTaskCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_taskTypeNamesAttrPtr.GetCount(), m_taskTypeIdsAttrPtr.GetCount());
	for (int i = 0; i < count; ++i){
		m_taskFactories.InsertOption(m_taskTypeNamesAttrPtr[i], m_taskTypeIdsAttrPtr[i]);

		if (m_taskEnablersCompPtr.IsValid()){
			if (i < m_taskEnablersCompPtr.GetCount()){
				iprm::IEnableableParam* enabledPtr = m_taskEnablersCompPtr[i];
				if (enabledPtr != nullptr){
					m_taskFactories.SetOptionEnabled(i, enabledPtr->IsEnabled());
				}
			}
		}
	}
}


} // namespace imthype



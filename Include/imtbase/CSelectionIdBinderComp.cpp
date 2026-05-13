// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSelectionIdBinderComp.h"


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>


namespace imtbase
{


// public methods

CSelectionIdBinderComp::CSelectionIdBinderComp()
	:m_observer(*this)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSelectionIdBinderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int modelIndex = 0;

	for (int i = 0; i < m_selectionCompPtr.GetCount(); i++){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_selectionCompPtr[i]);
		if (modelPtr != nullptr){
			m_observer.RegisterModel(modelPtr, modelIndex++);
		}
	}

	for (int i = 0; i < m_selectionParamCompPtr.GetCount(); i++){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_selectionParamCompPtr[i]);
		if (modelPtr != nullptr){
			m_observer.RegisterModel(modelPtr, modelIndex++);
		}
	}

	UpdateIds();
}


void CSelectionIdBinderComp::OnComponentDestroyed()
{
	m_observer.UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CSelectionIdBinderComp::UpdateIds()
{
	istd::CChangeNotifier notifier(this);

	ResetData();

	for (int i = 0; i < m_selectionCompPtr.GetCount(); i++){
		if (m_selectionCompPtr[i] != nullptr){
			ISelection::Ids ids = m_selectionCompPtr[i]->GetSelectedIds();
			for (const ISelection::Id& id : ids){
				if (!GetElementIds().contains(id)){
					QString name;
					QString description;

					const ICollectionInfo* constraintsPtr = m_selectionCompPtr[i]->GetSelectionConstraints();
					if (constraintsPtr != nullptr){
						name = constraintsPtr->GetElementInfo(id, EIT_NAME).toString();
						description = constraintsPtr->GetElementInfo(id, EIT_DESCRIPTION).toString();
					}

					InsertItem(id, name, description);
				}
			}
		}
	}

	for (int i = 0; i < m_selectionParamCompPtr.GetCount(); i++){
		if (m_selectionParamCompPtr[i] != nullptr){
			int index = m_selectionParamCompPtr[i]->GetSelectedOptionIndex();
			if (index >= 0){
				const iprm::IOptionsList* constraintsPtr = m_selectionParamCompPtr[i]->GetSelectionConstraints();
				if (constraintsPtr != nullptr){
					QByteArray id = constraintsPtr->GetOptionId(index);
					QString name = constraintsPtr->GetOptionName(index);
					QString description = constraintsPtr->GetOptionDescription(index);

					if (!GetElementIds().contains(id)){
						InsertItem(id, name, description);
					}
				}
			}
		}
	}
}


// public methods of the embedded class Observer

CSelectionIdBinderComp::Observer::Observer(CSelectionIdBinderComp& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class Observer

// reimplemented methods (imod::CMultiModelDispatcherBase)

void CSelectionIdBinderComp::Observer::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateIds();
}


} // namespace imtbase



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CSelection.h>


namespace imtbase
{


class CSelectionComp:
			public icomp::CComponentBase,
			public CSelection
{
public:
	typedef icomp::CComponentBase BaseClass;

	enum AutoSelectionMode
	{
		ASM_NO_ACTION,
		ASM_SELECT_ALL,
		ASM_DESELECT_ALL,
		ASM_SET_PRESET,
		ASM_UNSELECT_MISSING
	};

	I_BEGIN_COMPONENT(CSelectionComp);
		I_REGISTER_INTERFACE(ISelection);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_constraintsCompPtr, "Constraints", "Selection constraints", false, "");
		I_ASSIGN(m_initialSelectionModeAttrPtr, "SelectionMode", "Initial selection mode\n0 - SM_SINGLE\n1 - SM_MULTI", true, 0);
		I_ASSIGN(m_autoSelectioModeAttrPtr, "AutoSelectionMode", "Automatic selection mode when changing constraints.\n0 - Do nothing\n1 - Select all\n2 - Clear selection\n3 - Set predefined IDs\n4 - Unselect missing IDs", true, 0);
		I_ASSIGN_MULTI_0(m_initialSelectionIdsAttrPtr, "InitialSelectionIds", "Predefined IDs for initial selection", false);
		I_ASSIGN_MULTI_0(m_autoSelectionIdsAttrPtr, "AutoSelectionIds", "Predefined IDs for auto selection mode 3", false);
	I_END_COMPONENT;

protected:
	// reimplemented (CSelection)
	virtual void OnConstraintsUpdated(const istd::IChangeable::ChangeSet& changeset, const ICollectionInfo* modelPtr) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(ICollectionInfo, m_constraintsCompPtr);
	I_ATTR(int, m_initialSelectionModeAttrPtr);
	I_ATTR(int, m_autoSelectioModeAttrPtr);
	I_MULTIATTR(QByteArray, m_initialSelectionIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_autoSelectionIdsAttrPtr);

	ICollectionInfo::Ids m_lastConstraints;
};


} // namespace imtbase



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
		
	I_BEGIN_COMPONENT(CSelectionComp);
		I_REGISTER_INTERFACE(ISelection);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_constraintsCompPtr, "Constraints", "Selection constraints", false, "");
		I_ASSIGN(m_initialSelectionModeAttrPtr, "SelectionMode", "Initial selection mode\n0 - SM_SINGLE\n1 - SM_MULTI", true, 0);
		I_ASSIGN(m_autoSelectioModeAttrPtr, "AutoSelectionMode", "Automatic selection mode when changing constraints.\n0 - Do nothing\n1 - Select all\n2 - Clear selection\n3 - Set predefined ids\n4 - Unselect missing ids", true, 0);
		I_ASSIGN_MULTI_0(m_initialSelectionIdsAttrPtr, "InitialSelectionIds", "Predefined ids for initial selection", false);
		I_ASSIGN_MULTI_0(m_autoSelectionIdsAttrPtr, "AutoSelectionIds", "Predefined ids for auto selection mode 3", false);
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
};


} // namespace imtbase



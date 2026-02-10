// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/ISelection.h>


namespace imtbase
{


class CSelectionIdBinderComp:
			public icomp::CComponentBase,
			virtual public CCollectionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionIdBinderComp);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN_MULTI_0(m_selectionCompPtr, "Slave selection components", "Slave selection components", false);
		I_ASSIGN_MULTI_0(m_selectionParamCompPtr, "Slave selection param components", "Slave selection param components", false);
	I_END_COMPONENT;

	CSelectionIdBinderComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class Observer : public imod::CMultiModelDispatcherBase
	{
	public:
		Observer(CSelectionIdBinderComp& parent);

	protected:
		// reimplemented methods (imod::CMultiModelDispatcherBase)
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CSelectionIdBinderComp& m_parent;
	};

private:
	void UpdateIds();

private:
	I_MULTIREF(ISelection, m_selectionCompPtr);
	I_MULTIREF(iprm::ISelectionParam, m_selectionParamCompPtr);

	Observer m_observer;
};


} // namespace imtbase



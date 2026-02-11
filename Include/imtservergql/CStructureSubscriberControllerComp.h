// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtbase/IHierarchicalStructure.h>


namespace imtservergql
{


class CStructureSubscriberControllerComp:
			public CGqlPublisherCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureSubscriberControllerComp);
		I_ASSIGN(m_collectionStructureCompPtr, "CollectionStructure", "Collection structure", true, "CollectionStructure");
		I_ASSIGN_TO(m_modelCompPtr, m_collectionStructureCompPtr, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::IHierarchicalStructure, m_collectionStructureCompPtr);
	I_REF(imod::IModel, m_modelCompPtr);

};


} // namespace imtservergql



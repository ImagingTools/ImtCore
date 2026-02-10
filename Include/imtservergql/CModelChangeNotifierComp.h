// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	Simple notifier of the data changes.
	The subscriber will be informed about the fact of data model change. But no other information will be sent to the subscriber.
*/
class CModelChangeNotifierComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::CSingleModelObserverBase
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CModelChangeNotifierComp);
		I_ASSIGN(m_modelCompPtr, "Model", "Observed data model. On changes of this data model the subscriber will be notified", true, "Model");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imod::IModel, m_modelCompPtr);
};


} // namespace imtservergql



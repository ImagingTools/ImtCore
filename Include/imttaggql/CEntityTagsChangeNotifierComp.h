// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imttag/ITagAssignmentManager.h>


namespace imttaggql
{


/**
	Publishes OnEntityTagsChanged to the subscribers of the changed entity type
	that work in the tenant where the change happened.
*/
class CEntityTagsChangeNotifierComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imttag::ITagAssignmentManager>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imttag::ITagAssignmentManager> BaseClass2;

	I_BEGIN_COMPONENT(CEntityTagsChangeNotifierComp);
		I_ASSIGN(m_assignmentManagerCompPtr, "AssignmentManager", "Tag assignment manager", true, "TagAssignmentManager");
		I_ASSIGN_TO(m_assignmentManagerModelCompPtr, m_assignmentManagerCompPtr, true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QByteArray GetCommandId() const;

	I_REF(imttag::ITagAssignmentManager, m_assignmentManagerCompPtr);
	I_REF(imod::IModel, m_assignmentManagerModelCompPtr);
};


} // namespace imttaggql



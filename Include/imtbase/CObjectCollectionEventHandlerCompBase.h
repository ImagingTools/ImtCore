#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionEventHandler.h>

namespace imtbase
{


class CObjectCollectionEventHandlerCompBase:
			public icomp::CComponentBase,
			virtual public IObjectCollectionEventHandler
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectCollectionEventHandlerCompBase);
		I_REGISTER_INTERFACE(IObjectCollectionEventHandler);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "");
		I_ASSIGN(m_slaveEventHandlerCompPtr, "SlaveEventHandler", "Slave object collection event handler", false, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IObjectCollectionEventHandler)
	virtual void OnInsert(const ICollectionInfo::Id& objectId) override;
	virtual void OnUpdate(const ICollectionInfo::Id& objectId) override;
	virtual void OnRemove(const ICollectionInfo::Id& objectId) override;
	virtual void OnRename(const ICollectionInfo::Id& objectId) override;
	virtual void OnDescriptionUpdate(const ICollectionInfo::Id& objectId) override;
	virtual void OnEnableStateChange(const ICollectionInfo::Id& objectId) override;

protected:
	I_REF(IObjectCollection, m_objectCollectionCompPtr);

private:
	I_REF(IObjectCollectionEventHandler, m_slaveEventHandlerCompPtr);
};


} // namespace aculainspgui



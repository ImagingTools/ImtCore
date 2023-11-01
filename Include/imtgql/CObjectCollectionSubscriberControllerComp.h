#pragma once


#include <QObject>
#include <QTimer>

// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlSubscriberControllerCompBase.h>


namespace imtgql
{


class CObjectCollectionSubscriberControllerComp:
			public QObject,
			public imtgql::CGqlSubscriberControllerCompBase,
			public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
	Q_OBJECT
public:
	typedef imtgql::CGqlSubscriberControllerCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionSubscriberControllerComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	virtual bool SetSubscriptions() override;

private Q_SLOTS:
	void OnTimeout();

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);

protected:
	QTimer m_timer;
};


} // namespace imtgql



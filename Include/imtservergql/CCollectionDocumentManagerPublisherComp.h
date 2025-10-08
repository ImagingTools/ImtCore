#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtdoc/ICollectionDocumentManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


class CCollectionDocumentManagerPublisherComp:
	public CGqlPublisherCompBase,
	public imod::TSingleModelObserverBase<imtdoc::ICollectionDocumentManager>
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerPublisherComp)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "DummyCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (iomod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql



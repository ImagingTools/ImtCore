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
	I_END_COMPONENT;

protected:
	// reimplemented (iomod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtservergql



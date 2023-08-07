#pragma once


// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IStructuredCollectionFinder.h>


namespace idoc
{
	class IDocumentMetaInfo;
}


namespace imtbase
{


template <class BaseInterface>
class TICollectionStructureInfo: virtual public istd::IChangeable
{
public:
	/**
		Full node path
	*/
	virtual QByteArrayList GetNodePath(const QByteArray& nodeId) const = 0;

	virtual QSharedPointer<BaseInterface> GetNodeContent(const QByteArray& nodeId) const = 0;

	virtual const IStructuredCollectionFinder* GetCollectionFinder() const = 0;
};


typedef TICollectionStructureInfo<IStructuredObjectCollectionInfo> IObjectCollectionStructureInfo;


} // namespace imtbase



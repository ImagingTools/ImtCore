#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtbase/ICollectionFilter.h>


namespace idoc
{
	class IDocumentMetaInfo;
}


namespace imtbase
{


class ICollectionInfo;
class IStructuredCollectionFinder;


template <class BaseInterface>
class TIStructuredCollectionInfo : virtual public BaseInterface
{
public:
	enum ElementType
	{
		ET_NODE,
		ET_ELEMENT
	};

	virtual ElementType GetElementType(const QByteArray& elementId) const = 0;
	virtual QByteArrayList GetBasePath() const = 0;
};

typedef TIStructuredCollectionInfo<imtbase::ICollectionInfo> IStructuredCollectionInfo;
typedef TIStructuredCollectionInfo<imtbase::IObjectCollectionInfo> IStructuredObjectCollectionInfo;


class IHierarchicalCollectionFilter: virtual public ICollectionFilter
{
	enum FilterTypeFlags
	{
		FTF_NODE = 1,
		FTF_OBJECT = 2
	};

	virtual int GetFilterTypeFlags() = 0;
	virtual void SetFilterTypeFlags(int flags) = 0;
};


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



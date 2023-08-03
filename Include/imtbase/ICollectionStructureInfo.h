#pragma once


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>


namespace idoc
{
	class IDocumentMetaInfo;
}

namespace imtbase
{


class ICollectionInfo;
class IStructuredCollectionFinder;


class ICollectionStructureInfo: virtual public istd::IChangeable
{
public:
	class IStructuredCollectionInfo : virtual public imtbase::ICollectionInfo
	{
		enum ElementInfoType
		{
			EIT_NODE = EIT_USER
		};
	};

	struct NodeInfo
	{
		QString name;
		QString description;
		QByteArrayList path;
	};

	virtual QByteArray GetParentNodeId(const QByteArray& nodeId) const = 0;
	virtual QString GetNodeName(const QByteArray& nodeId) const = 0;
	virtual QString GetNodeDescription(const QByteArray& nodeId) const = 0;
	virtual QSharedPointer<idoc::IDocumentMetaInfo> GetNodeMetaInfo(const QByteArray& nodeId) const = 0;
	virtual QSharedPointer<IStructuredCollectionInfo> GetNodesContent(const QByteArray& nodeId) const = 0;
	virtual QSharedPointer<IStructuredCollectionInfo> GetChildNodes(const QByteArray& nodeId) const = 0;
	virtual QSharedPointer<IStructuredCollectionInfo> GetChildObjects(const QByteArray& nodeId) const = 0;
	virtual const IStructuredCollectionFinder* GetCollectionFinder() const = 0;
};



//class CNodeCollectionInfo: virtual public imtbase::ICollectionInfo
//{
//public:
//	CNodeCollectionInfo(const QByteArray& nodeId, const imtbase::IObjectCollection& nodeCollection)
//		:m_nodeId(nodeId),
//		m_collection(collection)
//	{
//	}
//
//	virtual Ids GetElementIds(
//				int offset = 0,
//				int count = -1,
//				const iprm::IParamsSet* selectionParamsPtr = nullptr) const
//	{
//		iprm::CParamsSet nodeFilter;
//		iprm::CIdParam nodeId;
//		nodeFilter.SetEditableParameter("NodeId", &nodeId);
//		nodeId.SetId(m_nodeId);
//
//		return m_collection.GetElementIds(offset, count, &nodeFilter);
//	}
//
//	const QByteArray& m_nodeId;
//	const imtbase::IObjectCollection& m_collection;
//};

} // namespace imtbase



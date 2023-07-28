#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ICollectionInfo;


class IIdHierarchical: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	/**
		Flags describing supported features of this hierarchical graph.
	*/
	enum HierarchicalFlags
	{
		/**
			If active child information is supported.
		*/
		HF_CHILDS_SUPPORTED = 0x01,
		/**
			If active parent information is supported.
			It will be inactive for single direction hierarchical graphs.
		*/
		HF_PARENT_SUPPORTED = 0x02
	};

	/**
		Get flags describing supported features of this hierarchical graph.
		\sa	HierarchicalFlags.
	*/
	virtual int GetHierarchicalFlags() const = 0;

	/**
		Current node ID
	*/
	virtual Id GetNodeId() const = 0;

	/**
		For single direction graph when only child connection is stored it will be always NULL.
		To check if this feature is supported check HF_PARENT_SUPPORTED in flags.
	*/
	virtual Id GetParentNodeId() const = 0;

	/**
		For single direction graph when only parent connection is stored it will be always NULL.
		To check if this feature is supported check HF_CHILDS_SUPPORTED in flags.
	*/
	virtual Ids GetChildNodeIds() const = 0;
};


} // namespace imtbase



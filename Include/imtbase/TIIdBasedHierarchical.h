#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class ICollectionInfo;


/**
	Common interface to define the hierarchical graph structures.
*/
template <class Interface>
class TIIdBasedHierarchical: virtual public Interface
{
public:
	typedef Interface InterfaceType;
	typedef TIIdBasedHierarchical<Interface> HierarchicalInterfaceType;

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
		Get child nodes list.
	*/
	virtual const ICollectionInfo& GetChildsList() const = 0;

	/**
		Get access to child node.
		For single direction graph when only parent connection is stored it will be always NULL.
		To check if this feature is supported check HF_CHILDS_SUPPORTED in flags.
	*/
	virtual HierarchicalInterfaceType* GetChild(const QByteArray& id) const = 0;

	/**
		Get access to parent node.
		For single direction graph when only child connection is stored it will be always NULL.
		To check if this feature is supported check HF_PARENT_SUPPORTED in flags.
	*/
	virtual HierarchicalInterfaceType* GetParent() const = 0;
};


} // namespace imtbase



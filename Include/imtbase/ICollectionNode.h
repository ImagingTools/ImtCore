#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/TIIdBasedHierarchical.h>


namespace imtbase
{


class ICollectionInfo;


/**
	Interface for non-iterable colllection info.
	\ingroup Collection
*/
class ICollectionNode: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	virtual QString GetName() const = 0;
	virtual bool SetName(const QString& name) = 0;

	/**
		Get contents of the node.
	*/
	virtual const ICollectionInfo& GetObjectsList() const = 0;
};


typedef TIIdBasedHierarchical<ICollectionNode> IHierarchicalCollectionNode;


} // namespace imtbase



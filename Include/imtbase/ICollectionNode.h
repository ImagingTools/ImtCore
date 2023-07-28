#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ICollectionInfo;


class ICollectionNode: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	virtual QByteArray GetNodeId() const = 0;
	virtual QByteArray GetParentNodeId() const = 0;
	virtual const ICollectionInfo& GetChildNodes() const = 0;
	virtual const ICollectionInfo& GetObjects() const = 0;
};


} // namespace imtbase



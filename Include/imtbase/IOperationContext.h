// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class IObjectCollection;


/**
	Common interface for the context of an operation.
	This interface provide informations:
	1. Who/what has started the operation
	2. Human-readable description of the operation
*/
class IOperationContext: virtual public istd::IChangeable
{
public:
	struct IdentifableObjectInfo
	{
		IdentifableObjectInfo(const QByteArray& id = QByteArray(), const QString& name = QString()): id(id), name(name) {}

		QByteArray id;
		QString name;

		bool operator==(const IdentifableObjectInfo& other) const
		{
			return (id == other.id) && (name == other.name);
		}

		bool operator!=(const IdentifableObjectInfo& other) const
		{
			return !operator==(other);
		}
	};
	
	static const QByteArray OPERATION_CONTEXT_INFO;
	typedef IdentifableObjectInfo OperationContextInfo;

	/**
		Get information about the initiator of the operation. E.g user-ID
	*/
	virtual IdentifableObjectInfo GetOperationOwnerId() const = 0;

	/**
		Description related to the operation.
	*/
	virtual QString GetOperationDescription() const = 0;

	/**
		Get changes collection.
	*/
	virtual imtbase::IObjectCollection* GetChangesCollection() = 0;
};


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::IOperationContext::OperationContextInfo);



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <functional>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>


namespace imtqml
{


/**
	\brief Transport-agnostic abstract bridge used by
	\c CDataModelController to fetch / store generic data models.

	\details
	A concrete bridge knows how to map a \c modelId (plus an opaque
	\c parameters map) to an actual fetch / store operation. Two
	implementations are provided out of the box:

	- \c CGqlDataModelBridge: forwards every call to an SDL-generated
	  GraphQL request (the GUI-client scenario).
	- \c CDataModelBridge: forwards every call to an in-process model
	  provider (the server / in-process scenario).

	All operations are asynchronous; implementations MUST invoke the
	callback exactly once on the GUI thread. An empty \c errorMessage
	signals success.
*/
class IDataModelBridge
{
public:
	typedef std::function<void(QVariant /*model*/, QString /*errorMessage*/)> TGetModelCallback;
	typedef std::function<void(QString /*errorMessage*/)> TSetModelCallback;

	virtual ~IDataModelBridge() = default;

	virtual void GetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			TGetModelCallback callback) = 0;

	virtual void SetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			TSetModelCallback callback) = 0;
};


} // namespace imtqml

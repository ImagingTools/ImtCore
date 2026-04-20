// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVariant>


namespace imtqml
{


/**
	GraphQL helpers shared between C++ data controllers.

	These utilities convert between Qt value types and the inline GraphQL
	literal syntax used by the generated SDL types. They mirror the
	behaviour of the QML \c Gql.GqlObject(...).fromObject(...) helpers used
	by the QML data controllers.
*/
namespace gql
{


/**
	Convert a \c QVariant to its GraphQL literal representation.

	Supports primitive types, lists and string-keyed maps (which are
	rendered as GraphQL input objects: \c {key:value, ...}). Strings are
	properly quoted and escaped. Anything not understood is rendered as a
	JSON-encoded string.
*/
QString ToGqlLiteral(const QVariant& value);


/**
	Build a complete GraphQL request body for the given operation.

	The arguments map is rendered as the single \c input parameter; the
	\c selection string (already in GraphQL braces, e.g. \c "{ id name }")
	defines the requested response fields. \c operation must be \c "query"
	or \c "mutation".
*/
QString BuildRequest(
			const QString& operation,
			const QString& commandId,
			const QVariantMap& inputArgs,
			const QString& selection);


} // namespace gql

} // namespace imtqml

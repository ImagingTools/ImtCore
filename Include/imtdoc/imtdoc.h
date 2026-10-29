// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QUrl>

// ACF includes
#include <iprm/IParamsSet.h>


namespace imtdoc
{


/**
	ID of the \c iprm::IIdParam with the collection object ID in the document source parameters.
*/
static const QByteArray s_objectIdParamId = QByteArrayLiteral("ObjectId");

/**
	ID of the \c imeas::INumericValue with the collection object revision in the document source parameters.
*/
static const QByteArray s_revisionParamId = QByteArrayLiteral("Revision");


/**
	Returns the URL of the collection object \a objectId: \c collection:///<objectId>.
	If \a revision is not negative, the URL addresses this revision of the object:
	\c collection:///<objectId>/<revision>.
*/
QUrl ObjectIdToUrl(const QByteArray& objectId, int revision = -1);

/**
	Extracts the collection object ID from a \c collection:///<objectId> or
	\c collection:///<objectId>/<revision> URL.
	Returns an empty ID for unsaved documents (empty URL) or invalid URLs.
*/
QByteArray UrlToObjectId(const QUrl& url);

/**
	Extracts the object revision from a \c collection:///<objectId>/<revision> URL.
	Returns -1 if the URL does not address a revision or is invalid.
*/
int UrlToRevision(const QUrl& url);

/**
	Returns the document source reference of the collection object \a objectId: \c <objectId>.
	If \a revision is not negative, the reference addresses this revision of the object:
	\c <objectId>/<revision>. Returns an empty reference for an empty \a objectId.
*/
QByteArray ObjectIdToSourceRef(const QByteArray& objectId, int revision = -1);

/**
	Extracts the collection object ID from a \c <objectId> or \c <objectId>/<revision>
	document source reference. Returns an empty ID for empty or invalid references.
*/
QByteArray SourceRefToObjectId(const QByteArray& sourceRef);

/**
	Extracts the object revision from a \c <objectId>/<revision> document source reference.
	Returns -1 if the reference does not address a revision or is invalid.
*/
int SourceRefToRevision(const QByteArray& sourceRef);

/**
	Converts the document source reference to the collection URL.
	Returns an empty URL for empty or invalid references.
*/
QUrl SourceRefToUrl(const QByteArray& sourceRef);

/**
	Converts the collection URL to the document source reference.
	Returns an empty reference for empty or invalid URLs.
*/
QByteArray UrlToSourceRef(const QUrl& url);

/**
	Creates the document source parameters of the collection object \a objectId.
	The parameter set contains the \c iprm::IIdParam \c s_objectIdParamId with the object ID and,
	if \a revision is not negative, the \c imeas::INumericValue \c s_revisionParamId with the revision.
	Returns an empty parameter set for an empty \a objectId (unsaved document).
*/
iprm::IParamsSetUniquePtr CreateObjectSourceParams(const QByteArray& objectId, int revision = -1);

/**
	Extracts the collection object ID from the document source parameters.
	Returns an empty ID if the parameters do not reference a collection object (unsaved document).
*/
QByteArray GetObjectIdFromParams(const iprm::IParamsSet& params);

/**
	Extracts the collection object revision from the document source parameters.
	Returns -1 if the parameters do not reference an object revision.
*/
int GetRevisionFromParams(const iprm::IParamsSet& params);


} // namespace imtdoc



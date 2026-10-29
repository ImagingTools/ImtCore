// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

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
	Creates the document source parameters of the collection object \a objectId.
	The parameter set contains the \c iprm::IIdParam \c s_objectIdParamId with the object ID and,
	if \a revision is not negative, the \c imeas::INumericValue \c s_revisionParamId with the revision.
	Returns an empty pointer for an empty \a objectId (unsaved document).
*/
iprm::IParamsSetUniquePtr CreateObjectSourceParams(const QByteArray& objectId, int revision = -1);

/**
	Extracts the collection object ID from the document source parameters.
	Returns an empty ID if \a paramsPtr is null or does not reference a collection object (unsaved document).
*/
QByteArray GetObjectIdFromParams(const iprm::IParamsSet* paramsPtr);

/**
	Extracts the collection object revision from the document source parameters.
	Returns -1 if \a paramsPtr is null or does not reference an object revision.
*/
int GetRevisionFromParams(const iprm::IParamsSet* paramsPtr);


} // namespace imtdoc



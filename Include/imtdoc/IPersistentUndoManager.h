// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <idoc/IUndoManager.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Extends \ref idoc::IUndoManager with document identity awareness.

	Undo manager implementations that persist their state externally (for example to files or to a
	database) typically need to associate the stored undo/redo history with a concrete document
	instance. This interface allows the owner of such an undo manager to provide the identity of the
	observed document — its unique instance ID and its type ID — so that the implementation can
	locate, separate and, if needed, clean up the persisted history data associated to it.
*/
class IPersistentUndoManager: virtual public idoc::IUndoManager
{
public:
	/**
		\brief Initialize persisted-undo context for a concrete document instance.

		\param documentId     Unique ID identifying the document instance.
		\param documentTypeId ID identifying the type of the document.
	*/
	virtual void InitializeDocumentContext(const QByteArray& documentId, const QByteArray& documentTypeId) = 0;

	/**
		\brief Remove undo/redo snapshots and keep only the current document snapshot.
	*/
	virtual void ResetHistory() = 0;
};

using IPersistentUndoManagerUniquePtr = istd::TUniqueInterfacePtr<IPersistentUndoManager>;
using IPersistentUndoManagerSharedPtr = istd::TSharedInterfacePtr<IPersistentUndoManager>;


} // namespace imtdoc

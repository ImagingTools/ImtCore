// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>


// Forward declarations
class QObject;


namespace imtqml
{


/**
	\brief Single, transport-agnostic registry that owns the mapping
	from \c documentId to its \c CDocumentRepresentationController
	instance.

	\details
	Per reviewer feedback (a single bridge serves all document types),
	exactly one implementation of this interface is registered at
	runtime through icomp:

	- \c CRepresentationControllerBridge: the default registry. It is
	  intentionally NOT exposed to QML — controllers self-register
	  through this interface in their \c QObject::componentComplete()
	  override. QML clients never see the bridge directly.

	The registry stores controllers by weak reference so destruction of
	a controller automatically removes its mapping; calls referring to
	an already-destroyed controller MUST return \c nullptr / \c false
	without crashing.
*/
class IRepresentationControllerBridge
{
public:
	virtual ~IRepresentationControllerBridge() = default;

	/**
		\brief Register \c controller under \c documentId. Replaces any
		previous mapping for the same \c documentId.
	*/
	virtual void RegisterRepresentationController(const QString& documentId, QObject* controller) = 0;

	/**
		\brief Remove the mapping for \c documentId. No-op if no entry
		exists.
	*/
	virtual void UnregisterRepresentationController(const QString& documentId) = 0;

	/**
		\brief Currently-registered controller for \c documentId, or
		\c nullptr if none / it has been destroyed.
	*/
	virtual QObject* GetRepresentationController(const QString& documentId) const = 0;

	/**
		\brief All currently registered \c documentId keys (controllers
		that survived the weak-pointer check).
	*/
	virtual QStringList RegisteredRepresentationControllerIds() const = 0;
};


} // namespace imtqml

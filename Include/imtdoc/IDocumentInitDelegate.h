// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace iprm { class IParamsSet; }
namespace istd { class IChangeable; }


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Delegate interface for initializing a newly created data object
	before observers are attached.

	The caller invokes \c InitializeDocument after the data object has been
	created by the factory (and, optionally, populated with default data)
	but before any change-tracking observers are attached.
	This allows the delegate to perform setup that should not be recorded
	as user actions — for example, setting default field values, initializing
	internal state, or applying configuration.

	\note Implementations must be thread-safe.
*/
class IDocumentInitDelegate : virtual public istd::IPolymorphic
{
public:
	/**
		\brief Initialize a data object before change-tracking begins.

		\param typeId       Registered type-ID of the object.
		\param errorString  Out-parameter for a human-readable error description
		                    when the method returns \c false.
		\param initParams   Optional initialization parameters provided by the
		                    caller (may be \c nullptr when no parameters were
		                    supplied).
		\param document     Mutable reference to the data object produced by the
		                    factory.  The delegate may freely mutate this object —
		                    changes will not be recorded by observers.

		\return \c true if initialization succeeded, \c false if it failed
		        (e.g. required parameters were not provided via \a initParams).
	*/
	virtual bool InitializeDocument(
		const QByteArray& typeId,
		istd::IChangeable& document,
		QString& errorString,
		const iprm::IParamsSet* initParams = nullptr) = 0;
};


} // namespace imtdoc



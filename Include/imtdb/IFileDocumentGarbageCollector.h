// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdb
{


/**
 * @interface IFileDocumentGarbageCollector
 * @brief Reclaims unreferenced content in the content-addressed document file store
 *
 * IFileDocumentGarbageCollector is the contract of the garbage collector paired with the
 * content-addressed file store written by CSqlDatabaseFileDocumentDelegateComp. The store
 * is append-only from a writer's point of view: content whose referencing SQL transaction
 * failed or was rolled back is never removed by the delegate, only by the collector. A
 * file store used with a document delegate but without a collector therefore leaks disk
 * forever, so the delegate references this interface to make the collector a required part
 * of any file-document deployment.
 *
 * The collector runs entirely on its own: it starts a timer when it is created and sweeps
 * the store on that interval, so nothing needs to drive it. This interface carries no
 * operations - it exists so the delegate can require a collector by reference (enforcing
 * that one is wired) while staying unit-testable against a mock, without depending on the
 * concrete component.
 *
 * @see CFileDocumentGarbageCollectorComp for the implementation
 * @see CSqlDatabaseFileDocumentDelegateComp for the paired document delegate
 */
class IFileDocumentGarbageCollector: virtual public istd::IPolymorphic
{
};


} // namespace imtdb

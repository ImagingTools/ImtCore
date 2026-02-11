// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// Acula includes
#include <imthype/ITaskResultCollection.h>


namespace imthype
{


/**
	Provider of the task results.
*/
class ITaskResultsProvider: virtual public istd::IInformationProvider
{
public:
	/**
		Get access to the collection of results.
	*/
	virtual const ITaskResultCollection* GetResults() const = 0;
};


} // namespace imthype



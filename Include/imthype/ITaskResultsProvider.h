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



#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

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
	virtual const imthype::ITaskResultCollection* GetResults() const = 0;
};


} // namespace imthype



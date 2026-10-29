// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>


// forward declaration
class QFile;

namespace iprm
{
class IParamsSet;
}


namespace imtsdlgencpp
{


/**
	The files, created for a processing schema. A file that is not generated in the current mode is NULL
*/
struct CxxEntryFileInfo
{
	typedef std::shared_ptr<QFile> FilePtr;

	bool isValid = false;
	FilePtr headerFile;
	FilePtr fwdHeaderFile;
	FilePtr objectHeaderFile;
	FilePtr sourceFile;
	FilePtr objectSourceFile;
};


/**
	Common interface for the processors, that work with the already completed files of the schema.
	In contrast to \c ICxxFileProcessor, it is called once per schema, after all entries are processed and all files are ended.
*/
class ICxxFilePostProcessor
{
public:
	/**
		\param files		- files of the processing schema
		\param paramsPtr	- [optional] additional params for processing
		\returns true - if processing is successful and false otherwise
		\note a post processor MUST silently skip files that it doesn't work with
	 */
	[[nodiscard]] virtual bool ProcessFiles(const CxxEntryFileInfo& files, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtsdlgencpp

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>


// forward declaration
class QIODevice;

namespace iprm 
{
class IParamsSet;
}

namespace imtsdl
{
class CSdlEntryBase;
}


namespace imtsdlgencpp
{


class ICxxFileProcessor
{
public:
	/**
		\brief Processing a file, by modifying file's content
		\param sdlType			- type to process
		\param headerFilePtr	- [usually required] header (.h) file, to write data
		\param sourceFilePtr	- [optional] source(.cpp) file, to write data
		\param paramsPtr		- [optional] additional params for processing
		\returns true - if processing is successful and false otherwise
	 */
	[[nodiscard]] virtual bool ProcessEntry(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* headerDevicePtr, QIODevice* sourceDevicePtr = nullptr, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;

	/**
		\returns the processor type identifier (GQL) or, in the case of an additional modifier, the type of that modifier (JSON). 
	*/
	[[nodiscard]] virtual QString GetProcessorTypeId() const = 0;

};


} // namespace imtsdlgencpp

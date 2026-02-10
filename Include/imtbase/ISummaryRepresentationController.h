// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	General interface for text based representation controller.
*/
class ISummaryRepresentationController: virtual public istd::IPolymorphic
{
public:
	/**
		Create textual representation for object.
	*/
	virtual bool CreateSummaryRepresentation(const istd::IChangeable& object, QString& textRepresentation) const = 0;
};


} // namespace imtbase



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtsdl/TElementList.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imtservergql
{


/**
	Fills the tags of a collection item representation.

	The SQL delegate of a taggable collection (attribute TaggableEntityType) selects the tags of
	every row as the "Tags" column, so filling them costs no extra query:

	\code
	// SDL: type DeviceItem { ... tags: [TagInfo] }
	if (requestInfo.items.isTagsRequested){
		imtservergql::CTagInfoRepresentation::FillTags(objectCollectionIterator, representationObject.tags);
	}
	\endcode
*/
class CTagInfoRepresentation
{
public:
	/**
		Read the tags of the object the iterator is positioned at.
		\return false if the collection does not select tags.
	*/
	static bool FillTags(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				istd::TNullableValue<imtsdl::TElementList<sdl::V1_0::imtbase::CTagInfo>>& tags);
};


} // namespace imtservergql



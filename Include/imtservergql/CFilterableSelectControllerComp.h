// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FilterableSelect.h>


namespace imtservergql
{


class CFilterableSelectControllerComp:
		public sdl::imtbase::FilterableSelect::CFilterableSelectCollectionControllerCompBase
{
public:
	typedef sdl::imtbase::FilterableSelect::CFilterableSelectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFilterableSelectControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::FilterableSelect::CFilterableSelectCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtbase::FilterableSelect::CFilterableSelectListGqlRequest& listRequest,
				sdl::imtbase::FilterableSelect::CSelectableItemData::V1_0& representationObject,
				QString& errorMessage) const override;
};


} // namespace imtservergql

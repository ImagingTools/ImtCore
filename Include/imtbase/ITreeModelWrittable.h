// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtbase
{


class ITreeModelWrittable: virtual public istd::IPolymorphic
{
public:
	virtual bool WriteToModel(imtbase::CTreeItemModel& model, int modelIndex = 0) const = 0;
};


} // namespace imtbase



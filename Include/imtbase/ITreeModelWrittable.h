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



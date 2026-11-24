// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TSimComponentWrap.h>


/**
	Simulated component as a model (see icomp::TModelCompWrap), as the package of the component makes it.

	The last owner takes the context from the component before it is destroyed, as a container of the components does it. Built by MSVC,
	a simulated model with an attribute or a reference fails in its destructor (access violation) when it still has its context: the
	destructor of icomp::TComponentWrap initializes the members of the component that is being destroyed (ACF).
*/
template <class Component>
class TSimModelComponentPtr: public icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<Component>>
{
public:
	~TSimModelComponentPtr()
	{
		if (this->use_count() == 1){
			this->get()->SetComponentContext(icomp::IComponentContextSharedPtr(), nullptr, false);
		}
	}
};



// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CLabConverter.h>


// ACF includes
#include <icmm/CLab.h>


namespace imtserverapp
{


bool CLabConverter::ConvertRepresentationToModel(const sdl::imtcolor::Lab::CLab::V1_0& sdlStruct, icmm::CLab& lab)
{
	if (!sdlStruct.L){
		qWarning() << qPrintable(__FILE__) << __LINE__ << "L is missing, but required";

		return false;
	}
	if (!sdlStruct.a){
		qWarning() << qPrintable(__FILE__) << __LINE__ << "a is missing, but required";

		return false;
	}
	if (!sdlStruct.b){
		qWarning() << qPrintable(__FILE__) << __LINE__ << "b is missing, but required";

		return false;
	}

	lab.SetL(*sdlStruct.L);
	lab.SetA(*sdlStruct.a);
	lab.SetB(*sdlStruct.b);

	return true;
}


bool CLabConverter::ConvertModelToRepresentation(const icmm::CLab& lab, sdl::imtcolor::Lab::CLab::V1_0& sdlStruct)
{
	sdlStruct.a.Emplace() = lab.GetA();
	sdlStruct.b.Emplace() = lab.GetB();
	sdlStruct.L.Emplace() = lab.GetL();

	return true;
}


} // namespace imtserverapp

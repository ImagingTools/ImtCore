// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Generated includes
#include <GeneratedFiles/imtcolorsdl/SDL/1.0/CPP/Lab.h>


namespace icmm
{
class CLab;
}


namespace imtserverapp
{


class CLabConverter
{
public:
	static bool ConvertRepresentationToModel(const sdl::imtcolor::Lab::CLab::V1_0& sdlStruct, icmm::CLab& lab);
	static bool ConvertModelToRepresentation(const icmm::CLab& lab, sdl::imtcolor::Lab::CLab::V1_0& sdlStruct);
};


} // namespace imtserverapp

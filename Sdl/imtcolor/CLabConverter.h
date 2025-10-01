#pragma once


// Generated includes
#include <GeneratedFiles/imtcolorsdl/SDL/1.0/CPP/Lab.h>


namespace icmm
{
class CLab;
}


namespace imtcolorsdl
{


class CLabConverter
{
public:
	static bool ConvertRepresentationToModel(const sdl::imtcolor::Lab::CLab::V1_0& sdlStruct, icmm::CLab& lab);
	static bool ConvertModelToRepresentation(const icmm::CLab& lab, sdl::imtcolor::Lab::CLab::V1_0& sdlStruct);
};


} // namespace imtcolorsdl

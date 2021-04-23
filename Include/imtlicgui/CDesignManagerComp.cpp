#include <imtlicgui/CDesignManagerComp.h>


extern int qInitResources_imtlicguilight();
extern int qCleanupResources_imtlicguilight();
extern int qInitResources_imtlicguidark();
extern int qCleanupResources_imtlicguidark();


namespace imtlicgui
{


// protected methods

void CDesignManagerComp::RegisterResourcesFunctions()
{
	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_LIGHT,
				qInitResources_imtlicguilight,
				qCleanupResources_imtlicguilight);

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_DARK,
				qInitResources_imtlicguidark,
				qCleanupResources_imtlicguidark);
}


} // namespace imtlicgui



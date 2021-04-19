#include <imtgui/CDesignManagerComp.h>


extern int qInitResources_imtguilight();
extern int qCleanupResources_imtguilight();
extern int qInitResources_imtguidark();
extern int qCleanupResources_imtguidark();


namespace imtgui
{


// protected methods

void CDesignManagerComp::RegisterResourcesFunctions()
{
	SetDesignResourcesFunctions(
				imtwidgets::CImtStyle::DS_LIGHT,
				qInitResources_imtguilight,
				qCleanupResources_imtguilight);

	SetDesignResourcesFunctions(
				imtwidgets::CImtStyle::DS_DARK,
				qInitResources_imtguidark,
				qCleanupResources_imtguidark);
}


} // namespace imtgui



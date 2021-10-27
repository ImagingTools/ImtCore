#include <imt3dgui/CDesignManagerComp.h>


extern int qInitResources_imt3dguilight();
extern int qCleanupResources_imt3dguilight();
extern int qInitResources_imt3dguidark();
extern int qCleanupResources_imt3dguidark();


namespace imt3dgui
{


// protected methods

void CDesignManagerComp::RegisterResourcesFunctions()
{
	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_LIGHT,
				qInitResources_imt3dguilight,
				qCleanupResources_imt3dguilight);

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_DARK,
				qInitResources_imt3dguidark,
				qCleanupResources_imt3dguidark);
}


} // namespace imt3dgui



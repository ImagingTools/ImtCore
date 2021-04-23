#include <imtauthgui/CDesignManagerComp.h>


extern int qInitResources_imtauthguilight();
extern int qCleanupResources_imtauthguilight();
extern int qInitResources_imtauthguidark();
extern int qCleanupResources_imtauthguidark();


namespace imtauthgui
{


// protected methods

void CDesignManagerComp::RegisterResourcesFunctions()
{
	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_LIGHT,
				qInitResources_imtauthguilight,
				qCleanupResources_imtauthguilight);

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_DARK,
				qInitResources_imtauthguidark,
				qCleanupResources_imtauthguidark);
}


} // namespace imtauthgui



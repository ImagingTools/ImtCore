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
	qDebug(qPrintable(QString("Registering resources for imtgui library")));

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_LIGHT,
				qInitResources_imtguilight,
				qCleanupResources_imtguilight);

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_DARK,
				qInitResources_imtguidark,
				qCleanupResources_imtguidark);
}


} // namespace imtgui



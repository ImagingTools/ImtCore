#include <imtstyle/CDesignManagerComp.h>


extern int qInitResources_imtstylelight();
extern int qCleanupResources_imtstylelight();
extern int qInitResources_imtstyledark();
extern int qCleanupResources_imtstyledark();


namespace imtstyle
{


// protected methods

void CDesignManagerComp::RegisterResourcesFunctions()
{
	qDebug(qPrintable(QString("Registering resources for imtstyle library")));

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_LIGHT,
				qInitResources_imtstylelight,
				qCleanupResources_imtstylelight);

	SetDesignResourcesFunctions(
				imtstyle::CImtStyle::DS_DARK,
				qInitResources_imtstyledark,
				qCleanupResources_imtstyledark);
}


} // namespace imtstyle



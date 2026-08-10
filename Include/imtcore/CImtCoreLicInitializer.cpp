#include <imtcore/CImtCoreLicInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


namespace imtcore
{


void InitializeImtCoreLic()
{
	Q_INIT_RESOURCE(imtlicguilight);
	Q_INIT_RESOURCE(imtlicguidark);

	Q_INIT_RESOURCE(imtlicguiqml);
}


} // namespace imtcore

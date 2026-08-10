#include <imtcore/CImtCoreLightThemePropertyInitializer.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>


namespace imtcore
{


void CImtCoreLightThemePropertyInitializer::Initialize()
{
	auto* appPtr = QCoreApplication::instance();
	if (appPtr != nullptr)
	{
		appPtr->setProperty("ThemeId", QVariant("Light"));
	}
}


} // namespace imtcore

#include <imtcore/CImtCoreGuiThemeInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreGuiThemeResources()
{
	Q_INIT_RESOURCE(imtguiTheme);
}

namespace imtcore
{


void CImtCoreGuiThemeInitializer::Initialize()
{
	DoInitImtCoreGuiThemeResources();
}


} // namespace imtcore

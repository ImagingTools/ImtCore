#include <imtcore/CImtCoreAuthThemeInitializer.h>

// Qt includes
#include <QtCore/qglobal.h>


Q_ALWAYS_INLINE static void DoInitImtCoreAuthThemeResources()
{
	Q_INIT_RESOURCE(imtauthguiTheme);
}

namespace imtcore
{


void CImtCoreAuthThemeInitializer::Initialize()
{
	DoInitImtCoreAuthThemeResources();
}


} // namespace imtcore

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "DemoTestServer.h"


// ImtCore includes
#include <GeneratedFiles/imtrestdemoTest/CImtRestDemoServer.h>


imtrestdemo::DemoCompositionFactory CreateTestCompositionFactory()
{
	return imtrestdemo::CreateDemoCompositionFactory<CImtRestDemoServer>();
}

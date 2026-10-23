// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrestdemo/DemoComposition.h>


/**
	Get the factory of the compositions of the demo server for the tests. The composition is the class that ARXC generates in this project
	from the partitura of the server application (Impl/ImtRestDemoServer/ImtRestDemoServer.acc), so the tests check the same composition.
*/
imtrestdemo::DemoCompositionFactory CreateTestCompositionFactory();

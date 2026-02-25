// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


// Simple test data class for the tests
class CTestData: public istd::IChangeable
{
public:
	CTestData() {}
	virtual ~CTestData() {}
	
	virtual int GetSupportedOperations() const override { return SO_CLONE | SO_COPY | SO_COMPARE; }
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		return istd::IChangeableUniquePtr(new CTestData());
	}
};



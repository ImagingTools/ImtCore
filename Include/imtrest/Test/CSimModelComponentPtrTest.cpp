// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSimModelComponentPtrTest.h"
#include "TSimModelComponentPtr.h"


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>


namespace
{


/**
	Data with an attribute and a reference of the component, the data are the first base as in the interface of the connection: the
	simplest model that fails in its destructor with MSVC if it has its context still.
*/
class CNumberData: virtual public istd::IChangeable
{
public:
	int number = 0;
};


class CNumberDataComp:
			public CNumberData,
			public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNumberDataComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_numberAttrPtr, "Number", "Initial number", false, 1);
		I_ASSIGN(m_slaveCompPtr, "Slave", "Unused reference", false, "Slave");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override
	{
		BaseClass::OnComponentCreated();

		number = *m_numberAttrPtr;
	}

private:
	I_ATTR(int, m_numberAttrPtr);
	I_REF(istd::IChangeable, m_slaveCompPtr);
};


typedef TSimModelComponentPtr<CNumberDataComp> NumberModel;


} // namespace


// The model takes the attribute on creation and leaves its context before its destructor
void CSimModelComponentPtrTest::ModelWithAttributesIsDestroyedTest()
{
	NumberModel model;
	model->SetIntAttr("Number", 7);
	model->InitComponent();

	QCOMPARE(model->number, 7);
}


I_ADD_TEST(CSimModelComponentPtrTest);



#include <imtbase/CStaticQmlTypeRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlTypeRegistratorComp::OnComponentCreated()
{
	static void* iPtr = nullptr;
	Q_ASSERT_X(iPtr == nullptr, __func__, "You are attempting to create a new instance of the component, however, another instance already exists!");
	iPtr = this;

	BaseClass::OnComponentCreated();

	qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtbase", 1, 0, "TreeItemModel");
}


} // namespace imtbase



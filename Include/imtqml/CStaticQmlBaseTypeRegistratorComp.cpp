#include <imtqml/CStaticQmlBaseTypeRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtqml
{


bool CStaticQmlBaseTypeRegistratorComp::s_isInitialized = false;


// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlBaseTypeRegistratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(s_isInitialized){
		I_IF_DEBUG(qWarning() << __func__ << "Another instance of registrator already exists.");

		return;
	}

	s_isInitialized = true;

	qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtqml", 1, 0, "TreeItemModel");
}


} // namespace imtqml



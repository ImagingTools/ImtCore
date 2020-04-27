#pragma once


// ACF includes
#include <ilog/IMessageContainer.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <istd/CClassInfo.h>
#include <istd/TComposedFactory.h>

#include <iauth/ILogin.h>
#include <ilog/CLogCompBase.h>
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>


namespace imtlog
{


class CLoginEventControllerComp: public ilog::CLogCompBase,	public imod::TSingleModelObserverBase<iauth::ILogin>
{
public:
	typedef ilog::CLogCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<iauth::ILogin> BaseClass2;

	I_BEGIN_COMPONENT(CLoginEventControllerComp);
	I_END_COMPONENT;

	CLoginEventControllerComp();

protected:
	// reimplemented (ilog::CLogCompBase)
	virtual void WriteMessageToLog(const MessagePtr& messagePtr) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask);
};


} // namespace imtlog



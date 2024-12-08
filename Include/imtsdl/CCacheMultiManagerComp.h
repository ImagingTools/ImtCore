#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtsdl/ICacheController.h>
#include <imtsdl/ICacheMultiManager.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


class CCacheMultiManagerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCacheMultiManagerComp)
		I_REGISTER_SUBELEMENT(CacheController);
		I_REGISTER_SUBELEMENT_INTERFACE(CacheController, ICacheController, ExtractCacheController);

		I_REGISTER_SUBELEMENT(CacheMultiManager);
		I_REGISTER_SUBELEMENT_INTERFACE(CacheMultiManager, ICacheMultiManager, ExtractCacheManager);

		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
	I_END_COMPONENT

	CCacheMultiManagerComp();

protected:
	void SetLog(istd::IPolymorphic& loggable, bool reset = false);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);

	std::unique_ptr<ICacheMultiManager> m_cacheMultiManagerPtr;
	std::shared_ptr<ICacheController> m_mainCacheControllerPtr;
	QList<std::shared_ptr<ICacheController>> m_additionalCacheControllerPtrList;

private:
	static ICacheController* ExtractCacheController(CCacheMultiManagerComp& component)
	{
		return component.m_mainCacheControllerPtr.get();
	}

	static ICacheMultiManager* ExtractCacheManager(CCacheMultiManagerComp& component)
	{
		return component.m_cacheMultiManagerPtr.get();
	}
};


} // namespace imtsdl



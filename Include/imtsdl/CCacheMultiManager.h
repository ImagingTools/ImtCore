#pragma once


// ACF includes
#include <ilog/CLoggerBase.h>

// ImtCore includes
#include <imtsdl/ICacheMultiManager.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


class CCacheMultiManager:
			virtual public ilog::CLoggerBase,
			private CSdlTools,
			virtual public ICacheMultiManager
{

public:
	// reimplemented (ICacheMultiManager)
	virtual bool Init(const QList<std::shared_ptr<ICacheController>>& controllers) override;
	virtual bool ResolveIncludePathForType(const CSdlType& type, QString& foundPath) const override;

private:
	static QString GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix = "sdl");
private:
	QList<std::shared_ptr<ICacheController>> m_controllersPtrList;
};


} // namespace imtsdl



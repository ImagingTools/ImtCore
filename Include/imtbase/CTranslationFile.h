#pragma once


// ImtCore includes
#include <imtbase/ITranslationFile.h>


namespace imtbase
{


class CTranslationFile: virtual public ITranslationFile
{
public:
	// reimplemented (ITranslationFile)
	virtual const QString& GetLanguage() const override;
	virtual void SetLanguage(const QString& language) override;
	virtual const QString& GetVersion() const override;
	virtual void SetVersion(const QString& version) override;
	virtual QList<Context> GetContextList() const override;
	virtual bool GetContext(Context& context, const QString& name) const override;
	virtual bool AddContext(const QString& context) override;
	virtual bool RemoveContext(const QString& context) override;
	virtual QList<Message> GetMessages(const QString& contextName) const override;
	virtual bool AddMessage(const QString& contextName, Message message) override;
	virtual bool RemoveMessage(const QString& contextName, const Message& message) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QString m_language;
	QString m_version;
	QList<ITranslationFile::Context> m_contextList;
};


} // namespace imtbase



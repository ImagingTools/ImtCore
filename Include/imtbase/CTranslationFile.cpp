#include <imtbase/CTranslationFile.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// reimplemented (ITranslationFile)

const QString& CTranslationFile::GetLanguage() const
{
	return m_language;
}


void CTranslationFile::SetLanguage(const QString& language)
{
	if (m_language != language){
		istd::CChangeNotifier changeNotifier(this);

		m_language = language;
	}
}


const QString& CTranslationFile::GetVersion() const
{
	return m_version;
}


void CTranslationFile::SetVersion(const QString& version)
{
	if (m_version != version){
		istd::CChangeNotifier changeNotifier(this);

		m_version = version;
	}
}


QList<ITranslationFile::Context> CTranslationFile::GetContextList() const
{
	return m_contextList;
}


bool CTranslationFile::GetContext(Context& context, const QString& name) const
{
	for (const Context& cntx : m_contextList){
		if (cntx.name == name){
			context = cntx;
			return true;
		}
	}

	return false;
}


bool CTranslationFile::AddContext(const QString& contextName)
{
	if (contextName.isEmpty()){
		return false;
	}

	for (const Context& cntx : m_contextList){
		if (cntx.name == contextName){
			return false;
		}
	}

	istd::CChangeNotifier changeNotifier(this);

	Context context;
	context.name = contextName;

	m_contextList << context;

	return true;
}


bool CTranslationFile::RemoveContext(const QString& contextName)
{
	for (const Context& cntx : m_contextList){
		if (cntx.name == contextName){
			istd::CChangeNotifier changeNotifier(this);
			m_contextList.removeAll(cntx);
			return true;
		}
	}

	return false;
}


QList<ITranslationFile::Message> CTranslationFile::GetMessages(const QString& contextName) const
{
	for (const Context& cntx : m_contextList){
		if (cntx.name == contextName){
			return cntx.messages;
		}
	}

	return QList<ITranslationFile::Message>();
}


bool CTranslationFile::AddMessage(const QString& contextName, ITranslationFile::Message message)
{
	Context context;
	if (!GetContext(context, contextName)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_contextList.removeAll(context);
	context.messages.append(message);
	m_contextList << context;

	return true;
}


bool CTranslationFile::RemoveMessage(const QString& contextName, const ITranslationFile::Message& message)
{
	Context context;
	if (!GetContext(context, contextName)){
		return false;
	}

	if (!context.messages.contains(message)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_contextList.removeAll(context);
	context.messages.removeAll(message);
	m_contextList << context;

	return true;
}


// reimplemented (iser::ISerializable)

bool CTranslationFile::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag tsTag("TS", "TS", iser::CArchiveTag::TT_MULTIPLE);

	bool retVal = true;

	iser::CArchiveTag contextTag("context", "Context item", iser::CArchiveTag::TT_GROUP, &tsTag);

	int objectCount = m_contextList.count();

	if (!archive.IsStoring()){
		objectCount = 0;

		m_contextList.clear();
	}

	retVal = retVal && archive.BeginMultiTag(tsTag, contextTag, objectCount);

	iser::CArchiveTag versionTag("version", "Version", iser::CArchiveTag::TT_LEAF, &tsTag);
	retVal = retVal && archive.BeginTag(versionTag);
	retVal = retVal && archive.Process(m_version);
	retVal = retVal && archive.EndTag(versionTag);

	iser::CArchiveTag languageTag("language", "Language", iser::CArchiveTag::TT_LEAF, &tsTag);
	retVal = retVal && archive.BeginTag(languageTag);
	retVal = retVal && archive.Process(m_language);
	retVal = retVal && archive.EndTag(languageTag);

	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(contextTag);

		Context context;
		if (archive.IsStoring()){
			context = m_contextList[i];
		}

		iser::CArchiveTag contextNameTag("name", "Context name", iser::CArchiveTag::TT_GROUP, &contextTag);
		retVal = retVal && archive.BeginTag(contextNameTag);
		retVal = retVal && archive.Process(context.name);
		retVal = retVal && archive.EndTag(contextNameTag);

		int messagesCount = context.messages.count();

		if (!archive.IsStoring()){
			messagesCount = 0;
			context.messages.clear();
		}

		for (int j = 0; j < messagesCount; j++){
			Message message;
			if (archive.IsStoring()){
				message = context.messages[j];
			}

			iser::CArchiveTag messageTag("message", "Context message", iser::CArchiveTag::TT_GROUP, &contextTag);
			retVal = retVal && archive.BeginTag(messageTag);

			iser::CArchiveTag sourceTag("source", "Message source", iser::CArchiveTag::TT_GROUP, &messageTag);
			retVal = retVal && archive.BeginTag(sourceTag);
			retVal = retVal && archive.Process(message.source);
			retVal = retVal && archive.EndTag(sourceTag);

			iser::CArchiveTag translationTag("translation", "Message translation", iser::CArchiveTag::TT_GROUP, &messageTag);
			retVal = retVal && archive.BeginTag(translationTag);
			retVal = retVal && archive.Process(message.translation);
			retVal = retVal && archive.EndTag(translationTag);

			retVal = retVal && archive.EndTag(messageTag);
		}

		retVal = retVal && archive.EndTag(contextTag);
	}

	retVal = retVal && archive.EndTag(tsTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTranslationFile::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_OBSERVE | SO_RESET;
}


bool CTranslationFile::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTranslationFile* sourcePtr = dynamic_cast<const CTranslationFile*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_language = sourcePtr->m_language;
		m_version = sourcePtr->m_version;
		m_contextList = sourcePtr->m_contextList;

		return true;
	}

	return false;
}


bool CTranslationFile::IsEqual(const IChangeable& object) const
{
	const CTranslationFile* sourcePtr = dynamic_cast<const CTranslationFile*>(&object);
	if (sourcePtr != NULL){
		return	m_language == sourcePtr->m_language &&
				m_version == sourcePtr->m_version &&
				m_contextList == sourcePtr->m_contextList;
	}

	return false;
}


istd::IChangeable* CTranslationFile::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CTranslationFile> clonePtr(new CTranslationFile);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CTranslationFile::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_language.clear();
	m_version.clear();
	m_contextList.clear();

	return true;
}


} // namespace imtbase



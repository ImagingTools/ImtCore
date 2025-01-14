#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class ITranslationFile: virtual public iser::ISerializable
{
public:
	enum TranslationType
	{
		TT_FINISHED,
		TT_UNFINISHED,
		TT_VANISHED,
		TT_OBSOLETE
	};

	struct Location
	{
		QString line;
		QString fileName;

		bool operator==(const Location& other) const
		{
			return	(line == other.line) &&
					(fileName == other.fileName);
		}

		bool operator!=(const Location& other) const
		{
			return !operator==(other);
		}
	};

	struct Message
	{
		QString source;
		QString translation;
		TranslationType type = TT_UNFINISHED;
		QList<Location> locations;

		bool operator==(const Message& other) const
		{
			return	(source == other.source) &&
					(translation == other.translation) &&
					(type == other.type) &&
					(locations == other.locations);
		}

		bool operator!=(const Message& other) const
		{
			return !operator==(other);
		}
	};

	struct Context
	{
		QString name;
		QList<Message> messages;

		bool operator==(const Context& other) const
		{
			return (name == other.name) && (messages == other.messages);
		}

		bool operator!=(const Context& other) const
		{
			return !operator==(other);
		}
	};

	virtual const QString& GetLanguage() const = 0;
	virtual void SetLanguage(const QString& language) = 0;
	virtual const QString& GetVersion() const = 0;
	virtual void SetVersion(const QString& version) = 0;
	virtual QList<Context> GetContextList() const = 0;
	virtual bool GetContext(Context& context, const QString& name) const = 0;
	virtual bool AddContext(const QString& contextName) = 0;
	virtual bool RemoveContext(const QString& contextName) = 0;
	virtual QList<Message> GetMessages(const QString& contextName) const = 0;
	virtual bool AddMessage(const QString& contextName, Message message) = 0;
	virtual bool RemoveMessage(const QString& contextName, const Message& message) = 0;
};


} // namespace imtbase



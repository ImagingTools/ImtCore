#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>



namespace imtsdl
{


class CSdlField: public iser::ISerializable
{
public:
	CSdlField();

	bool IsRequired() const;
	void SetIsRequired(bool isRequired);

	bool IsArray() const;
	void SetIsArray(bool isArray);

	bool IsNonEmpty() const;
	void SetIsNonEmpty(bool isNonEmpty);

	QString GetType() const;
	void SetType(const QString& type);

	QString GetId() const;
	void SetId(const QString& id);

	// operators
	bool operator==(const CSdlField& other) const;

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	/**
		Defines is value may be NULL
	*/
	bool m_isRequired;
	/**
		Defines whether the value is an array (contains several elements of the same type)
	*/
	bool m_isArray;
	/**
		This only makes sence only for arrays. Defines is array must contain at least one value
	*/
	bool m_isNonEmpty;
	/**
		Defines a type of value (int, string, user's custom type)
	*/
	QString m_type;
	/**
		Defines id, used in requests and responses
	*/
	QString m_id;

};


} // namespace imtsdl

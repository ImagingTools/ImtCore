#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


class CGqlRequest: virtual public IGqlRequest
{
public:
	CGqlRequest(RequestType requestType = RT_QUERY, const QByteArray& commandId = QByteArray());
	~CGqlRequest();

	void AddParamValue(const QByteArray& paramId, const QVariant& value);
	void AddParamValue(const QByteArray& paramId, const imtgql::CGqlEnum& value);
	void AddParam(const QByteArray& paramId, const CGqlObject& param);
	void AddField(const QByteArray& fieldId, const CGqlObject& field);
	void AddSimpleField(const QByteArray& fieldId);
	void SetGqlContext(const imtgql::IGqlContext* gqlContext);
	void SetRequestType(RequestType requestType);
	void SetCommandId(const QByteArray& commandId);
	QByteArray GetHeader(QByteArray headerId);

	const CGqlObject& GetFields() const;
	const CGqlObject& GetParams() const;

	void SetParams(const CGqlObject& params);

	const CGqlObject* GetFieldObject(const QByteArray& fieldId) const;
	const CGqlObject* GetParamObject(const QByteArray& paramId) const;

	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;
	virtual imtgql::IGqlContext* GetRequestContext() const override;
	virtual bool ParseQuery(const QByteArray& query, int &errorPosition);

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual QByteArray CreateQueryFields() const;
	virtual QByteArray CreateQueryParams() const;
	virtual QByteArray AddObjectFieldPart(const CGqlObject& gqlObject) const;
	virtual QByteArray AddObjectParamPart(const CGqlObject& gqlObject) const;
	virtual QByteArray AddObjectParamValue(const QVariant& value) const;
	virtual void ParceObjectFieldPart(CGqlObject& gqlObject, const QJsonObject& object) const;
	virtual void ParceObjectParamPart(CGqlObject& gqlObject, const QJsonObject& object) const;

private:
	void SetParseObject(const QByteArray& commandId);
	void SetParseText(const QByteArray& text);
	void StartArray(const QByteArray& text);
	bool CloseArray();

protected:
	QByteArray m_commandId;
	RequestType m_requestType;
	CGqlObject m_params;
	CGqlObject m_fields;

	bool m_startKey;
	bool m_startValue;
	bool m_startParams;
	bool m_startFields;
	bool m_startArrayPrimitives;
	bool m_textString;
	QByteArray m_currentField;

	CGqlObject* m_activeGqlObjectPtr;
	imtgql::IGqlContext* m_gqlContextPtr;
	QList<QByteArray> m_activeArrayIds;
};


} // namespace imtgql



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

	void AddParam(const CGqlObject& param);
	void AddField(const CGqlObject& field);
	void AddSimpleField(const QByteArray& fieldId);
	void SetGqlContext(const imtgql::IGqlContext* gqlContext);

	const QList<CGqlObject> GetFields() const;
	const QList<CGqlObject> GetParams() const;

	const CGqlObject* GetField(const QByteArray& fieldId) const;
	const CGqlObject* GetParam(const QByteArray& paramId) const;

	void SetField(const CGqlObject& gqlObject);
	void SetParam(const CGqlObject& gqlObject);

	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;
	virtual imtgql::IGqlContext* GetGqlContext() const override;
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
	virtual void ParceObjectFieldPart(CGqlObject &gqlObject, const QJsonObject& object) const;
	virtual void ParceObjectParamPart(CGqlObject &gqlObject, const QJsonObject& object) const;

private:
	void SetParseObject(const QByteArray& commandId);
	void SetParseText(const QByteArray& text);

protected:
	QByteArray m_commandId;
	RequestType m_requestType;
	QList<CGqlObject> m_params;
	QList<CGqlObject> m_fields;

	bool m_startKey;
	bool m_startValue;
	bool m_startParams;
	bool m_startFields;
	QByteArray m_currentField;

	CGqlObject* m_activeGqlObjectPtr;
	imtgql::IGqlContext* m_gqlContextPtr;
};


} // namespace imtgql



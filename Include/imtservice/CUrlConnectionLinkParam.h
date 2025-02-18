#pragma once


// ImtCore includes
#include <imtservice/CServiceConnectionInfo.h>
#include <imtservice/IServiceConnectionLinkParam.h>


namespace imtservice
{


class CUrlConnectionLinkParam:
			public imtservice::CServiceConnectionInfo,
			public virtual imtservice::IServiceConnectionLinkParam
{
public:
	typedef imtservice::CServiceConnectionInfo BaseClass;
	
	void SetDependantServiceConnectionId(const QByteArray& connectionId);

	// reimplemented (imtservice::IServiceConnectionLinkParam)
	virtual QByteArray GetDependantServiceConnectionId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_dependantServiceConnectionId;
};


} // namespace imtservice




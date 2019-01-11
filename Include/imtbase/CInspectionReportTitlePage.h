#pragma once


// ImtCore includes
#include <imtbase/IReportPage.h>


namespace imtbase
{


class CInspectionReportTitlePage: virtual public IReportPage
{
public:
	/**
		Get the name of the produced part.
	*/
	QString GetProductName() const;

	/**
		Set the name of the produced part.
	*/
	void SetProductName(const QString& productName);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive) override;

private:
	QString m_productName;
};


} // namespace imtbase



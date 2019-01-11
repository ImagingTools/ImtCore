#pragma once


// ImtCore includes
#include <imtbase/CReportPage.h>


namespace imtbase
{


class CInspectionReportTitlePage: public CReportPage
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



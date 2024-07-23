#pragma once

// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/CDocumentTable.h>


namespace imtmdbx
{


class CMask: virtual public IMask, public CDocumentTable
{
public:
	CMask(const QString& name, mdbx::txn_managed& txn, mdbx::key_mode keyMode = mdbx::key_mode::ordinal, mdbx::value_mode valueMode = mdbx::value_mode::single, bool hasIndex = false);

	virtual bool GetUnit(qint64 position) override;
	virtual bool SetUnit(qint64 position, bool unit) override;

	virtual quint64 GetItem(qint64 offset) override;
	virtual bool SetItem(qint64 offset, quint64 item) override;
	virtual qint64 GetNextItemOffset(qint64 startOffset = -1) override;
	virtual qint64 GetPreviosItemOffset(qint64 startOffset = 0xffffffff) override;

private:
    mdbx::map_handle m_mapHandle;
	//qint64 m_value;
	//qint64 m_offset;
};

}//namespace imtmdbx

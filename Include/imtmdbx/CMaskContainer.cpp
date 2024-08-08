#include <imtmdbx/CMaskContainer.h>


namespace imtmdbx
{


static int BitsSetTable256[256];
static bool isInit = false;

int BitCount(quint64 n)
{

	return (BitsSetTable256[n & 0xff] +
			BitsSetTable256[(n >> 8) & 0xff] +
			BitsSetTable256[(n >> 16) & 0xff] +
			BitsSetTable256[(n >> 24) & 0xff] +
			BitsSetTable256[(n >> 32) & 0xff] +
			BitsSetTable256[(n >> 40) & 0xff] +
			BitsSetTable256[(n >> 48) & 0xff] +
			BitsSetTable256[n >> 56]);

}

void Initialize()
{

	// To initially generate the
	// table algorithmically
	BitsSetTable256[0] = 0;
	for (int i = 0; i < 256; i++)
	{
		BitsSetTable256[i] = (i & 1) +
				BitsSetTable256[i / 2];
	}
}


CMaskContainer::CMaskContainer(OperationType operationType):
	m_operationType(operationType), m_documentTable(nullptr)
{
	if (!isInit){
		Initialize();
	}
}


void CMaskContainer::AddMask(IMask *mask, bool isInversion)
{
	if(!isInversion){
		m_maskList.append(mask);
	}
	else {
		m_maskListInv.append(mask);
	}
}

void CMaskContainer::Clear()
{
	m_maskList.clear();
	m_maskListInv.clear();
	m_maskList.squeeze();
	m_maskListInv.squeeze();
}

bool CMaskContainer::RemoveLastMask(bool isInversion)
{
	bool ok = true;
	if(!isInversion){
		if(!m_maskList.isEmpty()){
			m_maskList.removeLast();
			m_maskList.squeeze();
		}
		else {
			ok = false;
		}
	}
	else {
		if(!m_maskListInv.isEmpty()){
			m_maskListInv.removeLast();
			m_maskListInv.squeeze();
		}
		else {
			ok = false;
		}
	}

	return ok;
}

bool CMaskContainer::RemoveMask(int index, bool isInversion)
{
	bool ok = true;
	if(!isInversion){
		if(!m_maskList.isEmpty() && index < m_maskList.length()){
			m_maskList.remove(index);
			m_maskList.squeeze();
		}
		else {
			ok = false;
		}
	}
	else {
		if(!m_maskListInv.isEmpty() && index < m_maskListInv.length()){
			m_maskListInv.remove(index);
			m_maskListInv.squeeze();
		}
		else {
			ok = false;
		}

	}

	return ok;
}

int CMaskContainer::GetMaskCount(bool isInversion)
{
	int count = 0;
	if(!isInversion){
		count = m_maskList.length();
	}
	else {
		count = m_maskListInv.length();
	}
	return count;
}

void CMaskContainer::SetDocumentTable(IDocumentTable* documentTable)
{
	m_documentTable = documentTable;
}


quint64 CMaskContainer::GetUnitCount()
{
	quint64 activeOffset = 0;
	quint64 unitCount = 0;
	bool isStart = true;

	int whileCount = 0;
	while(1){
		quint64 activeItem = 0xffffffffffffffff;

		if (GetActiveItem(activeOffset, activeItem, isStart) == false){
			return unitCount;
		}

		isStart = false;

		unitCount += BitCount(activeItem);
	}

	return unitCount;
}


QList<quint64> CMaskContainer::GetUnitPositions(quint64 offset, int limit)
{
	QList<quint64> list;
	quint64 activeOffset = 0;
	quint64 unitCount = 0;
	bool isStart = true;

	while(1){
		quint64 activeItem = 0xffffffffffffffff;

		if (GetActiveItem(activeOffset, activeItem, isStart) == false){
			return list;
		}

		isStart = false;

		for(int i = 0; i < 64; i++){
			bool ok = (activeItem >> i) & (quint64)1;//check activeItem
			if(ok){
				unitCount++;
				if (unitCount > offset){
					int position = activeOffset * 64 + i;
					list.append(position);
					if(list.length() >= limit){
						return list;
					}
				}
			}
		}
	}

    return list;
}


QList<QByteArray> CMaskContainer::GetDocuments(quint64 offset, int limit)
{
	QList<QByteArray> list;
	quint64 activeOffset = 0;
	quint64 unitCount = 0;
	bool isStart = true;

	if (m_documentTable == nullptr){
		return list;
	}

	while(1){
		quint64 activeItem = 0xffffffffffffffff;

		if (GetActiveItem(activeOffset, activeItem, isStart) == false){
			return list;
		}

		isStart = false;

		for(int i = 0; i < 64; i++){
			bool ok = (activeItem >> i) & (quint64)1;//check activeItem
			if(ok){
				unitCount++;
				if (unitCount > offset){
					int position = activeOffset * 64 + i;
					list.append(m_documentTable->GetDocument(position));
					if(list.length() >= limit){
						return list;
					}
				}
			}
		}
	}

	return list;
}


bool CMaskContainer::GetActiveItem(quint64& activeOffset, quint64& activeItem, bool isStart)
{
	qint64 maxOffset = activeOffset;
	activeItem = 0xffffffffffffffff;
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 offset = 0;
		if (isStart){
			if (!m_maskList.at(i)->GetNearestOffset(offset, activeOffset)){
				return false;
			}
		}
		else{
			if (!m_maskList.at(i)->GetNextItemOffset(offset, activeOffset)){
				return false;
			}
		}
		qint64 offset_ = offset;
		if(offset_ > maxOffset){
			maxOffset = offset;
		}
	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		quint64 offset = 0;
		if (isStart){
			if (!m_maskListInv.at(i)->GetNearestOffset(offset, activeOffset)){
				return false;
			}
		}
		else{
			if(!m_maskListInv.at(i)->GetNextItemOffset(offset, activeOffset)){
				return false;
			}
		}
		qint64 offset_ = offset;
		if(offset_ > maxOffset){
			maxOffset = offset;
		}
	}

	activeOffset = maxOffset;

	for(int i = 0; i < m_maskList.length(); i++){
		quint64 item;
		if(activeOffset >= 0){
			if (!m_maskList.at(i)->GetItem(activeOffset, item)){
				item = 0;
			}
		}
		else {
			item = 0;
		}

		if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
			activeItem = activeItem & item;
		}
		else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
			activeItem = activeItem | item;
		}


	}
	for(int i = 0; i < m_maskListInv.length(); i++){

		quint64 item;
		if(activeOffset >= 0){
			if (!m_maskListInv.at(i)->GetItem(activeOffset, item)){
				item = 0;
			}
		}
		else {
			item = 0;
		}


		if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
			activeItem = activeItem & ~item;//Inversion
		}
		else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
			activeItem = activeItem | ~item;//Inversion
		}
	}

	return true;
}


}//namespace imtmdbx

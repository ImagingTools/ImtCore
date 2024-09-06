#include <imtmdbx/CMaskContainer.h>

// Qt includes
#include <QtCore/QDebug>


namespace imtmdbx
{


static int BitsSetTable256[256];
static bool isInit = false;

inline int BitCount(quint64 n)
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

//reimplemented (IMask)
bool CMaskContainer::GetUnit(quint64 position)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}
	bool firstUnit  = m_operationType == OT_AND ? true : false;

	bool retUnit = firstUnit;

	for(int i = 0; i < m_maskList.length(); i++){
		bool currUnit = m_maskList.at(i).first->GetUnit(position);
		if(m_operationType == OT_AND){
			retUnit = retUnit & currUnit;
		}
		else if(m_operationType == OT_OR){
			retUnit = retUnit | currUnit;
		}

	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		bool currUnit = m_maskListInv.at(i).first->GetUnit(position);
		if(m_operationType == OT_AND){
			retUnit = retUnit & currUnit;
		}
		else if(m_operationType == OT_OR){
			retUnit = retUnit | currUnit;
		}

	}

	return retUnit;
}


bool CMaskContainer::SetUnit(quint64 position, bool unit)
{
	return false;
}


bool CMaskContainer::GetItem(quint64 offset, quint64 &item)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	quint64 retItem = m_operationType == OT_AND ? 0xffffffffffffff : 0;

	for(int i = 0; i < m_maskList.length(); i++){
		quint64 currItem = 0;
		bool okCurr = false;
		if(m_maskList.at(i).second <= offset){
			okCurr = m_maskList.at(i).first->GetItem(offset, currItem);
		}

		if(!okCurr){
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}

		if(m_operationType == OT_AND){
			retItem = retItem & currItem;
		}
		else if(m_operationType == OT_OR){
			retItem = retItem | currItem;
		}
	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		quint64 currItem = 0;
		bool okCurr = false;
		if(m_maskListInv.at(i).second <= offset){
			okCurr = m_maskListInv.at(i).first->GetItem(offset, currItem);
		}
		if(!okCurr){
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}
		if(m_operationType == OT_AND){
			retItem = retItem & currItem;
		}
		else if(m_operationType == OT_OR){
			retItem = retItem | currItem;
		}
	}

	item = retItem;

	return true;
}


bool CMaskContainer::SetItem(quint64 offset, quint64 item)
{
	return false;
}


bool CMaskContainer::GetNearestOffset(quint64 &offset, quint64 startOffset)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	bool isLast = true;

	quint64 retOffset = m_operationType == imtmdbx::CMaskContainer::OT_AND ? 0 : 0xffffffffffffffff;
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 currOffset;
		bool okCurr = m_maskList.at(i).first->GetNearestOffset(currOffset, startOffset);
		if(!okCurr){
			m_maskList[i].second = 0xffffffffffffffff;
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}
		else{
			m_maskList[i].second = currOffset;
			isLast = false;
		}
		if(m_operationType == OT_AND){
			if(currOffset > retOffset){
				retOffset = currOffset;
			}
		}
		else if(m_operationType == OT_OR){
			if(currOffset < retOffset){
				retOffset = currOffset;
			}
		}
	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		quint64 currOffset;
		bool okCurr = m_maskListInv.at(i).first->GetNearestOffset(currOffset, startOffset);
		if(!okCurr){
			m_maskListInv[i].second = 0xffffffffffffffff;
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}
		else{
			m_maskListInv[i].second = currOffset;
			isLast = false;
		}
		if(m_operationType == OT_AND){
			if(currOffset > retOffset){
				retOffset = currOffset;
			}
		}
		else if(m_operationType == OT_OR){
			if(currOffset < retOffset){
				retOffset = currOffset;
			}
		}
	}

	offset = retOffset;

	if(isLast){
		return false;
	}

	return true;
}


bool CMaskContainer::GetNextItemOffset(quint64 &offset, quint64 startOffset)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	bool isLast = true;

	quint64 retOffset = 0;
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 currOffset;
		if (m_maskList.at(i).second <= startOffset){
			bool okCurr = m_maskList.at(i).first->GetNextItemOffset(currOffset, startOffset);
			if(!okCurr){
				m_maskList[i].second = 0xffffffffffffffff;
				if(m_operationType == OT_AND){
					return false;
				}
				else if(m_operationType == OT_OR){
					continue;
				}
			}
			else{
				m_maskList[i].second = currOffset;
				isLast = false;
			}
			if(m_operationType == OT_AND){
				if(currOffset > retOffset){
					retOffset = currOffset;
				}
			}
			else if(m_operationType == OT_OR){
				if(currOffset < retOffset){
					retOffset = currOffset;
				}
			}

		}
		else {
			currOffset = m_maskList[i].second;
			isLast = false;
		}
	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		quint64 currOffset;
		if (m_maskList.at(i).second <= startOffset){
			bool okCurr = m_maskListInv.at(i).first->GetNextItemOffset(currOffset, startOffset);
			if(!okCurr){
				m_maskListInv[i].second = 0xffffffffffffffff;
				if(m_operationType == OT_AND){
					return false;
				}
				else if(m_operationType == OT_OR){
					continue;
				}
			}
			else{
				m_maskListInv[i].second = currOffset;
				isLast = false;
			}
			if(m_operationType == OT_AND){
				if(currOffset > retOffset){
					retOffset = currOffset;
				}
			}
			else if(m_operationType == OT_OR){
				if(currOffset < retOffset){
					retOffset = currOffset;
				}
			}

		}
		else {
			currOffset = m_maskListInv[i].second;
			isLast = false;
		}
	}

	offset = retOffset;

	if(isLast || offset == 0xffffffffffffffff){
		return false;
	}

	return true;
}


bool CMaskContainer::GetPreviosItemOffset(quint64 &offset, quint64 startOffset)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	quint64 retOffset = 0;
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 currOffset;
		bool okCurr = m_maskList.at(i).first->GetPreviosItemOffset(currOffset, startOffset);
		if(!okCurr){
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}
		if(m_operationType == OT_AND){
			if(currOffset < retOffset){
				retOffset = currOffset;
			}
		}
		else if(m_operationType == OT_OR){
			if(currOffset > retOffset){
				retOffset = currOffset;
			}
		}
	}
	for(int i = 0; i < m_maskListInv.length(); i++){
		quint64 currOffset;
		bool okCurr = m_maskListInv.at(i).first->GetPreviosItemOffset(currOffset, startOffset);
		if(!okCurr){
			if(m_operationType == OT_AND){
				return false;
			}
			else if(m_operationType == OT_OR){
				continue;
			}
		}
		if(m_operationType == OT_AND){
			if(currOffset < retOffset){
				retOffset = currOffset;
			}
		}
		else if(m_operationType == OT_OR){
			if(currOffset > retOffset){
				retOffset = currOffset;
			}
		}
	}

	offset = retOffset;

	return true;
}
//reimplemented (IMask)


void CMaskContainer::AddMask(IMask *mask, bool isInversion)
{
	if(!isInversion){
		m_maskList.append(QPair<IMask*,int>(mask,0));
	}
	else {
		m_maskListInv.append(QPair<IMask*,int>(mask,0));
	}
}


void CMaskContainer::Clear()
{
	m_maskList.clear();
	m_maskListInv.clear();
	m_maskList.squeeze();
	m_maskListInv.squeeze();
}


void CMaskContainer::Erase()
{
	for(int i = 0; i < m_maskList.length(); i++){
		delete m_maskList.at(i).first;
	}

	for(int i = 0; i < m_maskListInv.length(); i++){
		delete m_maskListInv.at(i).first;
	}

	Clear();

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

bool CMaskContainer::RemoveMask(int index, int n, bool isInversion)
{
	bool ok = true;
	if(!isInversion){
		if(!m_maskList.isEmpty() && index + n -1  < m_maskList.length()){
			m_maskList.remove(index, n);
			m_maskList.squeeze();
		}
		else {
			ok = false;
		}
	}
	else {
		if(!m_maskListInv.isEmpty() && index + n -1 < m_maskListInv.length()){
			m_maskListInv.remove(index, n);
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
			//qDebug() << "whileCount " << whileCount;

			return unitCount;
		}

		isStart = false;

		whileCount++;

		unitCount += BitCount(activeItem);
	}

	qDebug() << "whileCount " << whileCount;

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
	quint64 maxOffset = activeOffset;
	quint64 minOffset = 0xffffffffffffffff;
	activeItem = m_operationType == imtmdbx::CMaskContainer::OT_AND ? 0xffffffffffffffff : 0;
	bool isLast = true;
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 offset = 0;
		bool changeOffset = false;
		if (isStart){
			if (!m_maskList.at(i).first->GetNearestOffset(offset, activeOffset)){
				if(m_operationType == OT_AND){
					return false;
				}
				else if(m_operationType == OT_OR){
					m_maskList[i].second = 0xffffffffffffffff;
					continue;
				}
			}
			else {//found nearest offset
				m_maskList[i].second = offset;
				isLast = false;
				changeOffset = true;
			}
		}
		else{//not start
			if (m_maskList.at(i).second <= activeOffset){
				if (!m_maskList.at(i).first->GetNextItemOffset(offset, activeOffset)){
					//qDebug() << "NOT FOUND!!!";
					if(m_operationType == OT_AND){
						return false;
					}
					else if(m_operationType == OT_OR){
						m_maskList[i].second = 0xffffffffffffffff;
						continue;
					}
				}
				else {//found next offset
					m_maskList[i].second = offset;
					isLast = false;
					changeOffset = true;
				}
			}
			else {//m_maskList.at(i).second > activeOffset
				offset = m_maskList[i].second;
				changeOffset = true;
				isLast = false;
			}

		}//not start

		/****************if (changeOffset)***********/
		if (changeOffset){
			if(offset == 0xffffffffffffffff){
				//qDebug() << "0xffffffffffffffff";
			}

			if(offset > maxOffset){
				maxOffset = offset;
			}
			if(offset < minOffset){
				minOffset = offset;
			}
		}
	}//for

	for (int i = 0; i < m_maskListInv.length(); i++){
		quint64 offset = 0;
		bool changeOffset = false;
		if (isStart){
			if (!m_maskListInv.at(i).first->GetNearestOffset(offset, activeOffset)){
				m_maskListInv[i].second = 0xffffffffffffffff;
				if(m_operationType == OT_AND){
					return false;
				}
				else if(m_operationType == OT_OR){
					continue;
				}
			}
			else {
				m_maskListInv[i].second = offset;
				isLast = false;
				changeOffset = true;
				isLast = false;
			}
		}
		else{
			if (m_maskListInv.at(i).second <= activeOffset){
				if (!m_maskListInv.at(i).first->GetNextItemOffset(offset, activeOffset)){
					m_maskListInv[i].second = 0xffffffffffffffff;
					isLast = false;
					if(m_operationType == OT_AND){
						return false;
					}
					else if(m_operationType == OT_OR){
						continue;
					}
				}
				else {
					m_maskListInv[i].second = offset;
					isLast = false;
					changeOffset = true;
				}
			}
			else {
				offset = m_maskListInv[i].second;
				changeOffset = true;
				isLast = false;
			}
		}
		if (changeOffset){
			if(offset > maxOffset){
				maxOffset = offset;
			}
			if(offset < minOffset){
				minOffset = offset;
			}
		}
	}


	/*******************conditions***************************/
//	if(maxOffset == 0xffffffffffffffff || minOffset == 0xffffffffffffffff){
//		isLast = true;
//	}

	if(isLast){
		return false;
	}

	if(m_operationType == OT_AND){
		if(maxOffset >= activeOffset){
			activeOffset = maxOffset;
		}
		else {
			Q_ASSERT(0);
		}
	}
	else if(m_operationType == OT_OR){
		if(minOffset == 0xffffffffffffffff){
			//qDebug() << "0xffffffffffffffff";
		}
		if(minOffset >= activeOffset){
			activeOffset = minOffset;
		}
		else {
			Q_ASSERT(0);
		}

	}	/*******************conditions***************************/




	/*******************calculation***************************/
	for(int i = 0; i < m_maskList.length(); i++){
		quint64 item = 0;
		if(m_maskList.at(i).second <= activeOffset){
			if (!m_maskList.at(i).first->GetItem(activeOffset, item)){
				item = 0;
			}
		}

		if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
			activeItem = activeItem & item;
		}
		else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
			activeItem = activeItem | item;
		}


	}
	for(int i = 0; i < m_maskListInv.length(); i++){

		quint64 item = 0;
		if(m_maskListInv.at(i).second <= activeOffset){
			if (!m_maskListInv.at(i).first->GetItem(activeOffset, item)){
				item = 0;
			}
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

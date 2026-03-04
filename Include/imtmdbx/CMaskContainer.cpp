// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMaskContainer.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtmdbx
{


// public methods

CMaskContainer::CMaskContainer(OperationType operationType):
	m_operationType(operationType),
	m_documentTable(nullptr)
{
}


void CMaskContainer::Erase()
{
	for (auto& [maskPtr, nextOffset] : m_maskList){
		delete maskPtr;
	}

	for (auto& [maskPtr, nextOffset] : m_maskListInv){
		delete maskPtr;
	}

	Clear();
}


bool CMaskContainer::isEmpty()
{
	return (m_maskList.isEmpty() && m_maskListInv.isEmpty());
}


bool CMaskContainer::GetActiveItem(quint64& activeOffset, quint64& activeItem, bool isStart)
{
	activeItem = m_operationType == imtmdbx::CMaskContainer::OT_AND ? imtmdbx::QUINT64_MAX : 0;

	quint64 maxOffset = activeOffset;
	quint64 minOffset = imtmdbx::QUINT64_MAX;
	bool isLast = true;

	auto processOffsets = [&](QList<QPair<IMask*,quint64>>& list) -> bool
	{
		for (auto& [maskPtr, nextOffset] : list) {
			quint64 offset = 0;
			bool changeOffset = false;

			if (isStart){
				if (maskPtr->GetNearestOffset(offset, activeOffset)){
					// found nearest offset
					nextOffset = offset;
					isLast = false;
					changeOffset = true;
				}
				else{
					if (m_operationType == OT_AND){
						return false;
					}

					if (m_operationType == OT_OR){
						nextOffset = imtmdbx::QUINT64_MAX;

						continue;
					}
				}
			}
			else {
				if (nextOffset > activeOffset){
					// found next offset
					offset = nextOffset;
					isLast = false;
					changeOffset = true;
				}
				else{
					if (maskPtr->GetNextItemOffset(offset, activeOffset)){
						// found next offset
						nextOffset = offset;
						isLast = false;
						changeOffset = true;
					}
					else{
						if (m_operationType == OT_AND){
							return false;
						}

						if (m_operationType == OT_OR){
							nextOffset = imtmdbx::QUINT64_MAX;

							continue;
						}
					}
				}
			}

			if (changeOffset) {
				maxOffset = std::max(maxOffset, offset);
				minOffset = std::min(minOffset, offset);
			}
		}

		return true;
	};

	if (!processOffsets(m_maskList)){
		return false;
	}
	if (!processOffsets(m_maskListInv)){
		return false;
	}

	/*******************conditions***************************/

	if (isLast){
		return false;
	}

	if (m_operationType == OT_AND) {
		if (maxOffset < activeOffset){
			qCritical() << "CMaskContainer::GetActiveItem: maxOffset < activeOffset";
			I_CRITICAL();

			return false;
		}

		activeOffset = maxOffset;
	}
	else if (m_operationType == OT_OR) {
		if (minOffset == imtmdbx::QUINT64_MAX){
			return false;
		}

		if (minOffset < activeOffset){
			qCritical() << "CMaskContainer::GetActiveItem: minOffset < activeOffset";
			I_CRITICAL();

			return false;
		}

		activeOffset = minOffset;
	}

	/*******************calculation***************************/

	auto processItems = [&](QList<QPair<IMask*,quint64>>& list, const bool& isInvertedList)
	{
		for (auto& [maskPtr, nextOffset] : list) {
			quint64 item = 0;
			if (nextOffset <= activeOffset){
				if (!maskPtr->GetItem(activeOffset, item)){
					item = 0;
				}
			}

			if (isInvertedList){
				item = ~item;
			}

			if (m_operationType == OT_AND){
				activeItem &= item;
			}
			else if (m_operationType == OT_OR){
				activeItem |= item;
			}
		}
	};

	processItems(m_maskList, false);
	processItems(m_maskListInv, true);

	return true;
}


// reimplemented (imtmdbx::IMask)

bool CMaskContainer::SetUnit(quint64 position, bool unit)
{
	return false;
}


bool CMaskContainer::SetItem(quint64 offset, quint64 item)
{
	return false;
}


bool CMaskContainer::GetUnit(quint64 position)
{
	if (m_maskList.isEmpty() && m_maskListInv.isEmpty()) {
		return false;
	}

	bool retUnit = m_operationType == OT_AND;

	auto processList = [&](QList<QPair<IMask*,quint64>>& targetList)
	{
		for (auto& [maskPtr, nextOffset] : targetList){
			bool currUnit = maskPtr->GetUnit(position);

			if (m_operationType == OT_AND){
				retUnit &= currUnit;
			}
			else if (m_operationType == OT_OR){
				retUnit |= currUnit;
			}
		}
	};

	processList(m_maskList);
	processList(m_maskListInv);

	return retUnit;
}


bool CMaskContainer::GetItem(quint64 offset, quint64& item)
{
	if (m_maskList.isEmpty() && m_maskListInv.isEmpty()) {
		return false;
	}

	quint64 retItem = m_operationType == OT_AND ? imtmdbx::QUINT64_MAX : 0;

	auto processList = [&](QList<QPair<IMask*,quint64>>& targetList) -> bool
	{
		for (auto& [maskPtr, nextOffset] : targetList){
			quint64 currItem = 0;
			if (!maskPtr->GetItem(offset, currItem)) {
				if (m_operationType == OT_AND){
					return false;
				}

				if (m_operationType == OT_OR){
					continue;
				}
			}

			if (m_operationType == OT_AND){
				retItem &= currItem;
			}
			else if (m_operationType == OT_OR){
				retItem |= currItem;
			}
		}

		return true;
	};

	if (!processList(m_maskList)) {
		return false;
	}
	if (!processList(m_maskListInv)) {
		return false;
	}

	item = retItem;

	return true;
}


bool CMaskContainer::GetNearestOffset(quint64& offset, quint64 startOffset)
{
	if (m_maskList.isEmpty() && m_maskListInv.isEmpty()) {
		return false;
	}

	bool isLast = true;
	quint64 retOffset = m_operationType == imtmdbx::CMaskContainer::OT_AND ? 0 : imtmdbx::QUINT64_MAX;

	auto processList = [&](QList<QPair<IMask*,quint64>>& targetList) -> bool
	{
		for (auto& [maskPtr, nextOffset] : targetList) {
			quint64 currOffset;
			if (maskPtr->GetNearestOffset(currOffset, startOffset)){
				nextOffset = currOffset;
				isLast = false;
			}
			else{
				nextOffset = imtmdbx::QUINT64_MAX;
				if (m_operationType == OT_AND) {
					return false;
				}

				if (m_operationType == OT_OR) {
					continue;
				}
			}

			if (m_operationType == OT_AND) {
				retOffset = std::max(retOffset, currOffset);
			}
			else if (m_operationType == OT_OR) {
				retOffset = std::min(retOffset, currOffset);
			}
		}

		return true;
	};

	if (!processList(m_maskList)) {
		return false;
	}
	if (!processList(m_maskListInv)) {
		return false;
	}

	offset = retOffset;

	return !isLast;
}


bool CMaskContainer::GetNextItemOffset(quint64& offset, quint64 startOffset)
{
	if(m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	bool isLast = true;
	quint64 retOffset = 0;

	auto processList = [&](QList<QPair<IMask*,quint64>>& targetList) -> bool
	{
		for (auto& [maskPtr, nextOffset] : targetList) {
			if (nextOffset > startOffset) {
				isLast = false;

				continue;
			}

			quint64 currOffset = 0;
			if (maskPtr->GetNextItemOffset(currOffset, startOffset)){
				isLast = false;
				nextOffset = currOffset;
			}
			else{
				nextOffset = imtmdbx::QUINT64_MAX;
				if (m_operationType == OT_AND){
					return false;
				}

				if (m_operationType == OT_OR){
					continue;
				}
			}

			if (m_operationType == OT_AND) {
				retOffset = std::max(retOffset, currOffset);
			}
			else if (m_operationType == OT_OR) {
				retOffset = std::min(retOffset, currOffset);
			}
		}

		return true;
	};

	if (!processList(m_maskList)){
		return false;
	}
	if (!processList(m_maskListInv)){
		return false;
	}

	offset = retOffset;
	if (isLast || offset == imtmdbx::QUINT64_MAX){
		return false;
	}

	return true;
}


bool CMaskContainer::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
	if (m_maskList.isEmpty() && m_maskListInv.isEmpty()){
		return false;
	}

	quint64 retOffset = 0;

	auto processList = [&](QList<QPair<IMask*,quint64>>& targetList) -> bool
	{
		for (auto& [maskPtr, nextOffset] : targetList){
			quint64 currOffset = 0;
			if (!maskPtr->GetPreviosItemOffset(currOffset, startOffset)) {
				if (m_operationType == OT_AND){
					return false;
				}

				if (m_operationType == OT_OR){
					continue;
				}
			}

			if (m_operationType == OT_AND){
				retOffset = std::min(retOffset, currOffset);
			}
			else if (m_operationType == OT_OR){
				retOffset = std::max(retOffset, currOffset);
			}
		}

		return true;
	};

	if (!processList(m_maskList)){
		return false;
	}
	if (!processList(m_maskListInv)){
		return false;
	}

	offset = retOffset;

	return true;
}


// reimplemented (imtmdbx::IMaskContainer)

void CMaskContainer::AddMask(IMask* mask, bool isInversion)
{
	QList<QPair<IMask*,quint64>>& targetList = isInversion ? m_maskListInv : m_maskList;

	targetList.append(QPair<IMask*,quint64>(mask, 0));
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
	QList<QPair<IMask*,quint64>>& targetList = isInversion ? m_maskListInv : m_maskList;

	return RemoveMask(targetList.size() - 1, 1, isInversion);
}


bool CMaskContainer::RemoveMask(int index, int n, bool isInversion)
{
	QList<QPair<IMask*,quint64>>& targetList = isInversion ? m_maskListInv : m_maskList;
	const bool canRemove = !targetList.isEmpty() && (index + n - 1) < m_maskList.length();
	if(canRemove){
		m_maskList.remove(index, n);
		m_maskList.squeeze();
	}

	return canRemove;
}


int CMaskContainer::GetMaskCount(bool isInversion)
{
	return isInversion ? m_maskListInv.length() : m_maskList.length();
}


void CMaskContainer::SetDocumentTable(IDocumentTable* documentTable)
{
	m_documentTable = documentTable;
}


quint64 CMaskContainer::GetUnitCount()
{
	quint64 unitCount	 = 0;
	quint64 activeOffset = 0;
	quint64 activeItem	 = imtmdbx::QUINT64_MAX;
	bool isStart = true;

	while(GetActiveItem(activeOffset, activeItem, isStart)){
		isStart = false;
		unitCount += imtmdbx::BitUtils::BitCount(activeItem);

		activeItem = imtmdbx::QUINT64_MAX;
	}

	return unitCount;
}


QList<quint64> CMaskContainer::GetUnitPositions(quint64 offset, quint64 limit)
{
	QList<quint64> list;
	quint64 activeOffset = 0;
	quint64 unitCount	 = 0;
	quint64 activeItem	 = imtmdbx::QUINT64_MAX;
	bool isStart = true;

	while(GetActiveItem(activeOffset, activeItem, isStart)){
		isStart = false;

		for(int i = 0; i < 64; i++){
			bool ok = (activeItem >> i) & static_cast<quint64>(1); // check activeItem
			if(!ok){
				continue;
			}

			unitCount++;
			if (unitCount > offset){
				int position = activeOffset * 64 + i;
				list.append(position);
				if(static_cast<quint64>(list.length()) >= limit){
					return list;
				}
			}
		}

		activeItem = imtmdbx::QUINT64_MAX;
	}

	return list;
}


QList<QByteArray> CMaskContainer::GetDocuments(quint64 offset, quint64 limit)
{
	if (m_documentTable == nullptr){
		return QList<QByteArray>();
	}

	QList<QByteArray> list;
	quint64 activeOffset = 0;
	quint64 unitCount	 = 0;
	quint64 activeItem	 = imtmdbx::QUINT64_MAX;
	bool isStart = true;

	while(GetActiveItem(activeOffset, activeItem, isStart)){
		isStart = false;

		for(int i = 0; i < 64; i++){
			bool ok = (activeItem >> i) & static_cast<quint64>(1); // check activeItem
			if(!ok){
				continue;
			}

			unitCount++;
			if (unitCount > offset){
				int position = activeOffset * 64 + i;
				list.append(m_documentTable->GetDocument(position));
				if(static_cast<quint64>(list.length()) >= limit){
					return list;
				}
			}
		}

		activeItem = imtmdbx::QUINT64_MAX;
	}

	return list;
}


} // namespace imtmdbx


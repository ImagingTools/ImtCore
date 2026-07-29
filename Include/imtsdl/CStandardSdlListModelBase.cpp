// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CStandardSdlListModelBase.h>


// Fires at the call site, so Q_FUNC_INFO correctly names the method that was invoked on the base instance.
#define ASSERT_NOT_BASE_INSTANCE() \
	Q_ASSERT_X(false, Q_FUNC_INFO, "called on base instance -- use a concrete SDL list model subclass")


namespace imtsdl
{


CStandardSdlListModelBase::CStandardSdlListModelBase(QObject* parent): QAbstractListModel(parent)
{
}


QObject* CStandardSdlListModelBase::getOwner() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return nullptr;
}


void CStandardSdlListModelBase::setOwner(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


int CStandardSdlListModelBase::count() const
{
	ASSERT_NOT_BASE_INSTANCE();
}


// --- default stubs (overridden in TSdlAbstractListModel / TListModelBase) ---

void CStandardSdlListModelBase::ClearCache()
{
	ASSERT_NOT_BASE_INSTANCE();
}


QVariant CStandardSdlListModelBase::GetOrCreateCachedObject(int /*index*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


QVariantMap CStandardSdlListModelBase::get(int /*row*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


QVariant CStandardSdlListModelBase::getData(const QString& /*nameId*/, int /*index*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


void CStandardSdlListModelBase::remove(int /*index*/)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::clear()
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::addElement(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::appendElement(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::insertElement(int, QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::removeElement(int)
{
	ASSERT_NOT_BASE_INSTANCE();
}


int CStandardSdlListModelBase::getItemsCount() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return -1;
}


bool CStandardSdlListModelBase::containsKey(const QString&, int) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


void CStandardSdlListModelBase::setProperty(int, const QString&, const QVariant&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


bool CStandardSdlListModelBase::swapItems(int, int)
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


QString CStandardSdlListModelBase::toJson() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return QStringLiteral("[]");
}


void CStandardSdlListModelBase::fromJSON(const QString&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CStandardSdlListModelBase::createFromJson(const QString&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


bool CStandardSdlListModelBase::isEqualWithModel(imtsdl::CStandardSdlListModelBase* /*model*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


QObject* CStandardSdlListModelBase::copyMe() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return nullptr;
}


int CStandardSdlListModelBase::rowCount(const QModelIndex& /*parent*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return 0;
}


QVariant CStandardSdlListModelBase::data(const QModelIndex& /*index*/, int /*role*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


} // namespace imtsdl

#undef ASSERT_NOT_BASE_INSTANCE

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdandardSdlListModelBase.h>


// Fires at the call site, so Q_FUNC_INFO correctly names the method that was invoked on the base instance.
#define ASSERT_NOT_BASE_INSTANCE() \
	Q_ASSERT_X(false, Q_FUNC_INFO, "called on base instance -- use a concrete SDL list model subclass")


namespace imtsdl
{


CSdandardSdlListModelBase::CSdandardSdlListModelBase(QObject* parent): QAbstractListModel(parent)
{
}


QObject* CSdandardSdlListModelBase::getOwner() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return nullptr;
}


void CSdandardSdlListModelBase::setOwner(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


int CSdandardSdlListModelBase::count() const
{
	ASSERT_NOT_BASE_INSTANCE();
}


// --- default stubs (overridden in TSdlAbstractListModel / TListModelBase) ---

void CSdandardSdlListModelBase::ClearCache()
{
	ASSERT_NOT_BASE_INSTANCE();
}


QVariant CSdandardSdlListModelBase::GetOrCreateCachedObject(int /*index*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


QVariantMap CSdandardSdlListModelBase::get(int /*row*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


QVariant CSdandardSdlListModelBase::getData(const QString& /*nameId*/, int /*index*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


void CSdandardSdlListModelBase::remove(int /*index*/)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::clear()
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::addElement(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::appendElement(QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::insertElement(int, QObject*)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::removeElement(int)
{
	ASSERT_NOT_BASE_INSTANCE();
}


int CSdandardSdlListModelBase::getItemsCount() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return -1;
}


bool CSdandardSdlListModelBase::containsKey(const QString&, int) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


void CSdandardSdlListModelBase::setProperty(int, const QString&, const QVariant&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


bool CSdandardSdlListModelBase::swapItems(int, int)
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


QString CSdandardSdlListModelBase::toJson() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return QStringLiteral("[]");
}


void CSdandardSdlListModelBase::fromJSON(const QString&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


void CSdandardSdlListModelBase::createFromJson(const QString&)
{
	ASSERT_NOT_BASE_INSTANCE();
}


bool CSdandardSdlListModelBase::isEqualWithModel(imtsdl::CSdandardSdlListModelBase* /*model*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return false;
}


QObject* CSdandardSdlListModelBase::copyMe() const
{
	ASSERT_NOT_BASE_INSTANCE();
	return nullptr;
}


int CSdandardSdlListModelBase::rowCount(const QModelIndex& /*parent*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return 0;
}


QVariant CSdandardSdlListModelBase::data(const QModelIndex& /*index*/, int /*role*/) const
{
	ASSERT_NOT_BASE_INSTANCE();
	return {};
}


} // namespace imtsdl

#undef ASSERT_NOT_BASE_INSTANCE

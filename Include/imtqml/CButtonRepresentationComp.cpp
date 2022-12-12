#include <imtqml/CButtonRepresentationComp.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

imtbase::CTreeItemModel* CButtonRepresentationComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{

	return nullptr;
}


} // namespace imtqml



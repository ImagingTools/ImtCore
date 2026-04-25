import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

/*!
	\qmltype FilterableSelectGqlDataProvider
	\inqmlmodule imtguigql
	\brief GQL-based data provider for FilterableSelectPopup using SDL structures.

	Extends CollectionDataProvider with the GetSelectableItems GQL query.
	Provides server-side text filtering and offset-based pagination
	using the FilterableSelect SDL schema.

	Usage:
	\code
	FilterableSelectGqlDataProvider {
		commandId: "GetSelectableItems"
		fields: ["id", "name", "description"]
		textFilteringInfoIds: ["name"]
	}
	\endcode
*/
CollectionDataProvider {
	id: root

	commandId: "GetSelectableItems"
	fields: ["id", "name"]
}

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0

/*!
	\qmltype TagSelectDataProvider
	\inqmlmodule imttaggui
	\brief Pages of the tag catalog with server-side text filtering.

	The single data source of every tag list: the tag picker, the tag filter and the
	Tags page. Items carry \c color; \c isSystemTag() and \c getUsageCount() read the
	rest of the tag.
*/
FilterableSelectGqlDataProvider {
	id: tagSelectDataProviderRoot

	collectionId: "Tags"
	multiSelect: true

	function getParameterValue(item, parameterId){
		if (!item || !item.parameters){
			return ""
		}

		for (var i = 0; i < item.parameters.length; i++){
			if (String(item.parameters[i].id) === parameterId){
				return String(item.parameters[i].value)
			}
		}

		return ""
	}

	function isSystemTag(item){
		return tagSelectDataProviderRoot.getParameterValue(item, "IsSystem") === "true"
	}

	function getUsageCount(item){
		var value = tagSelectDataProviderRoot.getParameterValue(item, "UsageCount")

		return value !== "" ? Number(value) : 0
	}

	function getColor(item){
		return item && item.color ? String(item.color) : "ededed"
	}
}

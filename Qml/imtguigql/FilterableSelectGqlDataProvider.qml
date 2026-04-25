import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtbaseFilterableSelectSdl 1.0

FilterableSelectDataProvider {
	id: root

	// HTTP headers
	function getHeaders(){
		return {}
	}

	property GetSelectableItemsInput getSelectableItemsInput: GetSelectableItemsInput {}
	property GqlSdlRequestSender getSelectableItemsRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseFilterableSelectSdlCommandIds.s_getSelectableItems
		sdlObjectComp: Component {
			GetSelectableItemsPayload {
				onFinished: {
					root.listObjectsReceived(m_items)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	function getSelectableItems(count, offset, filter){
		getSelectableItemsInput.m_collectionId = root.collectionId

		var viewParams = getSelectableItemsInput.m_viewParams
		viewParams.m_offset = offset || 0
		viewParams.m_count = count || 20

		if (filter){
			viewParams.m_filterModel = filter
		}
		else {
			viewParams.m_filterModel = null
		}

		getSelectableItemsRequest.send(getSelectableItemsInput)
	}
}

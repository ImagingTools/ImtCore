import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtbaseSettingsSdl 1.0

StyleBase {
	id: styleContainer;
	
	function getDesignScheme(theme){
		getStyleDataInput.m_schemeId = theme
		getStyleRequest.send(getStyleDataInput)
	}
	
	GetStyleDataInput {
		id: getStyleDataInput
	}
	
	TreeItemModel {
		id: styleModel
	}
	
	GqlSdlRequestSender {
		id: getStyleRequest
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_getStyleData
		
		sdlObjectComp: Component {
			StyleData {
				onFinished: {
					styleModel.clear()
					let sourceModel = styleModel.addTreeModel("source")
					sourceModel.createFromJson(m_data)
					
					styleContainer.parseStyleTheme(styleModel);
				}
			}
		}
	}
}



import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

import controlsgalleryParamsSetTestSdl 1.0


Rectangle {
	id: sdlTestPage;

	anchors.fill: parent;
	clip: true;

	color: Style.baseColor

	property ParamsSet paramSet: ParamsSet{}

	Row{
		anchors.centerIn: parent;
		spacing: 100
		height: 40
		Button{
			text: "Get Param Set"

			onClicked: {
				getParamSet.send()
			}
		}

		Button{
			text: "Send Param Set"

			onClicked: {
				sendParamSet.send(sdlTestPage.paramSet)
			}
		}

	}

	GqlSdlRequestSender {
		id: getParamSet;

		gqlCommandId: ControlsgalleryParamsSetTestSdlCommandIds.s_getParamsSet

		sdlObjectComp: Component {
			ParamsSet {
				onFinished: {
					sdlTestPage.paramSet = this
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: sendParamSet;

		gqlCommandId: ControlsgalleryParamsSetTestSdlCommandIds.s_setParamsSet
		// inputObjectComp: Component {
		// 	ParamsSet{
		// 	}
		// }

		sdlObjectComp: Component {
			SetParamsSetResult {
				onFinished: {
					console.log("RESULT::", m_result)
				}
			}
		}
	}


}


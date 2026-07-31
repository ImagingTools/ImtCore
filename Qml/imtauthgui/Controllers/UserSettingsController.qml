import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root

	signal settingsReceived()
	signal settingsReceiveFailed()
	signal settingsSaved()
	signal settingsSaveFailed()
	
	function getSettings(){
		getSettingsQuery.send()
	}
	
	function saveParam(){
		setSettingsQuery.send()
	}

	property GqlSdlRequestSender getSettingsQuery: GqlSdlRequestSender {
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_getSettings
		inputObjectComp: Component {
			GetSettingsInput {
				m_userId: AuthorizationController.getUserId()
			}
		}

		sdlObjectComp: Component {
			ParamsSet {
				onFinished: {
					root.createParamFromJson(this.toJson())
					root.settingsReceived()
				}
			}
		}
	}
	
	property GqlSdlRequestSender setSettingsQuery: GqlSdlRequestSender {
		requestType: 1
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_setSettings
		inputObjectComp: Component {
			SetSettingsInput {
				m_userId: AuthorizationController.getUserId()
				m_settings: root.paramModel.toJson()
			}
		}

		sdlObjectComp: Component {
			SetSettingsPayload {
				onFinished: {
					if (m_ok){
						root.settingsSaved()
						root.paramModelCreated()
					}
					else{
						root.settingsSaveFailed()
					}
				}
			}
		}
	}
}

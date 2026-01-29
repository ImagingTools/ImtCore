import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0


/**
	GraphQL-based SettingsController.
	
	This component extends SettingsController with GQL-specific implementations.
	It overrides the patTokenParamEditorComp to inject GqlBasedPatTokenController,
	keeping the GQL dependency isolated in the imtguigql module.
*/
SettingsController {
	id: root
	
	// Override patTokenParamEditorComp to inject GqlBasedPatTokenController
	patTokenParamEditorComp: Component { 
		PatTokenParamEditor {
			patTokenController: GqlBasedPatTokenController {}
		}
	}
}

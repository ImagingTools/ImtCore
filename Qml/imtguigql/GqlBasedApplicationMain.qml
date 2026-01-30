import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0


/**
	GraphQL-based ApplicationMain.
	
	This component extends ApplicationMain with GQL-specific configurations.
	It provides GQL-specific implementations for components that need protocol-specific logic,
	keeping all GQL dependencies isolated in the imtguigql module.
	
	This approach allows applications to use either the base ApplicationMain (protocol-agnostic)
	or GqlBasedApplicationMain (with GQL implementations) without modifying the base components.
*/
ApplicationMain {
	id: root
	
	// Override ThumbnailDecorator to provide GQL-specific SettingsController
	Component.onCompleted: {
		// Configure ThumbnailDecorator with GQL-specific SettingsController
		thumbnailDecorator.settingsController.patTokenParamEditorComp = Qt.createQmlObject(
			'import QtQuick 2.12; import imtgui 1.0; import imtguigql 1.0; import imtauthgui 1.0; Component { PatTokenParamEditor { patTokenController: GqlBasedPatTokenController {} } }',
			thumbnailDecorator.settingsController,
			"gqlPatTokenEditorComp"
		)
	}
}

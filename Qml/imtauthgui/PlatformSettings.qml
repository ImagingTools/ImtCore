import QtQuick 2.12
import imtauthgui 1.0

// Platform-aware Settings component
// - Uses LocalStorage singleton which handles web/desktop differences
QtObject {
	id: root
	
	property string category: "DefaultCategory"
	
	// Properties for login settings
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	
	// Load settings from storage on component creation
	Component.onCompleted: {
		loadFromStorage();
	}
	
	// Watch for changes and save
	onRememberMeChanged: saveToStorage()
	onLastUserChanged: saveToStorage()
	onStoredRefreshTokenChanged: saveToStorage()
	
	function loadFromStorage() {
		let rememberMeStr = LocalStorage.getItem(category + "_rememberMe");
		root.rememberMe = (rememberMeStr === "true");
		root.lastUser = LocalStorage.getItem(category + "_lastUser") || "";
		root.storedRefreshToken = LocalStorage.getItem(category + "_storedRefreshToken") || "";
	}
	
	function saveToStorage() {
		LocalStorage.setItem(category + "_rememberMe", root.rememberMe ? "true" : "false");
		LocalStorage.setItem(category + "_lastUser", root.lastUser);
		LocalStorage.setItem(category + "_storedRefreshToken", root.storedRefreshToken);
	}
	
	function clear() {
		root.rememberMe = false;
		root.lastUser = "";
		root.storedRefreshToken = "";
		
		LocalStorage.removeItem(category + "_rememberMe");
		LocalStorage.removeItem(category + "_lastUser");
		LocalStorage.removeItem(category + "_storedRefreshToken");
	}
}

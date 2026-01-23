import QtQuick 2.12
import Qt.labs.settings 1.0

// Platform-aware Settings component
// - For desktop: Uses Qt.labs.settings for persistent storage
// - For web: Uses localStorage API
QtObject {
	id: root
	
	property string category: "DefaultCategory"
	
	// Properties for login settings
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	
	// Desktop settings (only used on non-web platforms)
	property Settings desktopSettings: Settings {
		category: root.category
		
		property bool rememberMe: false
		property string lastUser: ""
		property string storedRefreshToken: ""
	}
	
	// Load settings from appropriate storage on component creation
	Component.onCompleted: {
		if (Qt.platform.os === "web") {
			// Load from localStorage
			loadFromLocalStorage();
		} else {
			// Load from Qt.labs.settings (automatic via property binding)
			root.rememberMe = desktopSettings.rememberMe;
			root.lastUser = desktopSettings.lastUser;
			root.storedRefreshToken = desktopSettings.storedRefreshToken;
		}
	}
	
	// Watch for changes and save
	onRememberMeChanged: {
		if (Qt.platform.os === "web") {
			saveToLocalStorage();
		} else {
			desktopSettings.rememberMe = root.rememberMe;
		}
	}
	
	onLastUserChanged: {
		if (Qt.platform.os === "web") {
			saveToLocalStorage();
		} else {
			desktopSettings.lastUser = root.lastUser;
		}
	}
	
	onStoredRefreshTokenChanged: {
		if (Qt.platform.os === "web") {
			saveToLocalStorage();
		} else {
			desktopSettings.storedRefreshToken = root.storedRefreshToken;
		}
	}
	
	function loadFromLocalStorage() {
		let rememberMeStr = localStorage.getItem(category + "_rememberMe");
		root.rememberMe = (rememberMeStr === "true");
		root.lastUser = localStorage.getItem(category + "_lastUser") || "";
		root.storedRefreshToken = localStorage.getItem(category + "_storedRefreshToken") || "";
	}
	
	function saveToLocalStorage() {
		localStorage.setItem(category + "_rememberMe", root.rememberMe ? "true" : "false");
		localStorage.setItem(category + "_lastUser", root.lastUser);
		localStorage.setItem(category + "_storedRefreshToken", root.storedRefreshToken);
	}
	
	function clear() {
		root.rememberMe = false;
		root.lastUser = "";
		root.storedRefreshToken = "";
		
		if (Qt.platform.os === "web") {
			localStorage.removeItem(category + "_rememberMe");
			localStorage.removeItem(category + "_lastUser");
			localStorage.removeItem(category + "_storedRefreshToken");
		}
		// Desktop settings will be cleared automatically via property binding
	}
}

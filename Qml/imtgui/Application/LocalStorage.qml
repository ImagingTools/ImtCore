pragma Singleton

import QtQuick 2.12
import Qt.labs.settings 1.0

// Singleton LocalStorage - provides localStorage-like API for both web and desktop
// - Web: Uses browser's localStorage directly
// - Desktop: Uses Qt.labs.settings as backend
QtObject {
	id: root
	
	// Desktop-only settings backend
	property Settings desktopStorage: Settings {
		category: "LocalStorage"
	}
	
	// Get item from storage
	function getItem(key) {
		if (Qt.platform.os === "web") {
			return localStorage.getItem(key);
		} else {
			// For desktop, read from Settings
			return desktopStorage.value(key, null);
		}
	}
	
	// Set item in storage
	function setItem(key, value) {
		if (Qt.platform.os === "web") {
			localStorage.setItem(key, value);
		} else {
			// For desktop, write to Settings
			desktopStorage.setValue(key, value);
		}
	}
	
	// Remove item from storage
	function removeItem(key) {
		if (Qt.platform.os === "web") {
			localStorage.removeItem(key);
		} else {
			// For desktop, remove from Settings
			desktopStorage.remove(key);
		}
	}
	
	// Clear all items (use with caution)
	function clear() {
		if (Qt.platform.os === "web") {
			localStorage.clear();
		} else {
			// For desktop, clear all Settings
			desktopStorage.clear();
		}
	}
	
	// Check if key exists
	function hasItem(key) {
		if (Qt.platform.os === "web") {
			return localStorage.getItem(key) !== null;
		} else {
			return desktopStorage.value(key, null) !== null;
		}
	}
}

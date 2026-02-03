import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root
	
	property ListModel tokens: ListModel {}
	
	signal modelChanged(var changeSet)
	
	function addToken(name, token, description) {
		tokens.append({
			"name": name,
			"token": token,
			"description": description || "",
			"createdDate": new Date().toISOString()
		})
		modelChanged([{name: "tokens"}])
	}
	
	function removeToken(index) {
		if (index >= 0 && index < tokens.count) {
			tokens.remove(index)
			modelChanged([{name: "tokens"}])
		}
	}
	
	function updateToken(index, name, token, description) {
		if (index >= 0 && index < tokens.count) {
			tokens.set(index, {
				"name": name,
				"token": token,
				"description": description || "",
				"createdDate": tokens.get(index).createdDate
			})
			modelChanged([{name: "tokens"}])
		}
	}
	
	function getToken(index) {
		if (index >= 0 && index < tokens.count) {
			return tokens.get(index)
		}
		return null
	}
	
	function toJson() {
		let tokensArray = []
		for (let i = 0; i < tokens.count; i++) {
			let token = tokens.get(i)
			tokensArray.push({
				name: token.name,
				token: token.token,
				description: token.description,
				createdDate: token.createdDate
			})
		}
		return JSON.stringify({tokens: tokensArray})
	}
	
	function createFromJson(json) {
		try {
			tokens.clear()
			let data = JSON.parse(json)
			if (data.tokens && Array.isArray(data.tokens)) {
				for (let i = 0; i < data.tokens.length; i++) {
					let token = data.tokens[i]
					tokens.append({
						name: token.name || "",
						token: token.token || "",
						description: token.description || "",
						createdDate: token.createdDate || new Date().toISOString()
					})
				}
			}
			return true
		} catch (e) {
			console.error("Failed to parse PAT tokens JSON:", e)
			return false
		}
	}
}

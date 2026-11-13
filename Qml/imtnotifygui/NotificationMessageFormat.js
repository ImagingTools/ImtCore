// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
.pragma library

// The messages of the notification types: the subset of ICU MessageFormat that imtnotify::NotificationMessageFormat
// checks on the server. {id}, {id, plural, =N {..} one {..} few {..} many {..} other {..}} with # for the number,
// {id, select, value {..} other {..}}; '' is an apostrophe, '{', '}' and '#' after an apostrophe start a quoted text.

var __cache = {}

function __isWordCharacter(character){
	return /[A-Za-z0-9_]/.test(character)
}

function __parse(message){
	var state = {"message": message, "position": 0}
	var nodes = __parseText(state, false, false)
	if (nodes === null || state.position < message.length){
		return null
	}

	return nodes
}

function __skipSpaces(state){
	while (state.position < state.message.length && /\s/.test(state.message[state.position])){
		state.position++
	}
}

function __readWord(state){
	var start = state.position
	while (state.position < state.message.length && __isWordCharacter(state.message[state.position])){
		state.position++
	}

	return state.message.substring(start, state.position)
}

// A text up to the end or, nested, up to the closing brace of its branch; null if it is not valid
function __parseText(state, isNested, isPlural){
	var nodes = []
	var text = ""
	while (state.position < state.message.length){
		var character = state.message[state.position]
		if (character === "'"){
			var next = state.message[state.position + 1]
			if (next === "'"){
				text += "'"
				state.position += 2
			}
			else if (next === "{" || next === "}" || next === "#"){
				state.position++
				while (state.position < state.message.length){
					if (state.message[state.position] === "'"){
						if (state.message[state.position + 1] === "'"){
							text += "'"
							state.position += 2

							continue
						}

						state.position++
						break
					}

					text += state.message[state.position]
					state.position++
				}
			}
			else{
				text += character
				state.position++
			}
		}
		else if (character === "{"){
			if (text !== ""){
				nodes.push({"type": "text", "value": text})
				text = ""
			}

			state.position++
			var argument = __parseArgument(state)
			if (argument === null){
				return null
			}

			nodes.push(argument)
		}
		else if (character === "}"){
			if (!isNested){
				return null
			}

			break
		}
		else if (character === "#" && isPlural){
			if (text !== ""){
				nodes.push({"type": "text", "value": text})
				text = ""
			}

			nodes.push({"type": "number"})
			state.position++
		}
		else{
			text += character
			state.position++
		}
	}

	if (isNested && state.position >= state.message.length){
		return null
	}

	if (text !== ""){
		nodes.push({"type": "text", "value": text})
	}

	return nodes
}

function __parseArgument(state){
	__skipSpaces(state)
	var id = __readWord(state)
	if (id === ""){
		return null
	}

	__skipSpaces(state)
	if (state.message[state.position] === "}"){
		state.position++

		return {"type": "argument", "id": id}
	}

	if (state.message[state.position] !== ","){
		return null
	}

	state.position++
	__skipSpaces(state)
	var argumentType = __readWord(state)
	if (argumentType !== "plural" && argumentType !== "select"){
		return null
	}

	__skipSpaces(state)
	if (state.message[state.position] !== ","){
		return null
	}

	state.position++

	var branches = {}
	for (;;){
		__skipSpaces(state)
		if (state.position >= state.message.length){
			return null
		}

		if (state.message[state.position] === "}"){
			state.position++
			break
		}

		var selector = ""
		if (state.message[state.position] === "="){
			state.position++
			selector = "=" + __readWord(state)
		}
		else{
			selector = __readWord(state)
		}

		if (selector === "" || selector === "="){
			return null
		}

		__skipSpaces(state)
		if (state.message[state.position] !== "{"){
			return null
		}

		state.position++
		var branch = __parseText(state, true, argumentType === "plural")
		if (branch === null){
			return null
		}

		state.position++
		branches[selector] = branch
	}

	if (!branches["other"]){
		return null
	}

	return {"type": argumentType, "id": id, "branches": branches}
}

// The plural category of a number in a language, as in the CLDR rules of the languages of the products
function pluralCategory(number, languageId){
	var language = String(languageId).toLowerCase().split(/[_-]/)[0]
	var isInteger = Math.floor(number) === number
	if (language === "ru" || language === "uk" || language === "be"){
		if (!isInteger){
			return "other"
		}

		var mod10 = number % 10
		var mod100 = number % 100
		if (mod10 === 1 && mod100 !== 11){
			return "one"
		}

		if (mod10 >= 2 && mod10 <= 4 && (mod100 < 12 || mod100 > 14)){
			return "few"
		}

		return "many"
	}

	return number === 1 ? "one" : "other"
}

function __formatValue(argument){
	if (argument.kind === "DateTime"){
		return new Date(argument.value).toLocaleString()
	}

	if (argument.kind === "Number"){
		return String(Number(argument.value))
	}

	return argument.value
}

// null if an argument is missing: the client then shows the notification without its message
function __render(nodes, argumentsMap, languageId, number){
	var result = ""
	for (var i = 0; i < nodes.length; i++){
		var node = nodes[i]
		if (node.type === "text"){
			result += node.value

			continue
		}

		if (node.type === "number"){
			result += String(number)

			continue
		}

		var argument = argumentsMap[node.id]
		if (!argument || argument.value === undefined || argument.value === null || String(argument.value) === ""){
			return null
		}

		var part = null
		if (node.type === "argument"){
			part = __formatValue(argument)
		}
		else if (node.type === "plural"){
			var value = Number(argument.value)
			var branch = node.branches["=" + String(value)]
			if (!branch){
				branch = node.branches[pluralCategory(value, languageId)]
			}

			if (!branch){
				branch = node.branches["other"]
			}

			part = __render(branch, argumentsMap, languageId, value)
		}
		else{
			var selected = node.branches[String(argument.value)]
			part = __render(selected ? selected : node.branches["other"], argumentsMap, languageId, number)
		}

		if (part === null){
			return null
		}

		result += part
	}

	return result
}

// The message with the arguments, {id: {"kind", "value"}}; null if the message is not valid or an argument is missing
function format(message, argumentsMap, languageId){
	if (!message){
		return null
	}

	var nodes = __cache[message]
	if (nodes === undefined){
		nodes = __parse(message)
		__cache[message] = nodes
	}

	if (nodes === null){
		return null
	}

	return __render(nodes, argumentsMap, languageId, 0)
}

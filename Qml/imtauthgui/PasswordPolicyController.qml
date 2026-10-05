import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

/**
 * PasswordPolicyController
 *
 * Abstract contract for the password policy: the rules as configured on the server,
 * the pre-submit check and the texts derived from them. Holds no transport —
 * a concrete implementation (e.g. GqlBasedPasswordPolicyProvider) overrides load()
 * and feeds the result back through applyPolicy().
 *
 * The check here is a convenience only — the server-side check stays authoritative,
 * and the password blocklist is never published, so it cannot be mirrored.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root;

	property bool loaded: false;

	property int minLength: 0;
	property int maxLength: 0;
	property bool requireLowercase: false;
	property bool requireUppercase: false;
	property bool requireDigit: false;
	property bool requireSpecialChar: false;
	property bool rejectLoginAsPassword: false;
	property bool blocklistUsed: false;
	property int historyDepth: 0;

	signal policyReceived();

	// Requests the policy from the server. Concrete implementations override this.
	function load(){}

	function applyPolicy(policy){
		if (!policy){
			return;
		}

		root.minLength = policy.minLength;
		root.maxLength = policy.maxLength;
		root.requireLowercase = policy.requireLowercase;
		root.requireUppercase = policy.requireUppercase;
		root.requireDigit = policy.requireDigit;
		root.requireSpecialChar = policy.requireSpecialChar;
		root.rejectLoginAsPassword = policy.rejectLoginAsPassword;
		root.blocklistUsed = policy.blocklistUsed;
		root.historyDepth = policy.historyDepth;
		root.loaded = true;

		root.policyReceived();
	}

	function validate(login, password){
		let violatedRules = [];

		if (password === ""){
			return violatedRules;
		}

		if (root.minLength > 0 && password.length < root.minLength){
			violatedRules.push("MinLength");
		}

		if (root.maxLength > 0 && password.length > root.maxLength){
			violatedRules.push("MaxLength");
		}

		let hasLowercase = false;
		let hasUppercase = false;
		let hasDigit = false;
		let hasSpecialChar = false;

		for (let index = 0; index < password.length; ++index){
			let character = password.charAt(index);
			let isLetter = character.toLowerCase() !== character.toUpperCase();

			if (isLetter){
				if (character === character.toLowerCase()){
					hasLowercase = true;
				}
				else{
					hasUppercase = true;
				}
			}
			else if (character >= "0" && character <= "9"){
				hasDigit = true;
			}
			else if (character.trim() !== ""){
				hasSpecialChar = true;
			}
		}

		if (root.requireLowercase && !hasLowercase){
			violatedRules.push("LowercaseRequired");
		}

		if (root.requireUppercase && !hasUppercase){
			violatedRules.push("UppercaseRequired");
		}

		if (root.requireDigit && !hasDigit){
			violatedRules.push("DigitRequired");
		}

		if (root.requireSpecialChar && !hasSpecialChar){
			violatedRules.push("SpecialCharRequired");
		}

		if (root.rejectLoginAsPassword && login !== "" && password.toLowerCase() === login.toLowerCase()){
			violatedRules.push("LoginAsPassword");
		}

		return violatedRules;
	}

	function describeRule(ruleId){
		switch (ruleId){
			case "MinLength": return qsTr("at least %1 characters").arg(root.minLength);
			case "MaxLength": return qsTr("at most %1 characters").arg(root.maxLength);
			case "LowercaseRequired": return qsTr("a lowercase letter");
			case "UppercaseRequired": return qsTr("an uppercase letter");
			case "DigitRequired": return qsTr("a digit");
			case "SpecialCharRequired": return qsTr("a special character");
			case "LoginAsPassword": return qsTr("a password different from the login");
			case "BlocklistedPassword": return qsTr("a password that is not commonly used");
			case "PasswordReused": return root.historyDepth > 0
						? qsTr("a password different from the last %1 ones").arg(root.historyDepth)
						: qsTr("a password that was not used before");
		}

		return "";
	}

	// Single sentence for a banner or a dialog, built from the rule IDs the server reported.
	function describeFailure(violatedRules, fallbackMessage){
		if (!violatedRules || violatedRules.length <= 0){
			return fallbackMessage;
		}

		switch (violatedRules[0]){
			case "InvalidCredentials": return qsTr("The current password is not correct.");
			case "MinPasswordAge": return qsTr("The password was changed too recently and cannot be changed again yet.");
			case "ExternalUser": return qsTr("This account is managed by an external system, its password cannot be changed here.");
			case "UnknownUser": return qsTr("This account no longer exists.");
			case "StorageError":
			case "InternalError":
			case "InvalidRequest": return qsTr("The password could not be changed. Please try again later.");
		}

		let requirements = [];
		for (let index = 0; index < violatedRules.length; ++index){
			let requirement = root.describeRule(violatedRules[index]);
			if (requirement !== ""){
				requirements.push(requirement);
			}
		}

		if (requirements.length <= 0){
			return fallbackMessage;
		}

		return qsTr("The password must contain %1.").arg(requirements.join(qsTr(", ")));
	}

	// Hint shown next to the input, before anything was typed.
	function requirementsText(){
		if (!root.loaded){
			return "";
		}

		let requirements = [];

		if (root.minLength > 0){
			requirements.push(root.describeRule("MinLength"));
		}

		if (root.requireLowercase){
			requirements.push(root.describeRule("LowercaseRequired"));
		}

		if (root.requireUppercase){
			requirements.push(root.describeRule("UppercaseRequired"));
		}

		if (root.requireDigit){
			requirements.push(root.describeRule("DigitRequired"));
		}

		if (root.requireSpecialChar){
			requirements.push(root.describeRule("SpecialCharRequired"));
		}

		if (requirements.length <= 0){
			return "";
		}

		return qsTr("The password must contain %1.").arg(requirements.join(qsTr(", ")));
	}
}

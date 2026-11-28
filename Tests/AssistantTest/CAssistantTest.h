// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the generic assistant core (imtassist): schema and registry validation,
	permission and tenant boundaries, prompt injection from tool data, confirmation
	of prepared changes including repetition and stale previews, and error and
	cancellation handling. All tests use the deterministic fake provider.
*/
class CAssistantTest: public QObject
{
	Q_OBJECT

private slots:
	// Schema and registries
	void testSchema_AcceptsValidObject();
	void testSchema_RejectsUnknownField();
	void testSchema_RejectsMissingRequiredField();
	void testSchema_RejectsWrongTypesAndValues();
	void testSchema_ValidatesNestedObjectLists();
	void testActionRegistry_RejectsInvalidRegistrations();
	void testUiRegistry_RejectsUnknownTypeAndInvalidData();

	// Enabling and configuration
	void testDisabled_NoProviderCall();
	void testUnconfiguredProvider_NoProviderCall();
	void testInvalidContext_Rejected();

	// Conversation flow
	void testClarification();
	void testEndToEnd_SearchSelectConfirm();
	void testReleasedContext_OnlyForCurrentRequest();
	void testUnknownAction_Rejected();
	void testInvalidParameters_Rejected();
	void testInvalidRepresentation_RejectedButResultShown();

	// Confirmation of changes
	void testNoConfirmation_NoChange();
	void testRejectedPreview_NoChange();
	void testExpiredPreview_NoChange();
	void testStalePreview_NoChange();
	void testRepeatedConfirmation_ChangesOnce();
	void testPermissionRevokedBeforeConfirmation_NoChange();

	// Principal and tenant boundaries
	void testOtherPrincipal_CannotUsePreview();
	void testOtherPrincipal_CannotUseConversation();
	void testSearch_RespectsTenant();

	// Prompt injection
	void testPromptInjection_ToolDataCannotConfirmOrEscalate();

	// Errors and cancellation
	void testProviderError_Visible();
	void testTimeout_Visible();
	void testInvalidProviderResponse_Visible();
	void testCancel_StopsTurn();
	void testTooManySteps_Visible();

	// Audit log
	void testAuditLog_ContainsNoContent();

	// Message format
	void testMessage_ToJson();

	// Standard representations and fake provider
	void testStandardRepresentations_SelectableList();
	void testFakeProvider_ResolvesContextPlaceholders();
	void testFakeProvider_ToolResultReply();
	void testEndToEnd_FakeProviderRules();
};



import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0


/**
 * TenantMembershipSubscriptionClient
 *
 * QML component that subscribes to tenant membership notifications
 * (invitations received, accepted, rejected) and ownership transfer.
 *
 * Usage:
 *   TenantMembershipSubscriptionClient {
 *       onInvitationReceived: function(data) { ... }
 *       onInvitationAccepted: function(data) { ... }
 *       onInvitationRejected: function(data) { ... }
 *       onOwnershipTransferred: function(data) { ... }
 *   }
 *
 * The data object contains: membershipId, userId, tenantId, tenantName, role
 */
SubscriptionClient {
	id: container;
	gqlCommandId: "OnMembershipNotification";

	signal invitationReceived(var notification);
	signal invitationAccepted(var notification);
	signal invitationRejected(var notification);
	signal ownershipTransferred(var notification);

	onMessageReceived: {
		if (!data) {
			return;
		}

		var notificationType = "";
		if (data.containsKey("notificationType")) {
			notificationType = data.getData("notificationType");
		}

		var notification = {
			"membershipId": data.containsKey("membershipId") ? data.getData("membershipId") : "",
			"userId": data.containsKey("userId") ? data.getData("userId") : "",
			"tenantId": data.containsKey("tenantId") ? data.getData("tenantId") : "",
			"tenantName": data.containsKey("tenantName") ? data.getData("tenantName") : "",
			"role": data.containsKey("role") ? data.getData("role") : ""
		};

		if (notificationType === "InvitationReceived" || notificationType === 0) {
			container.invitationReceived(notification);
		}
		else if (notificationType === "InvitationAccepted" || notificationType === 1) {
			container.invitationAccepted(notification);
		}
		else if (notificationType === "InvitationRejected" || notificationType === 2) {
			container.invitationRejected(notification);
		}
		else if (notificationType === "OwnershipTransferred" || notificationType === 3) {
			container.ownershipTransferred(notification);
		}
	}
}

// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


class CDocumentShareTest: public QObject
{
	Q_OBJECT

private slots:
	// Document share data
	void testShare_SerializeRoundTrip();
	void testShare_CopyFromAndClone();
	void testShare_ResetData();

	// Tenant filter: the cached collections key their selections by its serialization
	void testTenantFilter_DefaultsSelectOwnDocuments();
	void testTenantFilter_SerializeRoundTrip();
	void testTenantFilter_SerializationDiffersBySharing();
	void testTenantFilter_CopyFrom();

	// Relationship validity
	void testRelationshipValidity_OpenBounds();
	void testRelationshipValidity_Bounds();
};



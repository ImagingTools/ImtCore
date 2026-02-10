// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	\namespace imtlic
	\brief Namespace containing components and interfaces related to license management.
	
	\section conceptual_model Conceptual Model
	
	The imtlic module implements a comprehensive licensing system based on the following concepts:
	
	\subsection products Products
	A Product (IProductInfo, CProductInfo) represents a software or hardware product offered by the company.
	Products are described as a set of Features. Each product has:
	- A unique product ID
	- A human-readable name and description
	- A category classification
	- A collection of Features that define its capabilities
	
	\subsection features Features
	A Feature (IFeatureInfo, CFeatureInfo) represents an individual capability or functionality within a product.
	Features can:
	- Be optional or mandatory
	- Form hierarchies (parent-child relationships)
	- Have dependencies on other features
	- Be marked as permissions for access control
	
	\subsection license_definitions License Definitions
	A License Definition (ILicenseDefinition, CLicenseDefinition) is a container of Features that can be "unlocked"
	when a customer purchases a specific license. Each license definition:
	- Belongs to a specific Product
	- Contains a list of Features it enables
	- Can depend on other licenses (license dependencies)
	- Has a unique license ID for identification
	
	From a licensing perspective, a Product is essentially a container of all its available License Definitions,
	and each License Definition is a container of Features. Consequently, a Product is ultimately a container
	of all its Features.
	
	\subsection product_instances Product Instances
	When a Product is sold or installed, a Product Instance (IProductInstanceInfo, CProductInstanceInfo) is created.
	A Product Instance represents a specific installation and includes:
	- A unique instance ID (e.g., MAC address or hardware ID)
	- Customer information
	- Serial number and project assignment
	- A collection of activated License Instances
	
	\subsection license_instances License Instances
	A License Instance (ILicenseInstance, CLicenseInstance) is an activated license based on a License Definition.
	It extends the License Definition with:
	- An expiration date (or unlimited validity)
	- Instance-specific activation data
	
	\subsection product_licensing Product Licensing Info
	Product Licensing Info (IProductLicensingInfo, CProductLicensingInfo) provides access to all available
	licenses for a specific product. It serves as the complete catalog of licensing options for that product.
	
	\subsection workflow Typical Workflow
	1. Define a Product with its Features
	2. Create License Definitions that specify which Features each license unlocks
	3. When a product is sold, create a Product Instance
	4. Activate License Instances on the Product Instance based on purchased licenses
	5. The system validates available features based on active License Instances
	
	\ingroup LicenseManagement
*/
namespace imtlic
{


} // namespace imtlic



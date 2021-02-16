#pragma once


/**
\defgroup ImtCore ImagingTools Core SDK
\mainpage
\section Introduction
ImagingTools Core SDK is an extension framework for ACF.
The main goal of this framework is the implementation of components and interfaces using for realization of the business logic,
which can find a general application in every software product.
*/

/**
\defgroup Collection Collection
\section Motivation Motivation
One of the most important aspects in software development is the definition and implementation of the data layer.
A common task here is to group data. Every software product manages sets of objects.
The aim of the collection module is to standardize this management through a set of general interfaces and the associated implementations
\section Introduction Introduction
A collection is a container of objects. We do not know anything about the type of objects in the container.
The only thing we know about this container is that the objects that are located there can be clearly identified (e.g. via a UUID).
With regard to a collection, we therefore differentiate between two types of interfaces:
1. Access to the contents of a collection
2. Access to the information about the contents of a collection

The basic interface for access to information about a collection is imtbase::ICollectionInfo.
This simple interface allows querying all existing IDs of the elements in the collection.
If the IDs of elements are known, the properties (descriptions) of these elements can be accessed (via these IDs)
For a collection itself, we don't have a general interface.
Instead, there are interfaces for certain types of collections, the most important one is - imtbase::IObjectCollection.
This interface describes the collection of data objects, that is, objects that implement directly or indirectly the istd::IChangeable interface.
"Concrete" collection interfaces can offer their specialized extensions to the imtbase::ICollectionInfo interface.
There is a suitable imtbase::IObjectCollectionInfo interface for imtbase::IObjectCollection
*/


/**
\defgroup FileRepository File Repository
\section Motivation Motivation
In the document-oriented application you need persistence of the documents or another grouped application data. File-based document repository is a way to do it.
File collections are specializations of imtbase::IObjectCollection interfrace.
*/

/**
\defgroup LicenseManagement License Management
\section Motivation Motivation
In many applications it is desirable that the user only has limited access to the implemented software functions. A licensing mechanism that manages these access rights is often used for this purpose.
\section Concept
1. Definition of license data
2. Provide management of license collections
3. License check using (via binding more general interface iauth::IRightsProvider)
4. Low-level license key generation (Encryption)
5. Device persistence of license data.
6. File-based peristence of license data.
7. Diagnostics information about the license status in the application
*/



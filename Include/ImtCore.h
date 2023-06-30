#pragma once


/**
\defgroup ImtCore ImagingTools Core SDK
\mainpage
\section Introduction
ImagingTools Core SDK is an extension framework for ACF.
The main goal of this framework is the implementation of components and interfaces using for realization of the business logic,
which can find a general application in every software product.

\defgroup Collection Collection
\section Motivation Motivation
One of the most important aspects in the software development is the definition and implementation of the data layer.
A general task here is the grouping of data, since we can assume that any software product must manage sets of entities. We call such sets of entities - Collection.
The typical requirements for an entity collection are:
- Iterations over the collection
- Searching for entities based on one or more conditions
- Filtering of entities
- Reading and writing of entity data

The aim of the collection module is to standardize the working with object collections through a set of general interfaces and the associated implementations
\section Introduction Introduction
A collection is a container of items. We do not know anything about the structure or implementation detailes of the items in the container.
The only thing we know about this container is that the items, that are located there, can be clearly identified (e.g. via a UUID).
With regard to a collection, we therefore differentiate between two types of interfaces:
- Accessing to the items inside a collection
- Accessing to the information about the contents of a collection

The basic interface for access to information about a collection is imtbase::ICollectionInfo.
This simple interface allows querying all existing IDs of the elements in the collection.
If the IDs of elements are known, the properties (descriptions) of these elements can be accessed (via these IDs)

For a collection itself, we don't have a general interface.
Instead, there are interfaces for certain types of collections, the most important one is - imtbase::IObjectCollection.
This interface describes the collection of data objects, that is, objects that implement directly or indirectly the istd::IChangeable interface.
"Concrete" collection interfaces can offer their specialized extensions to the imtbase::ICollectionInfo interface.
There is a suitable imtbase::IObjectCollectionInfo interface for imtbase::IObjectCollection.

\section ObjectCollection Object Collection
As already said, for the collection of data objects we provide the interface imtbase::IObjectCollection. This interface allows CRUD/L operations for a document container.
There are following implementations of object/document collections:
- In-Memory collection (imtbase::CObjectCollection, imtbase::CObjectCollectionComp)
- File-based collection (imtrepo::CFileCollectionComp)
- SQL-Database collection (imtdb::CSqlDatabaseObjectCollectionComp)

An important aspect is the management of informations about objects containing in the collection. We differentiate between of followng kinds of such informations:
- Permanent/static information about a object/document in the collection such as Name or Description
- Meta-information related the object in the collection. For example Insertion Time, Last Modification Time. Important: these meta-informations are related to the Collection and not to the data object!
- Meta-information of the data object itself. Each object/document can have its own meta-informations

\defgroup FileRepository File Repository
\section Motivation Motivation
In the document-oriented application you need persistence of the documents or another grouped application data. File-based document repository is a way to do it.
File collections are specializations of imtbase::IObjectCollection interfrace.

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

We define following types of license data:
1. List of all product variants. This includes the overview of all available products and the associated licenses.
2. List of product installations.
3. Encrypted information file containing all license keys for a given product installation.

Each product application has knowledge about all available product licenses and 
activate them by reading and validating the license keys from the license file containing license keys.
The license keys will be defined by Product Manager before shipping of the installation setup to the customer.

*/

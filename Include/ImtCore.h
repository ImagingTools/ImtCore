#pragma once


/**
\defgroup ImtCore ImagingTools Core SDK
\mainpage
\section Introduction
Core SDK is an extension framework for ACF.
The main goal of this SDK is the implementation of extensions for core components and interfaces using for realization of the business logic,
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
Instead, there are interfaces for certain types of collections.
The most important one is - imtbase::IObjectCollection.
This interface describes the collection of data objects, that is, objects that implement directly or indirectly the istd::IChangeable interface.
"Concrete" collection interfaces can offer their specialized extensions to the imtbase::ICollectionInfo interface.
There is a suitable imtbase::IObjectCollectionInfo interface for imtbase::IObjectCollection
*/

/**
\defgroup DataManagement Data Management
\section Motivation Motivation
\section Example Examples
*/

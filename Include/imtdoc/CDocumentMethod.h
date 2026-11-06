// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <type_traits>
#include <utility>

// Qt includes
#include <QtCore/QtGlobal>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Type-erased call to a method of a document's concrete type.

	Binds a pointer-to-member-function together with its arguments at construction time, so that
	\c IDocumentService::ExecuteDocumentMethod can invoke it against the live document object it holds
	the lock for.

	The target type is reached by \c dynamic_cast, and that is a genuine \e cross-cast: the interface
	declaring the method needn't derive from \c istd::IChangeable, it only has to be a base of the
	same concrete document object. \c static_cast cannot express this.

	Both \c const and non-\c const methods can be bound, and their parameters must be taken by value
	or by \c const reference - see the constructors. The result of a non-\c void method is written to
	a variable passed to the constructor.

	\par Example
	\code
		documentServicePtr->ExecuteDocumentMethod(
					userId,
					documentId,
					imtdoc::CDocumentMethod(&MyDocument::AddItems, items));

		bool isRemoved = false;
		documentServicePtr->ExecuteDocumentMethod(
					userId,
					documentId,
					imtdoc::CDocumentMethod(&MyDocument::RemoveItem, isRemoved, itemId));
	\endcode
*/
class CDocumentMethod
{
public:
	/**
		\brief Bind \a method and \a args, to be invoked later against a document of type \a Document.

		\a Args is deduced from \a method alone; \a args is taken through the non-deduced \c std::decay_t,
		so a single template parameter pack describes both the method's formal parameters and the
		(by-value, decayed) copies stored for later invocation.

		\c operator() is \c const, so a method parameter that is a non-\c const lvalue reference cannot
		be bound.
	*/
	template <typename Document, typename... Args>
	explicit CDocumentMethod(void (Document::*method)(Args...), std::decay_t<Args>... args) :
				m_invoke(CreateInvoker<Document, void>(method, nullptr, std::move(args)...))
	{
	}

	/**
		\brief \c const overload of the constructor above.
	*/
	template <typename Document, typename... Args>
	explicit CDocumentMethod(void (Document::*method)(Args...) const, std::decay_t<Args>... args) :
				m_invoke(CreateInvoker<Document, void>(method, nullptr, std::move(args)...))
	{
	}

	/**
		\brief Bind \a method and \a args, to be invoked later against a document of type \a Document;
		the result of \a method is written to \a ret.

		\a Ret is deduced from \a method alone, \a ret only has to hold its decayed type.
		\a ret is referenced, not copied, so it must outlive every invocation of this object.
		\a ret is left unchanged if the method was not called.
	*/
	template <typename Ret, typename Document, typename... Args>
	explicit CDocumentMethod(Ret (Document::*method)(Args...), std::decay_t<Ret>& ret, std::decay_t<Args>... args) :
				m_invoke(CreateInvoker<Document, Ret>(method, &ret, std::move(args)...))
	{
	}

	/**
		\brief \c const overload of the constructor above.
	*/
	template <typename Ret, typename Document, typename... Args>
	explicit CDocumentMethod(Ret (Document::*method)(Args...) const, std::decay_t<Ret>& ret, std::decay_t<Args>... args) :
				m_invoke(CreateInvoker<Document, Ret>(method, &ret, std::move(args)...))
	{
	}

	/**
		\brief Invoke the bound method on \a document.

		\return  \c true on success, or \c false if \a document is not of the bound type, in which case
		         the method was not called.
	*/
	[[nodiscard]] bool operator()(istd::IChangeable& document) const
	{
		return m_invoke(document);
	}

private:
	typedef std::function<bool(istd::IChangeable&)> Invoker;

	/**
		Creates the invoker of \a method; for a non-\c void \a Ret the result is written to \a retPtr.
	*/
	template <typename Document, typename Ret, typename Method, typename... BoundArgs>
	static Invoker CreateInvoker(Method method, std::decay_t<Ret>* retPtr, BoundArgs... args)
	{
		return [method,
					retPtr,
#if __cplusplus >= 202002L // C++20 or newer
					...args = std::move(args)
#else
					args...
#endif
					](istd::IChangeable& document) {
			auto* documentPtr = dynamic_cast<Document*>(&document);
			Q_ASSERT(documentPtr != nullptr);
			if (documentPtr == nullptr){
				return false;
			}

			if constexpr (std::is_void_v<Ret>){
				Q_UNUSED(retPtr);

				(documentPtr->*method)(args...);
			}
			else{
				*retPtr = (documentPtr->*method)(args...);
			}

			return true;
		};
	}

private:
	Invoker m_invoke;
};


} // namespace imtdoc

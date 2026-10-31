#pragma once

#include "kl/core/types.hpp"

// TODO: Try to forward declare traits and move the implementation out of the way.
namespace kl
{
	template <typename T, typename U>          constexpr b8 isSame                          = false;
	template <typename T>                      constexpr b8 isSame<T, T>                    = true;
	template <typename T, typename... Us>      constexpr b8 isAnyOf                         = (isSame<T, Us> || ...);
	template <typename T>                      constexpr b8 isConst                         = false;
	template <typename T>                      constexpr b8 isConst<const T>                = true;
	template <typename T>                      constexpr b8 isVolatile                      = false;
	template <typename T>                      constexpr b8 isVolatile<volatile T>          = true;

	template <b8 C, typename T, typename F> struct _select              { using Type = T; };
	template <typename T, typename F>       struct _select<false, T, F> { using Type = F; };
	template <typename T>                   struct _removeC             { using Type = T; };
	template <typename T>                   struct _removeC<const T>    { using Type = T; };
	template <typename T>                   struct _removeV             { using Type = T; };
	template <typename T>                   struct _removeV<volatile T> { using Type = T; };
	template <typename T>                   struct _removeRef           { using Type = T; };
	template <typename T>                   struct _removeRef<T&>       { using Type = T; };
	template <typename T>                   struct _removeRef<T&&>      { using Type = T; };
	template <typename Dst, typename Src>   struct _copyRef             { using Type = Dst; };
	template <typename Dst, typename Src>   struct _copyRef<Dst, Src&>  { using Type = Dst&; };
	template <typename Dst, typename Src>   struct _copyRef<Dst, Src&&> { using Type = Dst&&; };

	template <b8 C, typename T, typename F> using select      = typename _select<C, T, F>::Type;
	template <typename T>                   using removeC     = typename _removeC<T>::Type;
	template <typename T>                   using removeV     = typename _removeV<T>::Type;
	template <typename T>                   using removeCV    = removeC<removeV<T>>;
	template <typename T>                   using removeRef   = typename _removeRef<T>::Type;
	template <typename T>                   using removeCVRef = removeCV<removeRef<T>>;
	template <typename Dst, typename Src>   using copyC       = select<isConst<Src>,    const Dst,    Dst>;
	template <typename Dst, typename Src>   using copyV       = select<isVolatile<Src>, volatile Dst, Dst>;
	template <typename Dst, typename Src>   using copyCV      = copyC<copyV<Dst, Src>, Src>;
	template <typename Dst, typename Src>   using copyRef     = typename _copyRef<Dst, Src>::Type;
	template <typename Dst, typename Src>   using copyCVRef   = copyRef<copyCV<Dst, removeRef<Src>>, Src>;

	// fundamental
	//   arithmetic
	//     integral
	//       boolean
	//       character
	//       integer
	//         signed integer
	//         unsigned integer
	//     floating point
	//   void
	//   nullptr
	// compound
	//   reference
	//   pointer
	//   member pointer
	//   array
	//   function
	//   enumeration
	//   class
	//     union

	template <typename T>                      constexpr b8 isBool                          = isAnyOf<removeCV<T>, b8>;
	template <typename T>                      constexpr b8 isCharacter                     = isAnyOf<removeCV<T>, c8, c16, utf8, utf16, utf32>;
	template <typename T>                      constexpr b8 isSignedInteger                 = isAnyOf<removeCV<T>, i8, i16, i32, i64, signed long>;
	template <typename T>                      constexpr b8 isUnsignedInteger               = isAnyOf<removeCV<T>, u8, u16, u32, u64, unsigned long>;
	template <typename T>                      constexpr b8 isFloat                         = isAnyOf<removeCV<T>, f32, f64, long double>;
	template <typename T>                      constexpr b8 isVoid                          = isAnyOf<removeCV<T>, void>;
	template <typename T>                      constexpr b8 isNull                          = isAnyOf<removeCV<T>, null>;

	template <typename T>                      constexpr b8 isInteger                       = isSignedInteger<T> || isUnsignedInteger<T>;
	template <typename T>                      constexpr b8 isIntegral                      = isBool<T> || isCharacter<T> || isInteger<T>;
	template <typename T>                      constexpr b8 isArithmetic                    = isIntegral<T> || isFloat<T>;
	template <typename T>                      constexpr b8 isFundamental                   = isArithmetic<T> || isVoid<T> || isNull<T>;

	template <typename T>                      constexpr b8 isSigned                        = isArithmetic<T> && requires { requires T(-1) < T(0); };
	template <typename T>                      constexpr b8 isUnsigned                      = isArithmetic<T> && requires { requires T(-1) > T(0); };

	template <typename T>                      constexpr b8 isLRef                          = false;
	template <typename T>                      constexpr b8 isLRef<T&>                      = true;
	template <typename T>                      constexpr b8 isRRef                          = false;
	template <typename T>                      constexpr b8 isRRef<T&&>                     = true;
	template <typename T>                      constexpr b8 isRef                           = isLRef<T> || isRRef<T>;
	template <typename T>                      constexpr b8 _isPtr                          = false;
	template <typename T>                      constexpr b8 _isPtr<T*>                      = true;
	template <typename T>                      constexpr b8 isPtr                           = _isPtr<removeCV<T>>;
	template <typename T>                      constexpr b8 _isMemberPtr                    = false;
	template <typename T, typename U>          constexpr b8 _isMemberPtr<T U::*>            = true;
	template <typename T>                      constexpr b8 isMemberPtr                     = _isMemberPtr<removeCV<T>>;
	template <typename T>                      constexpr b8 isArray                         = false;
	template <typename T>                      constexpr b8 isArray<T[]>                    = true;
	template <typename T, u64 N>               constexpr b8 isArray<T[N]>                   = true;
	template <typename T>                      constexpr b8 isFunction                      = !isConst<const T> && !isRef<T>;
	template <typename T>                      constexpr b8 isEnum                          = __is_enum(T);
	template <typename T>                      constexpr b8 isClass                         = __is_class(T);
	template <typename T>                      constexpr b8 isUnion                         = __is_union(T);
	template <typename T>                      constexpr b8 isCompound                      = !isFundamental<T>;

	template <typename T>                      constexpr b8 isAbstract                      = __is_abstract(T);
	template <typename T>                      constexpr b8 isAggregate                     = __is_aggregate(T);
	template <typename To, typename From>      constexpr b8 isAssignable                    = __is_assignable(To, From);
	template <typename Base, typename Derived> constexpr b8 isBaseOf                        = __is_base_of(Base, Derived);
	template <typename T, typename... Args>    constexpr b8 isConstructible                 = __is_constructible(T, Args...);
	template <typename To, typename From>      constexpr b8 isConvertibleTo                 = __is_convertible_to(From, To);
	template <typename T>                      constexpr b8 isDestructible                  = __is_destructible(T);
	template <typename T>                      constexpr b8 isEmpty                         = __is_empty(T);
	template <typename T>                      constexpr b8 isFinal                         = __is_final(T);
	template <typename T, typename U>          constexpr b8 isLayoutCompatible              = __is_layout_compatible(T, U);
	template <typename T>                      constexpr b8 isLiteralType                   = __is_literal_type(T);
	template <typename T>                      constexpr b8 isPod                           = __is_pod(T);
	template <typename Base, typename Derived> constexpr b8 isPointerInterconvertibleBaseOf = __is_pointer_interconvertible_base_of(Base, Derived);
	template <typename T>                      constexpr b8 isPolymorphic                   = __is_polymorphic(T);
	template <typename T>                      constexpr b8 isStandardLayout                = __is_standard_layout(T);
	template <typename T>                      constexpr b8 isTrivial                       = __is_trivial(T);
	template <typename To, typename From>      constexpr b8 isTriviallyAssignable           = __is_trivially_assignable(To, From);
	template <typename T>                      constexpr b8 isTriviallyCopyAssignable       = __is_trivially_assignable(T&, const T&);
	template <typename T>                      constexpr b8 isTriviallyMoveAssignable       = __is_trivially_assignable(T&, T);
	template <typename T, typename... Args>    constexpr b8 isTriviallyConstructible        = __is_trivially_constructible(T, Args...);
	template <typename T>                      constexpr b8 isTriviallyCopyConstructible    = __is_trivially_constructible(T, const T&);
	template <typename T>                      constexpr b8 isTriviallyMoveConstructible    = __is_trivially_constructible(T, T);
	template <typename T>                      constexpr b8 isTriviallyCopyable             = __is_trivially_copyable(T);
	template <typename T>                      constexpr b8 isTriviallyDestructible         = __is_trivially_destructible(T);

	// TODO: Where should this live?
	// TODO: Can replace is with `if consteval` if using C++ 23
	constexpr b8 isConstantEvaluated() { return __builtin_is_constant_evaluated(); }

	// TODO: Implement
	#if false
	isScalar
	isObject
	isImplicitLifetime
	isBoundedArray
	isUnboundedArray
	isScopedEnum
	isDefaultConstructible
	isCopyConstructible
	isMoveConstructible
	isNothrowAssignable
	isCopyAssignable
	isMoveAssignable
	hasVirtualDestructor
	isSwappableWith
	isSwappable
	isVirtualBaseOf
	isConvertible
	isInvocable
	isInvocableRet
	removeExtent
	removeAllExtents
	removePointer
	alignedStorage
	alignedUnion
	decay
	commonType
	commonReference
	basicCommonReference
	underlyingType
	invokeResult
	typeIdentity
	conjunction
	disjunction
	negation
	#endif

	//template <typename T1, typename T2, typename M1, typename M2> constexpr b8 isCorrespondingMember = __is_corresponding_member(T1, T2, M1, M2);
	//template <typename T> constexpr b8 hasUniqueObjectRepresentations = __has_unique_object_representations(T);
	//template <typename T> constexpr b8 hasVirtualDestructor           = __has_virtual_destructor(T);

	//alignmentOf
	//rank
	//extent
	//__builtin_addressof(_Val)
	//__builtin_bit_cast(_To, _Val)
	//__underlying_type(_Ty)
}

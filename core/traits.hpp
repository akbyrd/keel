#pragma once

#include "core/types.hpp"

template <typename T> struct _removeC                   { using Type = T; };
template <typename T> struct _removeC<const T>          { using Type = T; };
template <typename T> using removeC = typename _removeC<T>::Type;

template <typename T> struct _removeV                   { using Type = T; };
template <typename T> struct _removeV<volatile T>       { using Type = T; };
template <typename T> using removeV = typename _removeV<T>::Type;

template <typename T> struct _removeCV                   { using Type = T; };
template <typename T> struct _removeCV<const T>          { using Type = T; };
template <typename T> struct _removeCV<volatile T>       { using Type = T; };
template <typename T> struct _removeCV<const volatile T> { using Type = T; };
template <typename T> using removeCV = typename _removeCV<T>::Type;

template <typename T> struct _removeRef      { using Type = T; };
template <typename T> struct _removeRef<T&>  { using Type = T; };
template <typename T> struct _removeRef<T&&> { using Type = T; };
template <typename T> using removeRef = typename _removeRef<T>::Type;

template <typename T, typename U>          constexpr b8 isSame                             = false;
template <typename T>                      constexpr b8 isSame<T, T>                       = true;
template <typename T, typename... Us>      constexpr b8 isAnyOf                            = (isSame<T, Us> || ...);

template <typename T>                      constexpr b8 _isPointer                         = false;
template <typename T>                      constexpr b8 _isPointer<T*>                     = true;
template <typename T>                      constexpr b8 isPointer                          = _isPointer<removeCV<T>>;

template <typename T>                      constexpr b8 isIntegral                         = isAnyOf<removeCV<T>,
	bool,
	wchar_t, char8_t, char16_t, char32_t,
	char,      unsigned char, signed char,
	short,     unsigned short,
	int,       unsigned int,
	long,      unsigned long,
	long long, unsigned long long> || isPointer<T>;

template <typename T>                      constexpr b8 isFloatingPoint                    = isAnyOf<removeCV<T>, float, double, long double>;
template <typename T>                      constexpr b8 isArithmetic                       = isIntegral<T> || isFloatingPoint<T>;
template <typename T>                      constexpr b8 isNull                             = isSame<removeCV<T>, decltype(nullptr)>;
template <typename T>                      constexpr b8 isSigned                           = isIntegral<T> && T(-1) <  T(0);
template <typename T>                      constexpr b8 isUnsigned                         = isIntegral<T> && T(-1) >= T(0);
template <typename T>                      constexpr b8 isVoid                             = isSame<removeCV<T>, void>;
template <typename T>                      constexpr b8 isFundamental                      = isArithmetic<T> || isVoid<T> || isNull<T>;

template <typename T>                      constexpr b8 isLValueRef                        = false;
template <typename T>                      constexpr b8 isLValueRef<T&>                    = true;
template <typename T>                      constexpr b8 isRValueRef                        = false;
template <typename T>                      constexpr b8 isRValueRef<T&&>                   = true;

template <typename T>                      constexpr b8 isConst                            = false;
template <typename T>                      constexpr b8 isConst<const T>                   = true;

template <typename T>                      constexpr b8 isEnum                             = __is_enum(T);
template <typename T>                      constexpr b8 isUnion                            = __is_union(T);
template <typename T>                      constexpr b8 isAbstract                         = __is_abstract(T);
template <typename T>                      constexpr b8 isAggregate                        = __is_aggregate(T);
template <typename To, typename From>      constexpr b8 isAssignable                       = __is_assignable(To, From);
template <typename Base, typename Derived> constexpr b8 isBaseOf                           = __is_base_of(Base, Derived);
template <typename T>                      constexpr b8 isClass                            = __is_class(T);
template <typename T, typename... Args>    constexpr b8 isConstructible                    = __is_constructible(T, Args...);
template <typename To, typename From>      constexpr b8 isConvertibleTo                    = __is_convertible_to(From, To);
template <typename T>                      constexpr b8 isDestructible                     = __is_destructible(T);
template <typename T>                      constexpr b8 isEmpty                            = __is_empty(T);
template <typename T>                      constexpr b8 isFinal                            = __is_final(T);
template <typename T, typename U>          constexpr b8 isLayoutCompatible                 = __is_layout_compatible(T, U);
template <typename T>                      constexpr b8 isLiteralType                      = __is_literal_type(T);
template <typename T>                      constexpr b8 isPod                              = __is_pod(T);
template <typename Base, typename Derived> constexpr b8 isPointerInterconvertibleBaseOf    = __is_pointer_interconvertible_base_of(Base, Derived);
template <typename T>                      constexpr b8 isPolymorphic                      = __is_polymorphic(T);
template <typename T>                      constexpr b8 isStandardLayout                   = __is_standard_layout(T);
template <typename T>                      constexpr b8 isTrivial                          = __is_trivial(T);
template <typename To, typename From>      constexpr b8 isTriviallyAssignable              = __is_trivially_assignable(To, From);
template <typename T>                      constexpr b8 isTriviallyCopyAssignable          = __is_trivially_assignable(T&, const T&);
template <typename T>                      constexpr b8 isTriviallyMoveAssignable          = __is_trivially_assignable(T&, T);
template <typename T, typename... Args>    constexpr b8 isTriviallyConstructible           = __is_trivially_constructible(T, Args...);
template <typename T>                      constexpr b8 isTriviallyCopyConstructible       = __is_trivially_constructible(T, const T&);
template <typename T>                      constexpr b8 isTriviallyMoveConstructible       = __is_trivially_constructible(T, T);
template <typename T>                      constexpr b8 isTriviallyCopyable                = __is_trivially_copyable(T);
template <typename T>                      constexpr b8 isTriviallyDestructible            = __is_trivially_destructible(T);

// TODO: Where should this live?
// TODO: Can replace is with `if consteval` if using C++ 23
constexpr bool isConstantEvaluated() { return __builtin_is_constant_evaluated(); }

// TODO: Implement
#if false
isArray
isFunction
isArithmetic
isScalar
isObject
isCompound
isReference
isVolatile
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
conditional
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


#ifndef STX_ANY_HPP_INCLUDED
#define STX_ANY_HPP_INCLUDED

#ifndef STX_NAMESPACE_NAME
#define STX_NAMESPACE_NAME hpp
#endif
#define STX_NO_STD_ANY
// libstdc++ std::experimental::any only works in C++14 mode
#if !defined(STX_NO_STD_ANY) && defined(__GNUC__) && (__cplusplus < 201402)
#define STX_NO_STD_ANY
#endif

#if defined(__has_include) && !defined(STX_NO_STD_ANY)
#if __has_include(<any>) && (__cplusplus > 201402)
#include <any>
namespace STX_NAMESPACE_NAME
{
    using std::any;
    using std::any_cast;
    using std::bad_any_cast;
} // namespace STX_NAMESPACE_NAME
#define STX_HAVE_STD_ANY 1
#elif __has_include(<experimental/any>)
#include <experimental/any>
namespace STX_NAMESPACE_NAME
{
    using std::experimental::any;
    using std::experimental::any_cast;
    using std::experimental::bad_any_cast;
} // namespace STX_NAMESPACE_NAME
#define STX_HAVE_STD_ANY 1
#endif // __hasinclude(any)
#endif // defined(__hasinclude)

#ifndef STX_HAVE_STD_ANY

#include <new>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "utility/if_constexpr.hpp"

namespace STX_NAMESPACE_NAME
{

    class bad_any_cast : public std::bad_cast
    {
    public:
        const char* what() const noexcept override { return "bad any cast"; }
    };

    class any final
    {
    public:
        /// Constructs an object of type any with an empty state.
        any() : vtable(nullptr) {}

        /// Constructs an object of type any with an equivalent state as other.
        any(const any& rhs) : vtable(rhs.vtable)
        {
            if (!rhs.empty())
            {
                rhs.vtable->copy(rhs.storage, this->storage);
            }
        }

        /// Constructs an object of type any with a state equivalent to the original state of other.
        /// rhs is left in a valid but otherwise unspecified state.
        any(any&& rhs) noexcept : vtable(rhs.vtable)
        {
            if (!rhs.empty())
            {
                rhs.vtable->move(rhs.storage, this->storage);
                rhs.vtable = nullptr;
            }
        }

        /// Same effect as this->clear().
        ~any() { this->clear(); }

        /// Constructs an object of type any that contains an object of type T direct-initialized with
        /// std::forward<ValueType>(value).
        ///
        /// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
        /// This is because an `any` may be copy constructed into another `any` at any time, so a copy should
        /// always be allowed.
        template<typename ValueType, typename = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value>::type>
        any(ValueType&& value)
        {
            static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
                          "T shall satisfy the CopyConstructible requirements.");
            this->construct(std::forward<ValueType>(value));
        }

        /// Has the same effect as any(rhs).swap(*this). No effects if an exception is thrown.
        any& operator=(const any& rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        /// Has the same effect as any(std::move(rhs)).swap(*this).
        ///
        /// The state of *this is equivalent to the original state of rhs and rhs is left in a valid
        /// but otherwise unspecified state.
        any& operator=(any&& rhs) noexcept
        {
            any(std::move(rhs)).swap(*this);
            return *this;
        }

        /// Has the same effect as any(std::forward<ValueType>(value)).swap(*this). No effect if a exception is
        /// thrown.
        ///
        /// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
        /// This is because an `any` may be copy constructed into another `any` at any time, so a copy should
        /// always be allowed.
        template<typename ValueType, typename = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value>::type>
        any& operator=(ValueType&& value)
        {
            static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
                          "T shall satisfy the CopyConstructible requirements.");
            any(std::forward<ValueType>(value)).swap(*this);
            return *this;
        }

        /// If not empty, destroys the contained object.
        void clear() noexcept
        {
            if (!empty())
            {
                this->vtable->destroy(storage);
                this->vtable = nullptr;
            }
        }

        /// Returns true if *this has no contained object, otherwise false.
        bool empty() const noexcept { return this->vtable == nullptr; }

        /// If *this has a contained object of type T, typeid(T); otherwise typeid(void).
        const std::type_info& type() const noexcept { return empty() ? typeid(void) : this->vtable->type(); }

        /// Exchange the states of *this and rhs.
        void swap(any& rhs) noexcept
        {
            if (this->vtable != rhs.vtable)
            {
                any tmp(std::move(rhs));

                // move from *this to rhs.
                rhs.vtable = this->vtable;
                if (this->vtable != nullptr)
                {
                    this->vtable->move(this->storage, rhs.storage);
                    // this->vtable = nullptr; -- uneeded, see below
                }

                // move from tmp (previously rhs) to *this.
                this->vtable = tmp.vtable;
                if (tmp.vtable != nullptr)
                {
                    tmp.vtable->move(tmp.storage, this->storage);
                    tmp.vtable = nullptr;
                }
            }
            else // same types
            {
                if (this->vtable != nullptr)
                    this->vtable->swap(this->storage, rhs.storage);
            }
        }

    private: // Storage and Virtual Method Table
        union storage_union
        {
            using stack_storage_t = typename std::aligned_storage<2 * sizeof(void*), std::alignment_of<void*>::value>::type;

            void*           dynamic;
            stack_storage_t stack; // 2 words for e.g. shared_ptr
        };

        /// Base VTable specification.
        struct vtable_type
        {
            // Note: The caller is responssible for doing .vtable = nullptr after destructful operations
            // such as destroy() and/or move().

            /// The type of the object this vtable is for.
            const std::type_info& (*type)() noexcept;

            /// Destroys the object in the union.
            /// The state of the union after this call is unspecified, caller must ensure not to use src anymore.
            void (*destroy)(storage_union&) noexcept;

            /// Copies the **inner** content of the src union into the yet unitialized dest union.
            /// As such, both inner objects will have the same state, but on separate memory locations.
            void (*copy)(const storage_union& src, storage_union& dest);

            /// Moves the storage from src to the yet unitialized dest union.
            /// The state of src after this call is unspecified, caller must ensure not to use src anymore.
            void (*move)(storage_union& src, storage_union& dest) noexcept;

            /// Exchanges the storage between lhs and rhs.
            void (*swap)(storage_union& lhs, storage_union& rhs) noexcept;
        };

        /// VTable for dynamically allocated storage.
        template<typename T>
        struct vtable_dynamic
        {
            static const std::type_info& type() noexcept { return typeid(T); }

            static void destroy(storage_union& storage) noexcept
            {
                // assert(reinterpret_cast<T*>(storage.dynamic));
                delete reinterpret_cast<T*>(storage.dynamic);
            }

            static void copy(const storage_union& src, storage_union& dest) { dest.dynamic = new T(*reinterpret_cast<const T*>(src.dynamic)); }

            static void move(storage_union& src, storage_union& dest) noexcept
            {
                dest.dynamic = src.dynamic;
                src.dynamic  = nullptr;
            }

            static void swap(storage_union& lhs, storage_union& rhs) noexcept
            {
                // just exchage the storage pointers.
                std::swap(lhs.dynamic, rhs.dynamic);
            }
        };

        /// VTable for stack allocated storage.
        template<typename T>
        struct vtable_stack
        {
            static const std::type_info& type() noexcept { return typeid(T); }

            static void destroy(storage_union& storage) noexcept { reinterpret_cast<T*>(&storage.stack)->~T(); }

            static void copy(const storage_union& src, storage_union& dest) { new (&dest.stack) T(reinterpret_cast<const T&>(src.stack)); }

            static void move(storage_union& src, storage_union& dest) noexcept
            {
                // one of the conditions for using vtable_stack is a nothrow move constructor,
                // so this move constructor will never throw a exception.
                new (&dest.stack) T(std::move(reinterpret_cast<T&>(src.stack)));
                destroy(src);
            }

            static void swap(storage_union& lhs, storage_union& rhs) noexcept
            {
                std::swap(reinterpret_cast<T&>(lhs.stack), reinterpret_cast<T&>(rhs.stack));
            }
        };

        /// Whether the type T must be dynamically allocated or can be stored on the stack.
        template<typename T>
        struct requires_allocation : std::integral_constant<bool,
                                                            !(std::is_nothrow_move_constructible<T>::value // N4562 §6.3/3 [any.class]

                                                                  & sizeof(T) <= sizeof(storage_union::stack) &&

                                                              td::alignment_of<T>::value <= td::alignment_of<storage_union::stack_storage_t>::value)>

                                     ;

        // Returns the pointer to the vtable of the type T.

        emplate typename T >

            tatic vtable_type* vtable_for_type()

                sing VTableType typename std::conditional<requires_allocation<T>::value, vtable_dynamic<T>, table_stack<T>>::type;

        tatic vtable_type table = {

            TableType::type,
            TableType::destroy,
            TableType::copy,
            TableType::move,
            TableType::swap,

            ;

        eturn& table;

        rotected :

            emplate typename T >

            riend const T*
            any_cast(const any* operand) noexcept;

        emplate typename T >

            riend T* any_cast(any* operand) noexcept;

        // Same effect as is_same(this->type(), t);

        ool is_typed(const std::type_info& t) const

            eturn is_same(this->type(), t);

        // Checks if two type infos are the same.

        //

        // If ANY_IMPL_FAST_TYPE_INFO_COMPARE is defined, checks only the address of the

        // type infos, otherwise does an actual comparision. Checking addresses is

        // only a valid approach when there's no interaction with outside sources

        // (other shared libraries and such).

        tatic bool is_same(const std::type_info& a, const std::type_info& b)

#ifdef ANY_IMPL_FAST_TYPE_INFO_COMPARE

                eturn& a
            == &b;
#else

                eturn a
            == b;
#endif

        // Casts (with no type_info checks) the storage pointer as const T*.

        emplate typename T >

            onst T* cast() const noexcept

            eturn requires_allocation<typename std::decay<T>::type>::value reinterpret_cast<const T*>(storage.dynamic)

                einterpret_cast<const T*>(&storage.stack);

        // Casts (with no type_info checks) the storage pointer as T*.

        emplate typename T >

            *cast() noexcept

            eturn requires_allocation<typename std::decay<T>::type>::value reinterpret_cast<T*>(storage.dynamic)

                einterpret_cast<T*>(&storage.stack);

        rivate :

            torage_union storage; // on offset(0) so no padding for align

        table_type* table;

        // Chooses between stack and dynamic allocation for the type decay_t<ValueType>,

        // assigns the correct vtable, and constructs the object on our storage.

        emplate typename ValueType >

            oid construct(ValueType&& value)

                sing T = typename std::decay<ValueType>::type;

        his->vtable = vtable_for_type<T>();

        f_constexpr(requires_allocation<T>::value)

            torage.dynamic = new T(std::forward<ValueType>(value));

        lse_constexpr

            ew(&storage.stack) T(std::forward<ValueType>(value));

        nd_if_constexpr;

        ;

        amespace detail

                emplate typename ValueType >

            nline ValueType any_cast_move_if_true(typename std::remove_reference<ValueType>::type* p, std::true_type)

                eturn std::move(*p);

        emplate typename ValueType >

            nline ValueType any_cast_move_if_true(typename std::remove_reference<ValueType>::type* p, std::false_type)

                eturn* p;

        // namespace detail

        // Performs *any_cast<add_const_t<remove_reference_t<ValueType>>>(&operand), or throws bad_any_cast on

        // failure.

        emplate typename ValueType >

            nline ValueType any_cast(const any& operand)

                uto p = ny_cast<typename std::add_const<typename std::remove_reference<ValueType>::type>::type>(&operand);

        f(p == nullptr)

            hrow bad_any_cast();

        eturn* p;

        // Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.

        emplate typename ValueType >

            nline ValueType any_cast(any& operand)

                uto p = any_cast<typename std::remove_reference<ValueType>::type>(&operand);

        f(p == nullptr)

            hrow bad_any_cast();

        eturn* p;

        //

        // If ANY_IMPL_ANYCAST_MOVEABLE is not defined, does as N4562 specifies:

        //     Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.

        //

        // If ANY_IMPL_ANYCAST_MOVEABLE is defined, does as LWG Defect 2509 specifies:

        //     If ValueType is MoveConstructible and isn't a lvalue reference, performs

        //     std::move(*any_cast<remove_reference_t<ValueType>>(&operand)), otherwise

        //     *any_cast<remove_reference_t<ValueType>>(&operand). Throws bad_any_cast on failure.

        //

        emplate typename ValueType >

            nline ValueType any_cast(any&& operand)

#ifdef ANY_IMPL_ANY_CAST_MOVEABLE

                / https : // cplusplus.github.io/LWG/lwg-active.html#2509

                          sing can_move = std::integral_constant < bool,
                               std::is_move_constructible<ValueType>::value&& std::is_lvalue_reference<ValueType>::value > ;
#else

                sing can_move = std::false_type;
#endif

        uto p = any_cast<typename std::remove_reference<ValueType>::type>(&operand);

        f(p == nullptr)

            hrow bad_any_cast();

        eturn detail::any_cast_move_if_true<ValueType>(p, can_move());

        // If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object

        // contained by operand, otherwise nullptr.

        emplate typename T >

            nline const T* any_cast(const any* operand) noexcept

            f(operand == nullptr || !operand->is_typed(typeid(T)))

                eturn nullptr;

        lse

            eturn operand->cast<T>();

        // If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object

        // contained by operand, otherwise nullptr.

        emplate typename T >

            nline T* any_cast(any* operand) noexcept

            f(operand == nullptr || !operand->is_typed(typeid(T)))

                eturn nullptr;

        lse

            eturn operand->cast<T>();

    } /// namespace STX_NAMESPACE_NAME

    namespace std
    {

        nline void swap(STX_NAMESPACE_NAME::any & lhs, STX_NAMESPACE_NAME::any & rhs) noexcept

            hs.swap(rhs);
    }
    // namespace std

#endif // STX_HAVE_STD_ANY

#endif //  STX_ANY_HPP_INCLUDED

<div align="center">

Variadic library

---

[Example](/example.cpp) - [Documentation](https://koniarik.github.io/vari/)

</div>

C++ has `std::variant<Ts...>` as a tagged union, but we find it lacking in capabilities and just plain ... bad.

`vari` introduces enhanced alternatives with a more advanced API, flexible type conversions, and improved ways to define variadic types.

The library introduces four basic types:
 - `vptr<Ts...>` - A pointer to any type out of `Ts...`, which can be null.
 - `vref<Ts...>` - A reference to any type out of `Ts...`.
 - `uvptr<Ts...>` - A unique (owning) pointer to any type out of `Ts...`, which can be null.
 - `uvref<Ts...>` - A unique (owning) reference to any type out of `Ts...`.

---

- [vref and vptr](#vref-and-vptr)
- [uvptr and uvref](#uvptr-and-uvref)
- [Access API](#access-api)
  - [Visit](#visit)
  - [Take](#take)
- [Sub-typing](#sub-typing)
- [Concepts checks](#concepts-checks)
- [Single-type extension](#single-type-extension)
- [Type-sets](#type-sets)
- [Lvalue conversion from unique](#lvalue-conversion-from-unique)
- [Const](#const)
- [Typelist compatibility](#typelist-compatibility)
- [Dispatch](#dispatch)
- [Credits](#credits)


## vref and vptr

`vref` and `vptr` are used to point to any type from a specified list of types. The `vref` always points to a valid object, whereas `vptr` can be null.

```cpp
auto foo = [&](vref<int, float> v){
    v.visit([&](int& i)  { std::cout << "this is int: "   << i << std::endl;},
            [&](float& f){ std::cout << "this is float: " << f << std::endl;});
};

int i;
foo(i); // << vref<int,float> points to `i`

float f;
foo(f); // << vref<int, float> points to `f`
```

Here `foo` accepts a `vref` that references either an `int` or a `float`.

## uvptr and uvref

The `u`-prefixed variants (`uvptr` and `uvref`) imply unique ownership, which means they manage the lifetimes of objects.

```cpp
struct a_t{};
struct b_t{};

vari::uvptr<a_t, b_t> p;

```

Similar to `std::make_unique`, we provide a function `uwrap` for construction of unique variants:

```cpp
uvref<std::string, int> p = uwrap(std::string{"wololo"});
```

Here `uwrap` creates `uvref<std::string>` which gets converted into `uvref<std::string,int>` due to implicit conversion.

WARNING: `uvref` is movable, and when moved from, it enters a null state. It shall not be used in this state except for reassignment.

## Access API

To access the underlying type, `vptr`, `vref`, `uvptr`, and `uvref` use the `visit` method as the  primary interface. The `u` variants also have `take` to transfer ownership.

### Visit

The `visit` method works similarly to `std::visit` but allows multiple callables:

```cpp
auto foo = [&]( vari::vref< std::vector< std::string >, std::list< std::string > > r ) -> std::string&
{
        std::string& front = r.visit(
            [&]( std::vector< std::string >& v ) -> std::string& {
                    return v.front();
            },
            [&]( std::list< std::string >& l ) -> std::string& {
                    return l.front();
            } );
        return front;
};
```

For pointers, there must be a callable that accepting `vari::empty_t` to handle cases where the pointer is null:

```cpp
vari::vptr<int, std::string> r = nullptr;

r.visit([&](vari::empty_t){},
        [&](int&){},
        [&](std::string&){});
```

Variadic references can be constructed with references to any of the possible types:

```cpp
std::string a;
vari::vref<int, std::string> r = a;
```

This also allows us to combine it with `visit`, where the callable can handle multiple types:

```cpp
vari::uvptr<int, std::string> r;

r.visit([&](vari::empty_t){},
        [&](vari::vref<int, std::string>){});

```

Or we can mix both approaches:

```cpp
vari::uvptr<int, float, std::string> r = nullptr;

r.visit([&](vari::empty_t){},
        [&](std::string&){},
        [&](vari::vref<int, float>){});

```

### Take

`uvref` and `uvptr` retain ownership of referenced items, the `take` method is used to transfer ownership:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    std::move(r).take([&](vari::uvref<int>){},
                      [&](vari::uvref<std::string>){});
};
```

## Sub-typing

All variadic types support sub-typing, meaning any variadic type can be converted into a type that represents a superset of its types:

```cpp
std::string a;
vari::vref<std::string> p{a};

// allowed as {int, std::string} is superset of {std::string}
vari::vref<int, std::string> p2 = p;

// not allowed, as {int} is not superset of {int, std::string}
vari::vref<int, std::string> p3 = p2;
```

This feature also works seamlessly with `take`:

```cpp
struct a_t{};
struct b_t{};
struct c_t{};
struct d_t{};

auto foo = [&](vari::uvptr<a_t, b_t, c_t, d_t> p)
{
    std::move(p).take([&](vari::empty_t){},
                      [&](vari::uvref<a_t, b_t>){},
                      [&](vari::uvref<c_t, d_t>){});
};
```

In this example, `p` represents a set of four types. The `take` method allows us to split this set into four unique references, each representing one type. Sub-typing enables us to combine these references into two subsets, each consisting of two types.

Note: As a side-effect of this, `vptr<a_t, b_t>` is naturally convertible to `vptr<b_t, a_t>`

## Concepts checks

Access methods are subject to sanity checks on the set of provided callbacks: for each type in the set, exactly one callback must be callable.

For instance, the following code would fail to compile due to a concept check violation:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    r.visit([&](int&){},
            [&](int&){}, // error: second overload matching int
            [&](std::string&){});
};
```

Note that this rule also applies to templated arguments:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    r.visit([&](int&){},
            [&](auto&){}, // error: second overload matching int
            [&](std::string&){});
};
```

Another important check: each callable must be compatible with at least one type in the set.

```cpp
int i = 42;
vari::vref<int, float> v = i;
v.visit([&](int&){},
        [&](std::string&){}, // error: callable does not match any type
        [&](float&){});
```

## Single-type extension

To make working with variants more convenient, all variadic types allow direct access to the pointed-to type if there is only a single type in the type list:

```cpp
struct boo_t{
    int val;
};
boo_t b;
vari::vptr<boo_t> p = &b;

p->val = 42;
```

This feature makes `vref` a useful replacement for raw references in structures:

```cpp
struct my_type{
    vref<std::string> str;
};
```
If you used `std::string& str`, it would prevent the ability to reassign the reference within the structure. `vref`, however, does not have this limitation, allowing reassignment.

## Type-sets

For added convenience and functionality, the template argument list of variadic types can flatten and filter types for uniqueness.

Given the following type sets:

```cpp
using set_a = vari::typelist<int, std::string>;
using set_b = vari::typelist<float, int>;
using set_s = vari::typelist<set_a, set_b, std::string>;
```

The pointer `vptr<set_s>` resolves to the equivalent of `vptr<int, std::string, float>`. The flattening and filtering mechanism only applies to `vari::typelist`. For example, `void<std::tuple<a_t,b_t>>` would not be automatically resolved to a different form.

Why is this useful? It allows expressing complex data structures more effectively. Moreover, the typelists also interact well with sub-typing:

```cpp

using simple_types = vari::typelist<std::string, int, bool>;
struct array_t{};
struct object_t{};
using complex_types = vari::typelist<array_t, object_t>;
using json_types = vari::typelist<simple_types, complex_types>;

auto simple_to_str = [&](vari::vref<simple_types> p) { return std::string{}; };

auto to_str = [&](vari::vptr<json_types> p)
{
    using R = std::string;
    return p.visit([&](vari::empty_t) -> R { return ""; },
                   [&](vari::vref<simple_types> pp) -> R { return simple_to_str(pp); },
                   [&](array_t& pp) -> R { /* impl */ },
                   [&](object_t& pp) -> R { /* impl */ });
};
```

This approach makes it easier to handle complex type hierarchies while preserving the flexibility and power of variadic types.

## Lvalue conversion from unique

For added convenience, the library allows converting `uvptr` and `uvref` to their non-unique counterparts, but only if the expression is an lvalue reference:

```cpp
auto foo = [&](vari::vptr<int, std::string>){};

vari::uvptr<int> p;
foo(p); // allowed, `p` is lvalue

foo(vari::uvptr<std::string>{}); // forbidden, rvalue used
```

## Const

All variadic types support conversion from non-const version to const version:

```cpp
auto foo = [&](vari::vptr<int const, std::string const>){};

vari::vptr<int, std::string> p;
foo(p);
```

`const` is also properly propagated during typelist operations:

```cpp
using set_a = vari::typelist<int, std::string>;

using vp_a = vari::vptr<const set_a>;
using vp_b = vari::vptr<const int, const std::string>;
```

Both types `vp_a` and `vp_b` are compatible.

## Typelist compatibility

Library can be extended by using other types than just `vari::typelist` to represent set of types.

Whenever type is typelist is determined by `vari::typelist_traits<T>`. In case `vari::typelist_traits<T>:::is_compatible` evaluates to `true`, library considers `T` to be typelist-like type.

In such a case, `vari::typelist_traits<T>::types` should be a type which by itself is vari-compatible typelist. Transtively, this should eventually resolve into `vari::typelist` itself which is used by the library directly.

## Dispatch

We also ship free function `dispatch` for mapping a runtime value into compile-time value. It uses all
the safety checks of `visit`:

```cpp
// factory used by the library to create instances of types
auto factory = [&]<vari::index_type i>{
    return std::integral_constant<std::size_t, i>{};
};
// runtime index
vari::index_type v = 2;
vari::dispatch<3>(
    v,
    factory,
    [&](std::integral_constant<std::size_t, 0>){
        // `j` matches value of `v`
    },
    [&](std::integral_constant<std::size_t, 1>){
        // `j` matches value of `v`
    },
    [&](std::integral_constant<std::size_t, 2>){
        // `j` matches value of `v`
    });
```

## Credits

Credits for the idea for this should go to `avakar`, live long and prosper.

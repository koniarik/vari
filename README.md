<div align="center">

Variadic library

---

[Example](/example.cpp) - [Documentation](https://koniarik.github.io/vari/)

</div>

C++ has `std::variant<Ts...>` as a tagged union, but we find it lacking in capabilities and just plain ... bad.

`vari` introduces enhanced alternatives with different API, improved way of definition, or more flexible conversion between types.

4 basic types introduced by the library:
 - `vptr<Ts...>` - pointer to any type out of `Ts...` or null pointer
 - `vref<Ts...>` - reference to any type out of `Ts...`
 - `uvptr<Ts...>` - owning pointer to any type out of `Ts...` or null pointer
 - `uvref<Ts...>` - owning reference to any type out of `Ts...`

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

`vref` and `vptr` point to any type in specified list of types, `vref` guarantees that it points to something, while `vptr` can be null.

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

Here `foo` accepts vref that always references either `int` or `float value`.

## uvptr and uvref

`u` prefixed variants imply ownership (unique), this way we can use them to
manage lifetimes of allocated objects.

```cpp
struct a_t{};
struct b_t{};

vari::uvptr<a_t, b_t> p;

```

Similar (but not same) to `std::make_unique`, we have a friendly function to build
the unique variants: `uwrap`

```cpp
uvref<std::string, int> p = uwrap(std::string{"wololo"});
```

Here `uwrap` creates `uvref<std::string>` which gets converted into `uvref<std::string,int>` due to implicit conversion.

WARNING: `uvref` is movable object, once moved-from, it is put into `null` state - something not
possible otherwise. It shall not be used in this state in any matter except to be assigned-to.

## Access API

How to access the internal type? `vptr`, `vref`, `uvptr`, and `uvref` have `visit` method as the main interface.
`u` variants also have `take` to steal ownership.

### Visit

`visit` recreates approach similar to `std::visit`, except that we allow multiple callables instead of multiple variadics. Note that we can handle returning references from callables:

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

For each type of variadic, the `visit` expects to get one and only one callable accepting reference to that type.

In case of pointers, we opted to introduce empty branch for cases when it is null.

```cpp
vari::vptr<int, std::string> r = nullptr;

r.visit([&](vari::empty_t){},
        [&](int&){},
        [&](std::string&){});
```

Variadic references are constructible with references - all of the possible types:

```cpp
std::string a;
vari::vref<int, std::string> r = a;
```

This also means that we can combine this with visit - the callable can handle multiple types:

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

`uvref` and `uvptr` retain ownership of referenced items, `take` allows stealing the ownership
from the owner:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    std::move(r).take([&](vari::uvref<int>){},
                      [&](vari::uvref<std::string>){});
};
```

## Sub-typing

All variadic types support sub-typing - any variadic type can be converted to a type representing superset of types:

```cpp
std::string a;
vari::vref<std::string> p{a};

// allowed as {int, std::string} is superset of {std::string}
vari::vref<int, std::string> p2 = p;

// not allowed, as {int} is not superset of {int, std::string}
vari::vref<int, std::string> p3 = p2;
```

This also interacts well with `take`:

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

The way we can imagine this is: `p` can represent set of 4 types, `take` splits that into four unique references, each representing one type, *sub-typing* allows merging these references together - into two subsets, each made of two types.

Note: As a side-effect of this, `vptr<a_t, b_t>` is naturally convertible to `vptr<b_t, a_t>`

## Concepts checks

Access methods are subjected to sanity checking of the set of callbacks: For each type in the set of types, one and only one callback can be called.

That is, following would fail to compile due to concept check:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    r.visit([&](int&){},
            [&](int&){}, // error: second overload matching int
            [&](std::string&){});
};
```

Keep in mind, that this also affects templated arguments:

```cpp
auto foo = [&](vari::uvref<int, std::string> r)
{
    r.visit([&](int&){},
            [&](auto&){}, // error: second overload matching int
            [&](std::string&){});
};
```

As a second check: For each callable, there has to be at least one type it is callable with.

```cpp
int i = 42;
vari::vref<int, float> v = i;
v.visit([&](int&){},
        [&](std::string&){}, // error: callable does not match any type
        [&](float&){});
```

## Single-type extension

To make work with variants a bit more convenient, all allow direct access to pointed-to type in
case it is single type in the type-list:

```cpp
struct boo_t{
    int val;
};
boo_t b;
vari::vptr<boo_t> p = &b;

p->val = 42;
```

That is, in case there is only one type allowed, the signature of common methods is:
`T* vptr<T>::operator()`

Note that this makes `vref` a convenient replacement for raw references in structures:
```cpp
struct my_type{
    vref<std::string> str;
};
```
Using `std::string& str` would remove ability to assign into the structure, `vref` does not.

## Type-sets

To bring in even more convenience and capability, the template argument list of variadics is capable of flattening and filtering the types for uniqueness.

Given the following type sets:

```cpp
using set_a = vari::typelist<int, std::string>;
using set_b = vari::typelist<float, int>;
using set_s = vari::typelist<set_a, set_b, std::string>;
```

The pointer `vptr<set_s>` resolves to equivalent of `vptr<int, std::string, float>`. The flatenning/filtering mechanism only resolves `vari::typelist`, `void<std::tuple<a_t,b_t>>` would not be resolved to different form.

Why? well, suddenly one can express complex data structures. Note that the typelists also
interact well with sub-typing:

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

## Lvalue conversion from unique

To give this even more convenience, we also allow conversion of `uvptr` and `uvref` to non-unique variants if and only if the expression is lvalue reference:

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
    [&](std::integral_constant<std::size_t, 0>&){
        // `j` matches value of `v`
    },
    [&](std::integral_constant<std::size_t, 1>&){
        // `j` matches value of `v`
    },
    [&](std::integral_constant<std::size_t, 2>&){
        // `j` matches value of `v`
    });
```

## Credits

Credits for the idea for this should go to `avakar`, live long and prosper.

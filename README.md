
# Variadic library

Variadic pointers, references for C++20.

`std::variant<Ts...>` is C++20s form of tagged union which stores types by
value. This library introduces an alternative that stores items allocated
and extends the tagged union concept with type-sets, sub-typing and other forms of
API access, making it more pleasant to use.

Library provides 4 templated types as an alternative to variant:
 - `vref<B,Ts...>` - reference to any type out of `Ts...`
 - `vptr<B,Ts...>` - pointer to any type out of `Ts...` (aka: nullable reference)
 - `uvref<B,Ts...>` - owning reference to any type out of `Ts...`
 - `uvptr<B,Ts...>` - owning pointer to any type out of `Ts...`

## From std::variant to vptr or vref

Let's say that we want a function, that can accept one of two types, how to model it with `std::variant`?

```cpp

struct a_t{};
struct b_t{};

void foo(std::variant<a_t*, b_t*>);

a_t a;
foo(&a);

```

`vptr` usage is as simple:
```cpp
void foo(vari::vptr<void, a_t, b_t>);

a_t a;
foo(a);
```

But, what if we want to expres that the pointer can never be null? With variant we can use
`std::variant<std::reference_wrapper<a_t>, std::reference_wrapper<b_t>>` but that is
not elegant at all. Here, `vref` comes to the rescue:
```cpp
void foo(vari::vref<void, a_t, b_t>);
```

## Base type

First template argument of all variadics is not one of allowed types, but rather type that is pointed to internally: `vptr<B,Ts...>`
Choice of this type has two consequences:
 - Pointer to any type in list `Ts...` has to be convertible to pointer to `B` and back.
 - `B*` is type accessible common operations over pointers: `B& vptr<B,Ts...>::operator*()`

## uvptr and uvref

`u` prefixed variants imply ownership (unique), this way we can use them to
manage lifetimes of objects - in same way as `std::variant` does.

```cpp
struct a_t;
struct b_t;

vari::uvptr<void, a_t, b_t> p;

```
Note that the `uvptr` can accept forward declared struct in friendly manner.
Similar (bot not same) to `std::make_unique`, we have a friendly function to build
the unique variants: `uwrap`

```cpp
uvptr<void, std::string, int> p = uwrap<void>(std::string{"wololo"});
```
Note that signature of uwrap is `uvref<B,T> uwrap<B>(T item)`, this works as we allow natural conversion of `uvref` to `uvptr`, but not the other way around.

## Access API

Given that we do have a `vptr`, how to access it? All variants share the same approach to API, so the name of methods are the same.
All have `visit` and `match` methods. `uvptr` and `uvref` also has `take`.

### Visit

`visit` recreates approach similar to `std::visit`, except that we allow multiple callables instead of multiple variadics, note that we can handle returning from callables:
```cpp
void foo( vari::vref< void, std::vector< std::string >, std::list< std::string > > r )
{
        std::string& front = r.visit(
            [&]( std::vector< std::string >& v ) -> std::string& {
                    return v.front();
            },
            [&]( std::list< std::string >& l ) -> std::string& {
                    return l.front();
            } );
}
```
Note that in case of pointers, we opted to introduce empty branch for cases when it is null.
```cpp
void foo(vari::vptr<void, a_t, b_t> r)
{
    r.visit([&](vari::empty_t){},
            [&](a_t&){},
            [&](b_t&){});
}
```

### Match

`match` works with variadic references per se, instead of accessing items, we got variadic
reference to it:
```cpp
void foo(vari::uvptr<void, a_t, b_t> r)
{
    r.match([&](vari::empty_t){},
            [&](vari::vref<void, a_t>){},
            [&](vari::vref<void, b_t>){});
}
```
The benefit might not be obvious immediately, it becomes obvious once combined with sub-typing.

### Take

`uvref` and `uvptr` retain ownership of referenced items, `take` allows stealing the ownership
from the owner:
```cpp
void foo(vari::uvref<void, a_t, b_t> r)
{
    std::move(r).take([&](vari::uvref<void, a_t>){},
                      [&](vari::uvref<void, b_t>){});
}
```

### Concepts checks

All three methods are subjected to same sanity checking of the set of callbacks: For each type in the set, one and only one callbacks can be called.

That is, following would fail to compile due to concept check:
```cpp
void foo(vari::uvref<void, a_t, b_t> r)
{
    r.visit([&](a_t&){},
            [&](a_t&){}, // << second overload matching a_t
            [&](b_t&){});
}
```

Keep in mind, that this also affects templated arguments:
```cpp
void foo(vari::uvref<void, a_t, b_t> r)
{
    r.visit([&](a_t&){},
            [&](auto&){}); // << second overload matching a_t, but single for b_t
}
```

## Single-type extension

To make work with variant a bit more convenient, all allow direct access to pointed-to type in
case it is single type in the type-list:
```cpp
struct boo_t{
    int val;
};
boo_t b;
vari::vptr<void, boo_t> p{b};

p->val = 42;
```

That is, in case there is only one type allowed, the signature of common methods is:
`T* vptr<B, T>::operator()`
This makes `match` or `take` API calls much more convenient to use.

## Sub-typing

All variadic types support sub-typing - any variadic type can be converted to a type representing superset of types:
```cpp
a_t a;
vari::vref<void, a_t> p{a};

// allowed as {a_t, b_t} is superset of {a_t}
vari::vref<void, a_t, b_t> p2 = p;

// not allowed, as {a_t} is not superset of {a_t, b_t}
vari::vref<void, a_t> p3 = p2;
```

There are multiple ways this bring in a lot of convenience, want to have type-coherency?
```cpp
vari::uvref<void, a_t> gen_a();
vari::uvref<void, b_t> gen_b();

std::vector<vari::uvptr<void, a_t, b_t>> data = {gen_a(), gen_b(), nullptr};
```

This also interacts well if `match` or `take`:
```cpp
struct c_t;
struct d_t;

void foo(vari::vptr<void, a_t, b_t, c_t, d_t> p)
{
    p.match([&](vari::empty_t){},
            [&](vari::vref<void, a_t, b_t>){},
            [&](vari::vref<void, c_t, d_t>){});
}
```
The way we can imagine this is: `p` can represent set of 4 types, `match` splits that into four unique references, each representing one type, *sub-typing* allows merging these references together - into two subsets, each made of two types.

Note: As a side-effect of this, `vptr<void, a_t, b_t>` is naturally convertible to `vptr<void, b_t, a_t>`

## Type-sets

To bring in even more convenience and capability, the template argument list of variadics is capable of flattening and filtering the types for uniqueness. (Note that to implement this we opted for aliasing, for example: `vari::vptr` is actually alias to `vari::_vptr`)

Given the following type sets:
```cpp
using set_a = vari::typelist<a_t, b_t>;
using set_b = vari::typelist<c_t, d_t>;
using set_s = vari::typelist<set_a, set_b, d_t>;
```
The pointer `vptr<void, set_s>` actually resolves to equivalent of `vptr<void, a_t, b_t, c_t, d_t>`. The flatenning/filtering mechanism only resolves `vari::typelist`, that means that `void<void, std::tuple<a_t,b_t>>` would not be resolved to different form.

Why? well, suddenly one can express complex data structures. Note that the typelists also
interact well with sub-typing:
```cpp
using simple_types = vari::typelist<std::string, int, bool>;
using complex_types = vari::typelist<array_t, object_t>;
using json_types = vari::typelist<simple_types, complex_types>;

std::string simple_to_str(vari::vref<void, simple_types> p);

std::string to_str(vari::vref<void, json_types> p)
{
    using R = std::string;
    return p.match([&](vari::empty_t) -> R { return ""; },
                   [&](vari::vref<void, simple_types> pp) -> R { return simple_to_str(p); },
                   [&](vari::vref<void, array_t> pp) -> R { /* impl */ },
                   [&](vari::vref<void, object_t> pp) -> R { /* impl */ });
}
```

## Lvalue conversion from unique

To give this even more convenience, we also allow conversion of `uvptr` and `uvref` to non-unique variants if and only if the expression is lvalue reference:

```cpp
void foo(vari::vptr<void, a_t, b_t>);

vari::uvptr<void, a_t> p;
foo(p); // allowed, `p` is lvalue

foo(vari::uvptr<void, a_t>{}) // forbidden, rvalue used
```

## Const

All variadic types support conversion from non-const version to const version. To make this feasible, variadic types enforce that either all template arguments are const or none are:

```cpp
void foo(vari::vptr<const void, const a_t, const b_t>)

vari::vptr<void, a_t, b_t> p;
foo(p);
```

`const` is also properly propagated during typelist operations:
```cpp
using set_a = vari::typelist<a_t, b_t>;

using vp_a = vari::vptr<const void, const set_a>;
using vp_b = vari::vptr<const void, const a_t, const b_t>;
```
Both types `vp_a` and `vp_b` are equal.

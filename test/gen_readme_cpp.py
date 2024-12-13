
from enum import Enum
import sys

def load_file(file):
    with open(file, "r") as fd:
        while line := fd.readline():
            yield line

class snippet(Enum):
    START = 1
    END = 2

def find_snippets(lines):
    gen_stuff = False
    for line in lines:
        if "```cpp" in line:
            yield snippet.START
            gen_stuff = True
            continue
        elif "```" in line:
            yield snippet.END
            gen_stuff = False
            continue
        elif gen_stuff:
            if "error:" in line:
                yield "// " + line
            else:
                yield line
        else:
            yield "// " + line

def gen_cpp(lines):
    yield f"""
    #include <vari/uvptr.h>
    #include <vari/uvref.h>
    #include <vari/vptr.h>
    #include <vari/vref.h>
    #include <vari/vcast.h>
    #include <vari/dispatch.h>

    #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

    #include <doctest/doctest.h>
    #include <vector>
    #include <list>

    namespace vari{{

    TEST_CASE("readme"){{

        struct a_t{{}};
        struct b_t{{}};

    """

    for l in lines:
        if l == snippet.START:
            yield f"""
                {{
            """
        elif l == snippet.END:
            yield f"""
                }}
            """
        else:
            yield l

    yield f"""
    }}

    }}
    """

inpt_md = sys.argv[1]
outpt_cpp = sys.argv[2]

with open(outpt_cpp, "w") as fd:
    for out in gen_cpp(find_snippets(load_file(inpt_md))):
        fd.write(out)

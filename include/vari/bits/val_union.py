
import sys

file = sys.argv[1]

def gen_union(fd, n):

    tmpl_params = []
    tmpl_args = []
    items = ""
    conditions = ""
    for i in range(n):
        tmpl_params.append(f"typename T{i}")
        tmpl_args.append(f"T{i}")
        items += f"T{i} item{i};\n"
        conditions += f"if constexpr ( i == {i} )\n return s.item{i};\n"

    fd.write(f"""

template <{",".join(tmpl_params)}>
union _val_union< typelist< {",".join(tmpl_args)} > >
{{
        static constexpr index_type size = {n};

        _val_union() noexcept {{}};
        ~_val_union() {{}};

        {items}

        template < index_type i >
        constexpr static auto& get( auto& s )
        {{
                {conditions}
        }}
}};

    """)

with open(file,"r") as fd:
    lines = list(fd.readlines())


copy_text = True

with open(file, "w") as fd:
    for line in lines:
        if copy_text:
            fd.write(line)
            if line.startswith("// VARI VAL UNION GEN START"):
                copy_text = False
                for i in range(1,17):
                    gen_union(fd, i)
        else:
            if line.startswith("// VARI VAL UNION GEN END"):
                fd.write(line)
                copy_text = True

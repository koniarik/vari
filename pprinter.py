
import gdb

def get_template_arg_list(type_obj):
    """Return a type's template arguments as a list."""
    n = 0
    template_args = []
    while True:
        try:
            template_args.append(type_obj.template_argument(n))
        except:
            return template_args
        n += 1

class VPtrCorePrinter(gdb.ValuePrinter):

    def __init__(self, val):
        _, type_list = get_template_arg_list(val.type)
        alternatives = get_template_arg_list(type_list)
        self._typename = val.type
        if len(alternatives) > 1:
            self._index = int(val['index'])
        else:
            self._index = 0 if val['ptr'] == 0 else 1

        if self._index == 0 or self._index >= len(alternatives) + 1:
            self._ptr = val['ptr']
        else:
            type = alternatives[self._index-1]
            self._ptr = val['ptr'].cast(type.pointer())

    def to_string(self):
        return str(self._typename)

    def children(self):
        if self._ptr == 0:
            v = 0
        else:
            v = self._ptr.dereference()
        return [('index', self._index), ('ptr', v)]

    def display_hint(self):
        return "map"

class VPtrPrinter(gdb.ValuePrinter):

    def __init__(self, val):
        self._typename = val.type
        self._core = VPtrCorePrinter(val['_core'])

    def to_string(self):
        return str(self._typename)

    def children(self):
        return self._core.children()

    def display_hint(self):
        return self._core.display_hint()

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter(
        "vari")
    pp.add_printer('vari::_ptr_core', '^vari::_ptr_core', VPtrCorePrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_vptr', VPtrPrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_vref', VPtrPrinter)
    return pp

if __name__ == "__main__":
    gdb.printing.register_pretty_printer(
        gdb.current_objfile(),
        build_pretty_printer())

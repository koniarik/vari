
import gdb

if hasattr(gdb, 'ValuePrinter'):
    printer_base = gdb.ValuePrinter
else:
    printer_base = object

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

class VPtrCorePrinter(printer_base):

    _null_index = 2 ** 32

    def __init__(self, val):
        type_list = get_template_arg_list(val.type)[0]
        alternatives = get_template_arg_list(type_list)
        self._typename = val.type
        if len(alternatives) > 1:
            self._index = int(val['index'])
        else:
            self._index = self._null_index if val['ptr'] == 0 else 1

        if self._index == self._null_index or self._index >= len(alternatives):
            self._ptr = val['ptr']
            self._type = None
        else:
            self._type = alternatives[self._index]
            self._ptr = val['ptr'].cast(self._type.pointer())

    def to_string(self):
        return str(self._typename)

    class _iter(object):
        def __init__(self, ptr):
            self._ptr = ptr
            self._count = 0
        def __iter__(self):
            return self
        def __next__(self):
            if self._count == 0:
                self._count += 1
                if self._ptr == 0:
                    return ("empty", gdb.Value(0).cast(gdb.lookup_type("void").pointer()))
                else:
                    return (str(self._ptr.type.target()), self._ptr.dereference())
            if self._count == 1:
                raise StopIteration

    def children(self):
        return self._iter(self._ptr)

    def display_hint(self):
        return "array"

class VWrapperPrinter(printer_base):
    def __init__(self, val, core):
        self._typename = val.type
        self._core = VPtrCorePrinter(core)

    def to_string(self):
        return str(self._typename)

    def children(self):
        return self._core.children()

    def display_hint(self):
        return self._core.display_hint()

class VPtrPrinter(VWrapperPrinter):

    def __init__(self, val):
        super().__init__(val, val["_core"])

class UVPtrPrinter(VWrapperPrinter):
    def __init__(self, val):
        super().__init__(val, val["_ptr"]["_core"])

class UVRefPrinter(VWrapperPrinter):
    def __init__(self, val):
        super().__init__(val, val["_ref"]["_core"])


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter(
        "vari")
    pp.add_printer('vari::_ptr_core', '^vari::_ptr_core', VPtrCorePrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_vptr', VPtrPrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_uvptr', UVPtrPrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_vref', VPtrPrinter)
    pp.add_printer('vari::_ptr_core', '^vari::_uvref', UVRefPrinter)
    return pp

if __name__ == "__main__":
    gdb.printing.register_pretty_printer(
        gdb.current_objfile(),
        build_pretty_printer())

import ctypes
from ctypes import byref
import numpy as np
import six
# import libhdc_python as _libhdc

__all__ = ['HDC']


C_TYPES_MAP = {
    'float64': ctypes.POINTER(ctypes.c_double),
    'int32': ctypes.POINTER(ctypes.c_int32),
    'int64': ctypes.POINTER(ctypes.c_int64),
}


# ctypes
class _HDC_T(ctypes.Structure):
    """HDC C pointer (opaque)
    """
    pass


_HDC_T_P = ctypes.POINTER(_HDC_T)


libchdc = ctypes.cdll.LoadLibrary('libchdc.so')
_hdc_new_empty = libchdc.hdc_new_empty
_hdc_new_empty.restype = _HDC_T_P
# _hdc_as_int_1d = libchdc.hdc_as_int_1d
# _hdc_as_int_1d.restype = ctypes.POINTER(ctypes.c_long)
_hdc_as_voidptr = libchdc.hdc_as_voidptr
_hdc_as_voidptr.restype = ctypes.c_void_p
_hdc_get = libchdc.hdc_get
_hdc_get.restype = ctypes.POINTER(ctypes.c_int8)
_hdc_has_child = libchdc.hdc_has_child
_hdc_has_child.restype = ctypes.c_bool
_hdc_get_type = libchdc.hdc_get_type
_hdc_get_type.restype = ctypes.c_uint8
_hdc_get_slice = libchdc.hdc_get_slice
_hdc_get_slice.restype = _HDC_T_P
_hdc_get_shape = libchdc.hdc_get_shape
_hdc_get_shape.restype = ctypes.POINTER(ctypes.c_long)
_hdc_get_ndim = libchdc.hdc_get_ndim
_hdc_get_ndim.restype = ctypes.c_int8
_hdc_get_type_str = libchdc.hdc_get_type_str
_hdc_get_type_str.restype = ctypes.c_char_p
_hdc_as_string = libchdc.hdc_as_string
_hdc_as_string.restype = ctypes.c_char_p
_hdc_dumps = libchdc.hdc_dumps
_hdc_dumps.restype = ctypes.c_char_p
_hdc_childs_count = libchdc.hdc_childs_count
_hdc_childs_count.restype = ctypes.c_ulong
_hdc_keys_py = libchdc.hdc_keys_py
_hdc_keys_py.argtypes = [_HDC_T_P, ctypes.POINTER(ctypes.c_char_p)]
_hdc_serialize = libchdc.hdc_serialize
_hdc_serialize.restype = ctypes.c_char_p
_hdc_deserialize = libchdc.hdc_deserialize
_hdc_deserialize.restype = _HDC_T_P


class HDC(object):
    """HDC Python binding"""

    def __init__(self, data=None):
        super(HDC, self).__init__()
        self._c_ptr = _hdc_new_empty()
        if data is not None:
            self.set_data(data)

    def __getstate__(self):
        state = _hdc_serialize(self.c_ptr)
        return state

    def __setstate__(self, state):
        self._c_ptr = _hdc_deserialize(state)

    @classmethod
    def from_c_ptr(cls, c_ptr):
        self = object.__new__(cls)
        self._c_ptr = c_ptr
        return self

    @property
    def c_ptr(self):
        return self._c_ptr

    @c_ptr.setter
    def c_ptr(self, value):
        raise ValueError("c_ptr cannot be set")

    @c_ptr.deleter
    def c_ptr(self, value):
        raise ValueError("c_ptr cannot be deleted")

    @property
    def shape(self):
        return self.get_shape()

    def __setitem__(self, key, value):
        if isinstance(key, six.string_types):
            ckey = key.encode()
            if not _hdc_has_child(self._c_ptr, ckey):
                new_hdc = self.__class__(value)
                # TODO this is the problem - new_hdc gets lost from Python
                # we have to explain numpy not to deallocate the buffer
                libchdc.hdc_add_child(self._c_ptr, ckey, new_hdc._c_ptr)
            else:
                self[key].set_data(value)

        else:
            # key is numeric
            libchdc.hdc_set_slice(self._c_ptr, int(key), value._c_ptr)

    def __getitem__(self, key):
        if isinstance(key, six.string_types):
            ckey = key.encode()
            if not _hdc_has_child(self._c_ptr, ckey):
                raise KeyError('key not found')
            res = self.from_c_ptr(_hdc_get(self._c_ptr, ckey))
            return res
        else:
            ckey = ctypes.c_size_t(key)
            res = self.from_c_ptr(_hdc_get_slice(self._c_ptr, ckey))
            return res

    def get(self, key, default=None):
        """Similar to dict.get, does not convert to Python data type (use get_data).
        """
        if key not in self:
            return default
        else:
            return self[key]

    def get_data(self, key=None):
        """Return data as a Python type
        """
        if key is None:
            obj = self
        else:
            obj = self[key]

        type_str = self.get_type_str()
        if type_str == 'string':
            return _hdc_as_string(obj.c_ptr).decode()
        else:
            raise NotImplementedError('Type {typ} not implemented'.format(typ=type_str))

    def __contains__(self, key):
        return self.has_child(key)

    def has_child(self, key):
        return bool(_hdc_has_child(self.c_ptr, key.encode()))

    def append(self, data):
        libchdc.hdc_append_slice(self.c_ptr, data.c_ptr)

    def set_data(self, data):
        """Store data into the container
        """
        if isinstance(data, np.ndarray):
            # cdata = np.ctypeslib.as_ctypes(data)
            data = np.require(data, requirements=('C', 'O'))
            data.setflags(write=False)
            # data = np.ascontiguousarray(data)
            cshape = np.ctypeslib.as_ctypes(np.array(data.shape,
                                                     dtype=np.int64))
            cndim = ctypes.c_int8(data.ndim)
            if np.issubdtype(data.dtype, np.int8):
                cdata = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int8))
                libchdc.hdc_set_int8(self._c_ptr, cndim, byref(cshape), cdata)
            elif np.issubdtype(data.dtype, np.int32):
                cdata = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int32))
                libchdc.hdc_set_int32(self._c_ptr, cndim, byref(cshape), cdata)
            elif np.issubdtype(data.dtype, np.int64):
                cdata = data.ctypes.data_as(ctypes.POINTER(ctypes.c_int64))
                libchdc.hdc_set_int64(self._c_ptr, cndim, byref(cshape), cdata)
            elif np.issubdtype(data.dtype, np.float_):
                cdata = data.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
                libchdc.hdc_set_double(self._c_ptr, cndim, byref(cshape), cdata)
        elif isinstance(data, six.string_types):
            cdata = ctypes.c_char_p(data.encode())
            libchdc.hdc_set_string(self._c_ptr, cdata)
        elif isinstance(data, self.__class__):
            cdata = data.c_ptr
            libchdc.hdc_set_child(self.c_ptr, ctypes.c_char_p(''.encode()), cdata)
        else:
            raise NotImplementedError("{typ} not supported in HDC.set_data".format(typ=type(data)))

    def get_type_str(self):
        return _hdc_get_type_str(self._c_ptr).decode()

    def get_type(self):
        return _hdc_get_type(self._c_ptr)

    def get_shape(self):
        ndim = _hdc_get_ndim(self._c_ptr)
        cshape = _hdc_get_shape(self._c_ptr)
        shape = tuple((cshape[i] for i in range(ndim)))
        return shape

    def tolist(self):
        type_str = self.get_type_str()
        if type_str == 'hdc':
            res = [_hdc_get_slice(self._c_ptr, i) for i in range(self.shape[0])]
        else:
            res = self.asarray().tolist()

        return res

    def asarray(self):
        """Convert to a numpy array, sharing numerical data
        """
        type_str = self.get_type_str()
        if type_str == 'hdc':
            return np.array(self.tolist())
        ctype = C_TYPES_MAP.get(type_str, None)
        if ctype is None:
            raise ValueError('Cannot convert {} to numpy array'.format(type_str))
        c_void_ptr = _hdc_as_voidptr(self._c_ptr)
        cdata = ctypes.cast(c_void_ptr, ctype)
        res = np.ctypeslib.as_array(cdata, self.get_shape())

        return res

    def __array__(self):
        return self.asarray()

    def dump(self):
        """Dump the continer
        """
        libchdc.hdc_dump(self.c_ptr)

    def dumps(self):
        """Dump the continer
        """
        return _hdc_dumps(self.c_ptr).decode()

    def json_dump(self, filename, mode=0):
        """Save to json file
        """
        libchdc.hdc_to_json(self.c_ptr, filename.encode(), mode)

    def keys(self):
        """Get access keys of containers' children
        """
        n = _hdc_childs_count(self.c_ptr)
        string_buffers = [ctypes.create_string_buffer(1000) for i in range(n)]
        _keys = (ctypes.c_char_p * 4)(*map(ctypes.addressof, string_buffers))
        _hdc_keys_py(ctypes.cast(self.c_ptr, _HDC_T_P), _keys)
        keys_lst = []
        for i in range(n):
            keys_lst.append(_keys[i].decode())
        return keys_lst

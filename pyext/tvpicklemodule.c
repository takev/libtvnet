#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <tvpickle/tvpickle.h>

static PyObject *pytvp_error;

static Py_ssize_t pytvp_length_recurse_bytearray(Py_ssize_t size)
{
    return tvp_len_binary_string(size);
}

static Py_ssize_t pytvp_length_recurse(PyObject *o)
{
    Py_ssize_t  length;
    Py_ssize_t  rec_length;
    Py_ssize_t  item_nr;
    Py_ssize_t  nr_items;
    PyObject    *item;
    PyObject    *key;
    PyObject    *value;
    PyObject    *iterator;
    PyObject    *utf8_string;

    if (PyByteArray_Check(o)) {
        length = tvp_len_binary_string(PyByteArray_GET_SIZE(o));

    } else if (PyString_Check(o))  {
        length = tvp_len_binary_string(PyString_GET_SIZE(o));

    } else if (PyUnicode_Check(o)) {
        if ((utf8_string = PyUnicode_AsUTF8String(o)) == NULL) {
            return -1;
        }
        length = tvp_len_utf8_string(PyString_GET_SIZE(utf8_string));
        Py_DECREF(utf8_string);

    } else if (PyInt_Check(o)) {
        length = tvp_len_integer(PyInt_AS_LONG(o));

    } else if (PyFloat_Check(o)) {
        length = tvp_len_float(PyFloat_AS_DOUBLE(o));

    } else if (o == Py_None) {
        length = tvp_len_null();

    } else if (PyMapping_Check(o)) {
        nr_items = PyMapping_Size(o);
        iterator = PyObject_GetIter(o);

        length = tvp_len_dictionary(nr_items);
        while ((key = PyIter_Next(iterator)) != NULL) {
            value = PyObject_GetItem(o, key);

            if ((rec_length = pytvp_length_recurse(key)) == -1) {
                return -1;
            }
            length+= rec_length;
            if ((rec_length = pytvp_length_recurse(value)) == -1) {
                return -1;
            }
            length+= rec_length;
            Py_DECREF(value);
            Py_DECREF(key);
        }

    } else if (PySequence_Check(o)) {
        nr_items = PySequence_Length(o);

        length = tvp_len_list(nr_items);
        for (item_nr = 0; item_nr < nr_items; item_nr++) {
            item  = PySequence_Fast_GET_ITEM(o, item_nr);
            if ((rec_length = pytvp_length_recurse(item)) == -1) {
                return -1;
            }
            length += rec_length;
        }

    } else if (PyAnySet_Check(o)) {
        nr_items = PySet_Size(o);
        iterator = PyObject_GetIter(o);

        length = tvp_len_list(nr_items);
        while ((item = PyIter_Next(iterator)) != NULL) {
            if ((rec_length = pytvp_length_recurse(item)) == -1) {
                return -1;
            }
            length += rec_length;
            Py_DECREF(item);
        }

    } else {
        PyErr_SetString(pytvp_error, "Unexpected type.");
        return -1;
    }

    return length;
}

static void pytvp_encode_recurse(tvu_buffer_t *buffer, PyObject *o)
{
    Py_ssize_t  item_nr;
    Py_ssize_t  nr_items;
    PyObject    *item;
    PyObject    *key;
    PyObject    *value;
    PyObject    *iterator;
    PyObject    *utf8_string;

    if (PyByteArray_Check(o)) {
        tvp_enc_binary_string(buffer, (uint8_t *)PyByteArray_AS_STRING(o), PyByteArray_GET_SIZE(o));

    } else if (PyString_Check(o))  {
        tvp_enc_binary_string(buffer, (uint8_t *)PyString_AS_STRING(o), PyString_GET_SIZE(o));

    } else if (PyUnicode_Check(o)) {
        utf8_string = PyUnicode_AsUTF8String(o);
        tvp_enc_utf8_string_and_size(buffer, PyString_AS_STRING(utf8_string), PyString_GET_SIZE(utf8_string));
        Py_DECREF(utf8_string);

    } else if (PyInt_Check(o)) {
        tvp_enc_integer(buffer, PyInt_AS_LONG(o));

    } else if (PyFloat_Check(o)) {
        tvp_enc_float(buffer, PyFloat_AS_DOUBLE(o));

    } else if (o == Py_None) {
        tvp_enc_null(buffer);

    } else if (PyMapping_Check(o)) {
        nr_items = PyMapping_Size(o);
        iterator = PyObject_GetIter(o);

        tvp_enc_dictionary(buffer, nr_items);
        while ((key = PyIter_Next(iterator)) != NULL) {
            value = PyObject_GetItem(o, key);

            pytvp_encode_recurse(buffer, key);
            pytvp_encode_recurse(buffer, value);
            Py_DECREF(value);
            Py_DECREF(key);
        }

    } else if (PySequence_Check(o)) {
        Py_ssize_t nr_objects = PySequence_Length(o);

        tvp_enc_list(buffer, nr_objects);
        for (Py_ssize_t object_nr = 0; object_nr < nr_objects; object_nr++) {
            PyObject *object  = PySequence_Fast_GET_ITEM(o, object_nr);

            pytvp_encode_recurse(buffer, object);
        }

    } else if (PyAnySet_Check(o)) {
        nr_items = PySet_Size(o);
        iterator = PyObject_GetIter(o);

        tvp_enc_list(buffer, nr_items);
        while ((item = PyIter_Next(iterator)) != NULL) {
            pytvp_encode_recurse(buffer, item);
            Py_DECREF(item);
        }

    } else {
        PyErr_SetString(pytvp_error, "Unexpected type.");
    }
}

static PyObject *pytvp_decode_recurse(tvu_buffer_t *buffer)
{
    tvp_token_t token = tvp_dec_token(buffer);
    Py_ssize_t  item_nr;
    PyObject    *list;
    PyObject    *dictionary;
    PyObject    *object;
    PyObject    *key;
    PyObject    *value;

    switch (token.type) {
    case TVP_TOKEN_END:
        return (PyObject *)-1;
    case TVP_TOKEN_ERROR:
        PyErr_SetString(pytvp_error, "Running over bound during decoding.");
        return NULL;
    case TVP_TOKEN_NULL:
        Py_INCREF(Py_None);
        return Py_None;
    case TVP_TOKEN_INTEGER:
        return PyInt_FromLong(token.value.i);
    case TVP_TOKEN_FLOAT:
        return PyFloat_FromDouble(token.value.f);
    case TVP_TOKEN_UTF8_STRING:
        return PyUnicode_DecodeUTF8((char *)token.data, token.value.u, NULL);
    case TVP_TOKEN_BIN_STRING:
        return PyString_FromStringAndSize((char *)token.data, token.value.u);
    case TVP_TOKEN_LIST:
        list = PyList_New(token.value.u);
        for (item_nr = 0; item_nr < token.value.u; item_nr++) {
            if ((object = pytvp_decode_recurse(buffer)) == NULL) {
                return NULL;
            }
            PyList_SET_ITEM(list, item_nr, object);
        }
        return list;
    case TVP_TOKEN_DICTIONARY:
        dictionary = PyDict_New();
        for (item_nr = 0; item_nr < token.value.u; item_nr++) {
            if ((key = pytvp_decode_recurse(buffer)) == NULL) {
                return NULL;
            }
            if ((value = pytvp_decode_recurse(buffer)) == NULL) {
                return NULL;
            }
            if (PyDict_SetItem(dictionary, key, value) == -1) {
                return NULL;
            }
            Py_DECREF(key);
            Py_DECREF(value);
        }
        return dictionary;
    default:
        PyErr_SetString(pytvp_error, "Unexpected token type.");
        return NULL;
    }
}


static Py_ssize_t pytvp_length_tuple(PyObject *args)
{
    Py_ssize_t length = 0;
    Py_ssize_t nr_objects;
    Py_ssize_t object_nr;
    Py_ssize_t object_length;

    if (!PySequence_Check(args)) {
        PyErr_SetString(pytvp_error, "Expecting a sequence as argument");
        return -1;
    }

    nr_objects = PySequence_Length(args);
    for (object_nr = 0; object_nr < nr_objects; object_nr++) {
        if ((object_length = pytvp_length_recurse(PySequence_Fast_GET_ITEM(args, object_nr))) == -1) {
            return -1;
        }
        length+= object_length;
    }

    return length;
}

static void pytvp_encode_tuple(tvu_buffer_t *buffer, PyObject *args)
{
    Py_ssize_t nr_objects;
    Py_ssize_t object_nr;

    nr_objects = PySequence_Length(args);
    for (object_nr = 0; object_nr < nr_objects; object_nr++) {
        pytvp_encode_recurse(buffer, PySequence_Fast_GET_ITEM(args, object_nr));
    }
}

static PyObject *pytvp_decode_tuple(tvu_buffer_t *buffer)
{
    PyObject    *list = PyList_New(0);
    PyObject    *object;
    tvp_token_t token;

    while (1) {
        if ((object = pytvp_decode_recurse(buffer)) == NULL) {
            return NULL;
        }
        if (object == (PyObject *)-1) {
            break;
        }
        PyList_Append(list, object);
    }

    return list;
}

static PyObject *pytvp_encode(PyObject *self __attribute__((unused)), PyObject *args)
{
    Py_ssize_t      length;
    PyObject        *byte_array;
    tvu_buffer_t    buffer;
    
    if ((length = pytvp_length_tuple(args)) == -1) {
        return NULL;
    }

    // Allocate a string without initializing it first then the pointer inside it for filling it in.
    if ((byte_array = PyString_FromStringAndSize(NULL, length)) == NULL) {
        return NULL;
    }
    buffer.data = (uint8_t *)PyString_AS_STRING(byte_array);
    buffer.offset = 0;

    pytvp_encode_tuple(&buffer, args);
    return byte_array;
}

static PyObject *pytvp_decode(PyObject *self __attribute__((unused)), PyObject *args)
{
    PyObject        *list;
    PyObject        *tuple;
    PyObject        *object;
    tvu_buffer_t    buffer;
    char            *data;
    Py_ssize_t      size;

    if (!PyArg_ParseTuple(args, "s#", &data, &size)) {
        return NULL;
    }
    buffer.size = size;
    buffer.data = (uint8_t *)data;
    buffer.offset = 0;
    
    if ((list = pytvp_decode_tuple(&buffer)) == NULL) {
        return NULL;
    }

    if (PySequence_Size(list) == 1) {
        // Convert the list to a single item.
        object = PySequence_GetItem(list, 0);
        Py_DECREF(list);
        return object;

    } else {
        // Convert the list to a tuple.
        tuple = PyList_AsTuple(list);
        Py_DECREF(list);
        return tuple;
    }
}

static PyMethodDef pytvp_methods[] = {
    {"encode", pytvp_encode, METH_VARARGS, "Encode python objects passed as argument."},
    {"decode", pytvp_decode, METH_VARARGS, "Decode bytes into python objects."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC inittvpickle(void)
{
    PyObject *m;

    if ((m = Py_InitModule("tvpickle", pytvp_methods)) == NULL) {
        return;
    }

    pytvp_error = PyErr_NewException("pytvp.error", NULL, NULL);
    Py_INCREF(pytvp_error);
    PyModule_AddObject(m, "error", pytvp_error);
}

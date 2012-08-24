#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <tvutils/tvutils.h>
#include <tvnet/tvnet.h>

static PyObject *pytvn_error;

static Py_ssize_t pytvn_length_recurse_bytearray(Py_ssize_t size)
{
    return tvnp_length_binary_string(size);
}

static Py_ssize_t pytvn_length_recurse(PyObject *o)
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
        length = tvnp_length_binary_string(PyByteArray_GET_SIZE(o));

    } else if (PyString_Check(o))  {
        length = tvnp_length_binary_string(PyString_GET_SIZE(o));

    } else if (PyUnicode_Check(o)) {
        if ((utf8_string = PyUnicode_AsUTF8String(o)) == NULL) {
            return -1;
        }
        length = tvnp_length_utf8_string(PyString_GET_SIZE(utf8_string));
        Py_DECREF(utf8_string);

    } else if (PyInt_Check(o)) {
        length = tvnp_length_integer(PyInt_AS_LONG(o));

    } else if (PyFloat_Check(o)) {
        length = tvnp_length_float(PyFloat_AS_DOUBLE(o));

    } else if (o == Py_None) {
        length = tvnp_length_null();

    } else if (PyMapping_Check(o)) {
        nr_items = PyMapping_Size(o);
        iterator = PyObject_GetIter(o);

        length = tvnp_length_dictionary(nr_items);
        while ((key = PyIter_Next(iterator)) != NULL) {
            value = PyObject_GetItem(o, key);

            if ((rec_length = pytvn_length_recurse(key)) == -1) {
                return -1;
            }
            length+= rec_length;
            if ((rec_length = pytvn_length_recurse(value)) == -1) {
                return -1;
            }
            length+= rec_length;
            Py_DECREF(value);
            Py_DECREF(key);
        }

    } else if (PySequence_Check(o)) {
        nr_items = PySequence_Length(o);

        length = tvnp_length_list(nr_items);
        for (item_nr = 0; item_nr < nr_items; item_nr++) {
            item  = PySequence_Fast_GET_ITEM(o, item_nr);
            if ((rec_length = pytvn_length_recurse(item)) == -1) {
                return -1;
            }
            length += rec_length;
        }

    } else if (PyAnySet_Check(o)) {
        nr_items = PySet_Size(o);
        iterator = PyObject_GetIter(o);

        length = tvnp_length_list(nr_items);
        while ((item = PyIter_Next(iterator)) != NULL) {
            if ((rec_length = pytvn_length_recurse(item)) == -1) {
                return -1;
            }
            length += rec_length;
            Py_DECREF(item);
        }

    } else {
        PyErr_SetString(pytvn_error, "Unexpected type.");
        return -1;
    }

    return length;
}

static void pytvn_encode_recurse(tvu_buffer_t *buffer, PyObject *o)
{
    Py_ssize_t  item_nr;
    Py_ssize_t  nr_items;
    PyObject    *item;
    PyObject    *key;
    PyObject    *value;
    PyObject    *iterator;
    PyObject    *utf8_string;

    if (PyByteArray_Check(o)) {
        tvnp_encode_binary_string(buffer, (uint8_t *)PyByteArray_AS_STRING(o), PyByteArray_GET_SIZE(o));

    } else if (PyString_Check(o))  {
        tvnp_encode_binary_string(buffer, (uint8_t *)PyString_AS_STRING(o), PyString_GET_SIZE(o));

    } else if (PyUnicode_Check(o)) {
        utf8_string = PyUnicode_AsUTF8String(o);
        tvnp_encode_utf8_string_and_size(buffer, PyString_AS_STRING(utf8_string), PyString_GET_SIZE(utf8_string));
        Py_DECREF(utf8_string);

    } else if (PyInt_Check(o)) {
        tvnp_encode_integer(buffer, PyInt_AS_LONG(o));

    } else if (PyFloat_Check(o)) {
        tvnp_encode_float(buffer, PyFloat_AS_DOUBLE(o));

    } else if (o == Py_None) {
        tvnp_encode_null(buffer);

    } else if (PyMapping_Check(o)) {
        nr_items = PyMapping_Size(o);
        iterator = PyObject_GetIter(o);

        tvnp_encode_dictionary(buffer, nr_items);
        while ((key = PyIter_Next(iterator)) != NULL) {
            value = PyObject_GetItem(o, key);

            pytvn_encode_recurse(buffer, key);
            pytvn_encode_recurse(buffer, value);
            Py_DECREF(value);
            Py_DECREF(key);
        }

    } else if (PySequence_Check(o)) {
        Py_ssize_t nr_objects = PySequence_Length(o);

        tvnp_encode_list(buffer, nr_objects);
        for (Py_ssize_t object_nr = 0; object_nr < nr_objects; object_nr++) {
            PyObject *object  = PySequence_Fast_GET_ITEM(o, object_nr);

            pytvn_encode_recurse(buffer, object);
        }

    } else if (PyAnySet_Check(o)) {
        nr_items = PySet_Size(o);
        iterator = PyObject_GetIter(o);

        tvnp_encode_list(buffer, nr_items);
        while ((item = PyIter_Next(iterator)) != NULL) {
            pytvn_encode_recurse(buffer, item);
            Py_DECREF(item);
        }

    } else {
        PyErr_SetString(pytvn_error, "Unexpected type.");
    }
}

static PyObject *pytvn_decode_recurse(tvu_buffer_t *buffer)
{
    tvnp_token_t token = tvnp_decode_token(buffer);
    Py_ssize_t  item_nr;
    PyObject    *list;
    PyObject    *dictionary;
    PyObject    *object;
    PyObject    *key;
    PyObject    *value;

    switch (token.type) {
    case TVNP_TOKEN_END:
        return (PyObject *)-1;
    case TVNP_TOKEN_ERROR:
        PyErr_SetString(pytvn_error, "Running over bound during decoding.");
        return NULL;
    case TVNP_TOKEN_NULL:
        Py_RETURN_NONE;
    case TVNP_TOKEN_INTEGER:
        return PyInt_FromLong(token.value.i);
    case TVNP_TOKEN_FLOAT:
        return PyFloat_FromDouble(token.value.f);
    case TVNP_TOKEN_UTF8_STRING:
        return PyUnicode_DecodeUTF8((char *)token.data, token.value.u, NULL);
    case TVNP_TOKEN_BIN_STRING:
        return PyString_FromStringAndSize((char *)token.data, token.value.u);
    case TVNP_TOKEN_LIST:
        list = PyList_New(token.value.u);
        for (item_nr = 0; item_nr < token.value.u; item_nr++) {
            if ((object = pytvn_decode_recurse(buffer)) == NULL) {
                return NULL;
            }
            PyList_SET_ITEM(list, item_nr, object);
        }
        return list;
    case TVNP_TOKEN_DICTIONARY:
        dictionary = PyDict_New();
        for (item_nr = 0; item_nr < token.value.u; item_nr++) {
            if ((key = pytvn_decode_recurse(buffer)) == NULL) {
                return NULL;
            }
            if ((value = pytvn_decode_recurse(buffer)) == NULL) {
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
        PyErr_SetString(pytvn_error, "Unexpected token type.");
        return NULL;
    }
}


static Py_ssize_t pytvn_length_tuple(PyObject *args)
{
    Py_ssize_t length = 0;
    Py_ssize_t nr_objects;
    Py_ssize_t object_nr;
    Py_ssize_t object_length;

    if (!PySequence_Check(args)) {
        PyErr_SetString(pytvn_error, "Expecting a sequence as argument");
        return -1;
    }

    nr_objects = PySequence_Length(args);
    for (object_nr = 0; object_nr < nr_objects; object_nr++) {
        if ((object_length = pytvn_length_recurse(PySequence_Fast_GET_ITEM(args, object_nr))) == -1) {
            return -1;
        }
        length+= object_length;
    }

    return length;
}

static void pytvn_encode_tuple(tvu_buffer_t *buffer, PyObject *args)
{
    Py_ssize_t nr_objects;
    Py_ssize_t object_nr;

    nr_objects = PySequence_Length(args);
    for (object_nr = 0; object_nr < nr_objects; object_nr++) {
        pytvn_encode_recurse(buffer, PySequence_Fast_GET_ITEM(args, object_nr));
    }
}

static PyObject *pytvn_decode_tuple(tvu_buffer_t *buffer)
{
    PyObject    *list = PyList_New(0);
    PyObject    *object;
    tvnp_token_t token;

    while (1) {
        if ((object = pytvn_decode_recurse(buffer)) == NULL) {
            return NULL;
        }
        if (object == (PyObject *)-1) {
            break;
        }
        PyList_Append(list, object);
    }

    return list;
}

static PyObject *pytvn_init(PyObject *self __attribute__((unused)), PyObject *args)
{
    utf8_t  *argv0;

    if (!PyArg_ParseTuple(args, "s:init", &argv0)) {
        return NULL;
    }

    tvu_init(argv0);

    Py_RETURN_NONE;    
}

static PyObject *pytvn_encode(PyObject *self __attribute__((unused)), PyObject *args)
{
    Py_ssize_t      length;
    PyObject        *byte_array;
    tvu_buffer_t    buffer;
    
    if ((length = pytvn_length_tuple(args)) == -1) {
        return NULL;
    }

    // Allocate a string without initializing it first then the pointer inside it for filling it in.
    if ((byte_array = PyString_FromStringAndSize(NULL, length)) == NULL) {
        return NULL;
    }
    buffer.data = (uint8_t *)PyString_AS_STRING(byte_array);
    buffer.offset = 0;

    pytvn_encode_tuple(&buffer, args);
    return byte_array;
}

static PyObject *pytvn_decode(PyObject *self __attribute__((unused)), PyObject *args)
{
    PyObject        *list;
    PyObject        *tuple;
    PyObject        *object;
    tvu_buffer_t    buffer;
    char            *data;
    Py_ssize_t      size;

    if (!PyArg_ParseTuple(args, "s#:decode", &data, &size)) {
        return NULL;
    }
    buffer.size = size;
    buffer.data = (uint8_t *)data;
    buffer.offset = 0;
    
    if ((list = pytvn_decode_tuple(&buffer)) == NULL) {
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

static PyObject *pytvn_socket(PyObject *self __attribute__((unused)), PyObject *args)
{
    int fd;

    if (!PyArg_ParseTuple(args, ":socket")) {
        return NULL;
    }

    if ((fd = tvns_socket()) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    return PyInt_FromLong(fd);
}

static PyObject *pytvn_bind(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    long long   service;

    if (!PyArg_ParseTuple(args, "iL:bind", &fd, &service)) {
        return NULL;
    }

    if (tvns_bind(fd, service) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pytvn_unbind(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    long long   service;

    if (!PyArg_ParseTuple(args, "iL:unbind", &fd, &service)) {
        return NULL;
    }

    if (tvns_unbind(fd, service) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pytvn_client_bind(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    long long   service;

    if (!PyArg_ParseTuple(args, "i:client_bind", &fd)) {
        return NULL;
    }

    if (tvns_client_bind(fd, &service) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    return PyLong_FromLongLong(service);
}

static PyObject *pytvn_sendto(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    long long   service;
    void        *buffer;
    Py_ssize_t  buffer_size;
    int         flags = 0;
    tvu_int     r;

    if (!PyArg_ParseTuple(args, "iLs#|i:sendto", &fd, &service, &buffer, &buffer_size, &flags)) {
        return NULL;
    }

    if ((r = tvns_sendto(fd, service, buffer, buffer_size, flags)) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    return PyLong_FromLongLong(r);
}

static PyObject *pytvn_recvfrom(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    tvu_int     service;
    char        buffer[65536];
    size_t      buffer_size = 65536;
    int         flags = 0;
    tvu_int     r;
    PyObject    *t;

    if (!PyArg_ParseTuple(args, "i|i:recvfrom", &fd, &flags)) {
        return NULL;
    }

    if ((r = tvns_recvfrom(fd, &service, buffer, buffer_size, flags)) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    t = PyTuple_New(2);
    PyTuple_SET_ITEM(t, 0, PyString_FromStringAndSize(buffer, r));
    PyTuple_SET_ITEM(t, 1, PyLong_FromLongLong(service));
    return t;
}

static PyObject *pytvn_close(PyObject *self __attribute__((unused)), PyObject *args)
{
    int         fd;
    long long   service;

    if (!PyArg_ParseTuple(args, "iL:close", &fd, &service)) {
        return NULL;
    }

    if (tvns_close(fd, service) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *pytvn_cuid(PyObject *self __attribute__((unused)), PyObject *args)
{
    tvn_cuid_t id;

    if (!PyArg_ParseTuple(args, ":cuid")) {
        return NULL;
    }

    if ((id = tvn_cuid()) == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    return PyLong_FromLongLong(id);
}

static PyMethodDef pytvn_methods[] = {
    {"tvu_init", pytvn_init, METH_VARARGS, "Initialize tvutils."},
    {"socket", pytvn_socket, METH_VARARGS, "Create a socket for a service."},
    {"bind", pytvn_bind, METH_VARARGS, "Bind a socket for a service."},
    {"unbind", pytvn_unbind, METH_VARARGS, "Unbind a socket for a service."},
    {"client_bind", pytvn_client_bind, METH_VARARGS, "Unbind a socket for a service."},
    {"sendto", pytvn_sendto, METH_VARARGS, "Send a message to a service, or back to the client."},
    {"recvfrom", pytvn_recvfrom, METH_VARARGS, "Receive a message from a client, or back from the service."},
    {"close", pytvn_close, METH_VARARGS, "Close socket."},
    {"cuid", pytvn_cuid, METH_VARARGS, "Get unique identifier."},
    {"encode", pytvn_encode, METH_VARARGS, "Encode python objects passed as argument."},
    {"decode", pytvn_decode, METH_VARARGS, "Decode bytes into python objects."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC inittvnet(void)
{
    PyObject *m;

    if ((m = Py_InitModule("tvnet", pytvn_methods)) == NULL) {
        return;
    }

    pytvn_error = PyErr_NewException("pytvn.error", NULL, NULL);
    Py_INCREF(pytvn_error);
    PyModule_AddObject(m, "error", pytvn_error);
}

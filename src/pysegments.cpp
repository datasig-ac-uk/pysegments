
#include "pysegments.h"


extern "C" {

    static PyObject *PyInterval_new(PyTypeObject *, PyObject *, PyObject *);
    static int PyInterval_init(PyObject *, PyObject *, PyObject *);
    static PyObject *PyInterval_repr(PyObject *);
    static PyObject *PyInterval_str(PyObject *);

    static PyObject *PyInterval_excluded_end(PyObject *);
    static PyObject *PyInterval_included_end(PyObject *);
    static PyObject *PyInterval_sup(PyObject *);
    static PyObject *PyInterval_inf(PyObject *);

    static PyObject *PyInterval_contains(PyObject *, PyObject *);
    static PyObject *PyInterval_is_associated(PyObject *, PyObject *);


    static PyObject *PyDyadicInterval_new(PyTypeObject *, PyObject *, PyObject *);
    static PyObject *PyDyadicInterval_repr(PyObject *);
    static PyObject *PyDyadicInterval_str(PyObject *);

    static PyObject *PyDyadicInterval_excluded_end(PyObject *);
    static PyObject *PyDyadicInterval_included_end(PyObject *);
    static PyObject *PyDyadicInterval_sup(PyObject *);
    static PyObject *PyDyadicInterval_inf(PyObject *);

    static PyObject *PyDyadicInterval_flip_interval(PyObject *);
    //static PyObject *PyDyadicInterval_shift_back(PyObject *, PyObject *);
    //static PyObject *PyDyadicInterval_shift_forward(PyObject *, PyObject *);
    static PyObject *PyDyadicInterval_shrink_to_contained_end(PyObject *, PyObject *);
    static PyObject *PyDyadicInterval_shrink_to_omitted_end(PyObject *);
    static PyObject *PyDyadicInterval_dyadic_bracket(PyObject *, PyObject *, PyObject *);

    static PyObject *PyDyadicInterval_shrink_left(PyObject *, PyObject *);
    static PyObject *PyDyadicInterval_shrink_right(PyObject *, PyObject *);
    static PyObject *PyDyadicInterval_expand_interval(PyObject *, PyObject *);

    static PyObject *PyDyadicInterval_aligned(PyObject *);
    static PyObject *PyDyadicInterval_richcompare(PyObject *, PyObject *, int);
    static PyObject *PyDyadicInterval_contains(PyObject *, PyObject *);


    static PyObject *Py_to_dyadic_intervals(PyObject *, PyObject *, PyObject *);

    static PyObject *Py_segment(PyObject *, PyObject *, PyObject *);

}




// Interval type

static PyMethodDef PyInterval_methods[] = {
    {
        "contains",
        (PyCFunction) PyInterval_contains,
        METH_O,
        "Test if an interval contains another interval or float."
    },
    {
        "is_associated",
        (PyCFunction) PyInterval_is_associated,
        METH_NOARGS,
        "Test if the interval is associated to another interval."
    },
    {
        "included_end",
        (PyCFunction) PyInterval_included_end,
        METH_NOARGS,
        "Get the included end of the interval."
    },
    {
        "excluded_end",
        (PyCFunction) PyInterval_excluded_end,
        METH_NOARGS,
        "Get the included end of the interval."
    },
    {
        "inf",
        (PyCFunction) PyInterval_inf,
        METH_NOARGS,
        "Get the infimum of the interval."
    },
    {
        "sup",
        (PyCFunction) PyInterval_sup,
        METH_NOARGS,
        "Get the supremum of the interval."
    },
    {nullptr}
};


static PyTypeObject PyIntervalType = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "_segments.Interval",
    .tp_basicsize = sizeof(PyInterval),
    .tp_itemsize = 0,
    .tp_repr = (reprfunc) PyInterval_repr,
    .tp_str = (reprfunc) PyInterval_str,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Clopen interval",
    .tp_methods = PyInterval_methods,
    .tp_new = PyInterval_new,
};


PyObject *
interval_to_pyinterval(const segment::interval& interval)
{
    PyInterval *self = (PyInterval *) PyIntervalType.tp_alloc(&PyIntervalType, 0);
    if (self == nullptr) {
        return nullptr;
    }
    self->intrvl = segment::interval {interval};
    return (PyObject *) self;
}


static PyObject *
PyInterval_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{

    static const char *kwlist[] = {"a", "b", nullptr};
    double a, b;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", 
                                     const_cast<char**>(kwlist),
                                     &a, &b))
        return nullptr;

    PyInterval *self = (PyInterval *) type->tp_alloc(type, 0);
    self->intrvl = segment::interval {std::make_pair(a, b)};

    return (PyObject *) self;
}


static PyObject *
PyInterval_repr(PyObject *self)
{
    PyInterval *_self = (PyInterval *) self;
    std::string left_s, right_s;

    left_s = std::to_string(_self->intrvl.inf());
    right_s = std::to_string(_self->intrvl.sup());

    return PyUnicode_FromFormat("Interval(%s, %s)",
                                left_s.c_str(),
                                right_s.c_str());
}

static PyObject *
PyInterval_str(PyObject *self)
{
    PyInterval *_self = (PyInterval *) self;
    std::string left_s, right_s;

    left_s = std::to_string(_self->intrvl.inf());
    right_s = std::to_string(_self->intrvl.sup());

    return PyUnicode_FromFormat("[%s, %s)",
                                left_s.c_str(),
                                right_s.c_str());
}

static PyObject *
PyInterval_excluded_end(PyObject *self)
{
    double ans = ((PyInterval *) self)->intrvl.excluded_end();
    return PyFloat_FromDouble(ans);
}

static PyObject *
PyInterval_included_end(PyObject *self)
{
    double ans = ((PyInterval *) self)->intrvl.included_end();
    return PyFloat_FromDouble(ans);
}

static PyObject *
PyInterval_sup(PyObject *self)
{
    double ans = ((PyInterval *) self)->intrvl.sup();
    return PyFloat_FromDouble(ans);
}

static PyObject *
PyInterval_inf(PyObject *self)
{
    double ans = ((PyInterval *) self)->intrvl.inf();
    return PyFloat_FromDouble(ans);
}


// Dyadic Interval type



static PyMethodDef PyDyadicInterval_methods[] = {
    {
        "excluded_end", 
        (PyCFunction) PyDyadicInterval_excluded_end,
        METH_NOARGS,
        "Get excluded endpoint for the interval."
    },
    {
        "included_end", 
        (PyCFunction) PyDyadicInterval_included_end,
        METH_NOARGS,
        "Get included endpoint for the interval."
    },
    {
        "sup", 
        (PyCFunction) PyDyadicInterval_sup,
        METH_NOARGS,
        "Get the supremum of the interval."
    },
    {
        "inf", 
        (PyCFunction) PyDyadicInterval_inf,
        METH_NOARGS,
        "Get the infimum of the interval."
    },
    {
        "flip_interval", 
        (PyCFunction) PyDyadicInterval_flip_interval,
        METH_NOARGS,
        "Flip the interval."
    },
    //{
    //    "shift_back", 
    //    (PyCFunction) PyDyadicInterval_shift_back,
    //    METH_O,
    //    "Shift the interval back by arg."
    //},
    //{
    //    "shift_forward", 
    //    (PyCFunction) PyDyadicInterval_shift_forward,
    //    METH_O,
    //    "Shift the interval forward by arg."
    //},
    {
        "shrink_to_contained_end", 
        (PyCFunction) PyDyadicInterval_shrink_to_contained_end,
        METH_O,
        "Shrink the interval towards the contained end."
    },
    {
        "shrink_to_omitted_end", 
        (PyCFunction) PyDyadicInterval_shrink_to_omitted_end,
        METH_NOARGS,
        "Shrink the interval towards the omitted end."
    },
    {
        "dyadic_bracket", 
        (PyCFunction) PyDyadicInterval_dyadic_bracket,
        METH_CLASS | METH_VARARGS | METH_KEYWORDS,
        "Get the dyadic bracket for a number with given precision."
    },
    {
        "shrink_left", 
        (PyCFunction) PyDyadicInterval_shrink_left,
        METH_O,
        "Shrink the interval to the left."
    },
    {
        "shrink_right", 
        (PyCFunction) PyDyadicInterval_shrink_right,
        METH_O,
        "Get excluded endpoint for the interval."
    },
    {
        "expand_interval", 
        (PyCFunction) PyDyadicInterval_expand_interval,
        METH_O,
        "Expand the interval."
    },
    {
        "aligned", 
        (PyCFunction) PyDyadicInterval_aligned,
        METH_NOARGS,
        "Check if the interval is aligned with its parent."
    },
    {
        "contains", 
        (PyCFunction) PyDyadicInterval_contains,
        METH_O,
        "Check if the interval contains another interval."
    },
    {nullptr}
};



static PyTypeObject PyDyadicIntervalType = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "_segments.DyadicInterval",
    .tp_basicsize = sizeof(PyDyadicInterval),
    .tp_itemsize = 0,
    .tp_repr = (reprfunc) PyDyadicInterval_repr,
    .tp_str = (reprfunc) PyDyadicInterval_str,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Clopen Dyadic interval",
    .tp_richcompare = (richcmpfunc) PyDyadicInterval_richcompare,
    .tp_methods = PyDyadicInterval_methods,
    .tp_new = PyDyadicInterval_new,
};

PyDyadicInterval *
build_pydyadicinterval_C(segment::dyadic_interval interval)
{
    PyDyadicInterval *self = (PyDyadicInterval *) PyDyadicIntervalType.tp_alloc(&PyDyadicIntervalType, 0);
    if (self != nullptr) {
        self->intrvl = interval;
    }
    return self;
}

static PyObject *
PyDyadicInterval_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{

    static const char *kwlist[] = {"k", "n", nullptr};

    long n=0, k=0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ll", 
                                     const_cast<char**>(kwlist),
                                     &k, &n)) {
        return nullptr;
    }

    PyDyadicInterval *self = (PyDyadicInterval *) type->tp_alloc(type, 0);
    
    segment::dyadic_interval new_interval {k, n};
    self->intrvl = new_interval;
    
    return (PyObject *) self;
}

static PyObject *
PyDyadicInterval_repr(PyObject *self)
{
    PyDyadicInterval *_self = (PyDyadicInterval *) self;

    return PyUnicode_FromFormat("DyadicInterval(%i, %i)",
                                _self->intrvl.k,
                                _self->intrvl.n);
}

static PyObject *
PyDyadicInterval_str(PyObject *self)
{
    PyDyadicInterval *_self = (PyDyadicInterval *) self;
    std::string left_s, right_s;

    left_s = std::to_string(_self->intrvl.inf());
    right_s = std::to_string(_self->intrvl.sup());

    return PyUnicode_FromFormat("[%s, %s)",
                                left_s.c_str(),
                                right_s.c_str());
}

static PyObject *
PyDyadicInterval_excluded_end(PyObject *self)
{
    segment::dyadic num = ((PyDyadicInterval *) self)->intrvl.excluded_end();
    return PyFloat_FromDouble(double{num});

}

static PyObject *
PyDyadicInterval_included_end(PyObject *self)
{
    segment::dyadic num = ((PyDyadicInterval *) self)->intrvl.included_end();
    return PyFloat_FromDouble(double{num});
}

static PyObject *
PyDyadicInterval_sup(PyObject *self)
{
    segment::dyadic num = ((PyDyadicInterval *) self)->intrvl.sup();
    return PyFloat_FromDouble(double{num});
}

static PyObject *
PyDyadicInterval_inf(PyObject *self)
{
    segment::dyadic num = ((PyDyadicInterval *) self)->intrvl.inf();
    return PyFloat_FromDouble(double{num});
}

static PyObject *
PyDyadicInterval_flip_interval(PyObject *self)
{
    ((PyDyadicInterval *) self)->intrvl.flip_interval();
    Py_RETURN_NONE;
}

static PyObject *
PyDyadicInterval_shrink_to_contained_end(PyObject *self, PyObject *arg)
{
    long _arg = 1;

    _arg = PyLong_AsLong(arg);
    if (_arg == -1 && PyErr_Occurred())
        return nullptr;

    segment::dyadic_interval  interval = ((PyDyadicInterval *) self)->intrvl.shrink_to_contained_end(_arg);
    return (PyObject *) build_pydyadicinterval_C(interval);
}

static PyObject *
PyDyadicInterval_shrink_to_omitted_end(PyObject *self)
{
    segment::dyadic_interval  interval = ((PyDyadicInterval *) self)->intrvl.shrink_to_omitted_end();
    return (PyObject *) build_pydyadicinterval_C(interval);
}

static PyObject *
PyDyadicInterval_dyadic_bracket(PyObject *self, PyObject *args, PyObject *kwargs)
{
    static const char *kwlist[] = {"value", "precision", nullptr};
    double val = 0.0;
    int prec = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|di", 
                                    const_cast<char**>(kwlist),
                                     &val, &prec)) {
        return nullptr;
    }

    segment::dyadic_interval interval = segment::dyadic_interval::dyadic_bracket(val, prec);
    return (PyObject *) build_pydyadicinterval_C(interval);
}

static PyObject *
PyDyadicInterval_shrink_left(PyObject *self, PyObject *args)
{
    long _arg = 1;

    _arg = PyLong_AsLong(self);
    if (_arg == -1 && PyErr_Occurred())
        return nullptr;

    ((PyDyadicInterval *) self)->intrvl.shrink_interval_left(_arg);

    Py_RETURN_NONE;
}

static PyObject *
PyDyadicInterval_shrink_right(PyObject *self, PyObject *args)
{
    long _arg = 1;

    _arg = PyLong_AsLong(args);
    if (_arg == -1 && PyErr_Occurred())
        return nullptr;

    ((PyDyadicInterval *) self)->intrvl.shrink_interval_right(_arg);
    Py_RETURN_NONE;
}

static PyObject *
PyDyadicInterval_expand_interval(PyObject *self, PyObject *args)
{
    long _arg = 1;

    _arg = PyLong_AsLong(args);
    if (_arg == -1 && PyErr_Occurred())
        return nullptr;

    ((PyDyadicInterval *) self)->intrvl.expand_interval(_arg);
    Py_RETURN_NONE;    
}

static PyObject *
PyDyadicInterval_aligned(PyObject *self)
{
    int res = ((PyDyadicInterval *) self)->intrvl.aligned();
    if (res == -1) {
        PyErr_SetString(PyExc_RuntimeError, "An error occurred in dyandic interval align method");
        return nullptr;
    } else if (res) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject *
PyDyadicInterval_richcompare(PyObject *self, PyObject *other, int op)
{
    PyDyadicInterval *lhs = (PyDyadicInterval *) self;

    int result = (PyObject_IsInstance(other, (PyObject *) &PyDyadicIntervalType));
    if (result == -1) {
        PyErr_SetString(PyExc_RuntimeError, "An unknown error occurred");
        return nullptr;
    }
    if (result == 0) Py_RETURN_NOTIMPLEMENTED;

    PyDyadicInterval *rhs = (PyDyadicInterval *) other;

    bool res;

    switch (op) { 
        case Py_EQ: res=(lhs->intrvl == rhs->intrvl); break;
        case Py_LE: res=(lhs->intrvl < rhs->intrvl); break;
        case Py_LT: res=(lhs->intrvl < rhs->intrvl || lhs->intrvl == rhs->intrvl); break;
        case Py_GE: res=(!(lhs->intrvl < rhs->intrvl || lhs->intrvl == rhs->intrvl)); break;
        case Py_GT: res=(!(lhs->intrvl < rhs->intrvl)); break;
        default:
            Py_RETURN_NOTIMPLEMENTED;
    }

    if (res) Py_RETURN_TRUE;
    Py_RETURN_FALSE;

}

static PyObject *
PyDyadicInterval_contains(PyObject *self, PyObject *other)
{
    PyDyadicInterval *lhs = (PyDyadicInterval *) self;

    int result = (PyObject_IsInstance(other, (PyObject *) &PyDyadicIntervalType));
    if (result == -1) {
        PyErr_SetString(PyExc_RuntimeError, "An unknown error occurred");
        return nullptr;
    }
    if (result == 0) Py_RETURN_NOTIMPLEMENTED;

    PyDyadicInterval *rhs = (PyDyadicInterval *) other;

    if (lhs->intrvl.contains(rhs->intrvl))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}


// These functions can operate on either dyadic intervals or intervals.
static PyObject *
PyInterval_contains(PyObject *self, PyObject *other)
{
    PyInterval * _self = (PyInterval *) self;
    bool res;

    int result = (PyObject_IsInstance(other, (PyObject *) &PyDyadicIntervalType));
    if (result == -1) {
        return nullptr;
    } else if (result == 1) {
        segment::dyadic_interval inner = ((PyDyadicInterval *) other)->intrvl;
        segment::interval conv {inner.inf(), inner.sup()};
        res = _self->intrvl.contains(conv);
    }

    result = (PyObject_IsInstance(other, (PyObject *) &PyIntervalType));
    if (result == -1) {
        return nullptr;
    } else if (result == 1) {
        res = _self->intrvl.contains(((PyInterval *) other)->intrvl);
    } else if (result == 0) {
        double arg = PyFloat_AsDouble(other);
        if (arg == -1.0 && PyErr_Occurred()) {
            return nullptr;
        }
        
        res = _self->intrvl.contains(arg);
    }

    if (res) Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject *
PyInterval_is_associated(PyObject *self, PyObject *other)
{
    PyInterval * _self = (PyInterval *) self;
    bool res;

    int result = (PyObject_IsInstance(other, (PyObject *) &PyDyadicIntervalType));
    if (result == -1) return nullptr;
    if (result == 0) Py_RETURN_NOTIMPLEMENTED;

    res = _self->intrvl.is_associated(((PyInterval *) other)->intrvl);

    if (res)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}



// Module level functions

static PyObject *
Py_to_dyadic_intervals(PyObject *self, PyObject *args, PyObject *kwargs)
{
    typedef std::deque<segment::dyadic_interval> IntervalDeque;

    static const char *kwlist[] = {"inf", "sup", "tolerance", nullptr};
    double inf, sup;
    int tolerance;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ddi", 
                                     const_cast<char**>(kwlist),
                                     &inf, &sup, &tolerance)) {
        return nullptr;
    }


    IntervalDeque dyadic_intervals = to_dyadic_intervals<clopen, segment::dyadic>(inf, sup, tolerance, clopen);

    Py_ssize_t i, deq_size = static_cast<Py_ssize_t>(dyadic_intervals.size());

    PyObject *di_list = PyList_New(deq_size);
    if (di_list == nullptr)
        return nullptr;

    for (i = 0; i < deq_size; ++i) {
        PyList_SET_ITEM(
            di_list,
            i,
            (PyObject *) build_pydyadicinterval_C(dyadic_intervals[i])
        );
    }

    return di_list;
}


static PyObject *
Py_segment(PyObject *self, PyObject *args, PyObject *kwargs)
{
    typedef std::list<segment::interval> IntervalList;

    static const char *kwlist[] = {"interval", "in_character", "tolerance", "signal_tol", nullptr};
    PyObject *interval = nullptr;
    PyObject *in_character = nullptr;
    long tolerance, signal_tol;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOll", 
                                     const_cast<char**>(kwlist),
                                     &interval,
                                     &in_character,
                                     &tolerance,
                                     &signal_tol))
        return nullptr;

    if (interval == nullptr || in_character == nullptr) {
        PyErr_SetString(PyExc_ValueError, "Interval and in_character must be provided");
        return nullptr;
    }
        

    if (!PyFunction_Check(in_character)) {
        PyErr_SetString(PyExc_TypeError, "Expected a function or callable object");
        return nullptr;
    }
        

    if (!PyObject_IsInstance((PyObject *) interval, (PyObject *) &PyIntervalType)) {
        PyErr_SetString(PyExc_TypeError, "Expected an interval");
        return nullptr;
    }
        

    segment::interval& c_interval = ((PyInterval *) interval)->intrvl;
    SegmentsFunctionCapsule _in_character {in_character};

    IntervalList output;
    try {
        output = segment::segment(c_interval, _in_character, tolerance, signal_tol);
    } catch (SegmentsError) {
        if (!PyErr_Occurred())
            PyErr_SetString(PyExc_RuntimeError, "An unknown error occurred");
        return nullptr;
    }

    Py_ssize_t i = 0, size = static_cast<Py_ssize_t>(output.size());
    PyObject *rv_list = PyList_New(size);
    if (rv_list == nullptr) {
        PyErr_SetString(PyExc_RuntimeError, "An unknown error occurred");
        return nullptr;
    }

    for (auto elt : output) {
        PyList_SET_ITEM(rv_list, i, interval_to_pyinterval(elt));
        ++i;
    }

    return rv_list;
}


static PyMethodDef SegmentsMethods[] = {
    {
        "to_dyadic_intervals", 
        (PyCFunction) Py_to_dyadic_intervals,
        METH_VARARGS | METH_KEYWORDS,
        "Divide an interval into dyadic intervals."
    },
    {
        "segment",
        (PyCFunction) Py_segment,
        METH_VARARGS | METH_KEYWORDS,
        "Break an interval into dyadic segments."
    },
    {nullptr, nullptr, 0, nullptr} /* sentinel */
};


static struct PyModuleDef segmentsmodule = {
    PyModuleDef_HEAD_INIT,
    "_segments",
    nullptr,
    -1,
    SegmentsMethods
};


PyMODINIT_FUNC
PyInit__segments(void) {

    PyObject *m;

    if (PyType_Ready(&PyDyadicIntervalType) < 0) 
        return nullptr;

    
    if (PyType_Ready(&PyIntervalType) < 0)
        return nullptr;

    m = PyModule_Create(&segmentsmodule);
    if (m == nullptr) {
        return nullptr;
    }
        
    Py_INCREF(&PyIntervalType);
    if (PyModule_AddObject(m, "DyadicInterval", (PyObject *) &PyDyadicIntervalType) < 0) {
        Py_DECREF(&PyDyadicIntervalType);
        Py_DECREF(m);
        return nullptr;
    }
        
    Py_INCREF(&PyIntervalType);
    if (PyModule_AddObject(m, "Interval", (PyObject *) &PyIntervalType) < 0) {
        Py_DECREF(&PyIntervalType);
        Py_DECREF(m);
        return nullptr;
    }

    return m;


}
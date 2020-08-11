

#ifndef __PYSEGS_H
#define __PYSEGS_H

#include <Python.h>

#include "segment.h"


PyObject *interval_to_pyinterval(const segment::interval &);

// Thown after PyErr_SetString called, so Python will know
// the actual error type.
struct SegmentsError {};



/** Wrapper for a Python function to make it accessible
 *  in the C++ layer. 
 */
class SegmentsFunctionCapsule
{
    PyObject *_func;

public:

    SegmentsFunctionCapsule(PyObject *func) : _func{func}
    {}


    bool operator()(const segment::interval& intrvl) const
    {
        PyObject *from_interval = interval_to_pyinterval(intrvl);
        if (from_interval == nullptr) {
            PyErr_SetString(
                PyExc_ValueError,
                "Could not convert from interval to PyInterval"
            );
            throw SegmentsError {};
        }

        PyObject *result = PyObject_CallFunctionObjArgs(
            _func,
            from_interval,
            nullptr
        );
         // from_interval must live until the function call
         // completes, but can then be deallocated.
        Py_DECREF(from_interval);

        if (result == nullptr) {
            // error should already be set by _func
            throw SegmentsError {};
        }
        
        int bool_result = PyObject_IsTrue(result);

        // result isn't needed any longer
        Py_DECREF(result);

        if (bool_result == -1) {
            PyErr_SetString(PyExc_RuntimeError, "Error converting result to bool");
            throw SegmentsError {};
        }

        return bool_result;
    }

};


// Python objects

extern "C" {

    typedef struct {
        PyObject_HEAD
        typename segment::interval intrvl;
    } PyInterval;

    typedef struct {
        PyObject_HEAD
        typename segment::dyadic_interval intrvl;
    } PyDyadicInterval;

}







#endif
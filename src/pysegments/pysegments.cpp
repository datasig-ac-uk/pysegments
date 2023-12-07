//
// Created by sam on 08/11/22.
//

#include "pysegments.h"

#include <sstream>
#include <cmath>

#include <pybind11/functional.h>
#include <pybind11/stl.h>


namespace py = pybind11;
using namespace pybind11::literals;

using namespace segments;

namespace
{
    struct Tolerance
    {
        depth_t trim;
        depth_t signal;
    };

    depth_t from_length(const interval& arg) noexcept
    {
        auto length = arg.sup() - arg.inf();
        depth_t expo;
        frexp(length, &expo);
        return -std::min(0, expo - 2);
    }

    inline
    Tolerance get_tolerance(const interval& arg, const py::object& pytol, const py::object& pysignal_tol)
    {
        Tolerance result{0, 0};
        if (pytol.is_none() && pysignal_tol.is_none())
        {
            result.signal = from_length(arg);
            result.trim = result.signal;
        }
        else if (pytol.is_none())
        {
            result.signal = pysignal_tol.cast<depth_t>();
            result.trim = result.signal;
        }
        else if (pysignal_tol.is_none())
        {
            result.trim = pytol.cast<depth_t>();
            result.signal = result.trim;
        }
        else
        {
            result.signal = pysignal_tol.cast<depth_t>();
            result.trim = pytol.cast<depth_t>();
        }


        return result;
    }

    std::vector<interval> py_segment(interval arg,
                                     predicate_t&& predicate,
                                     py::object pytol,
                                     py::object pysignal_tol
    )
    {
        auto tol = get_tolerance(arg, pytol, pysignal_tol);

        return segment(arg, predicate, tol.signal, tol.trim);
    }

    std::vector<interval> py_segment_two_floats(interval arg,
                                                std::function<bool(double, double)> predicate,
                                                py::object pytol, py::object pysignal_tol)
    {
        auto tol = get_tolerance(arg, pytol, pysignal_tol);

        return segment(arg, [predicate](const interval& ivl)
        {
            return predicate(ivl.inf(), ivl.sup());
        }, tol.signal, tol.trim);
    }
} // namespace


PYBIND11_MODULE(_segments, m)
{
    py::class_<interval> py_interval(m, "Interval");

    py_interval.def(py::init<double, double>(), "a"_a = 0.0, "b"_a = 1.0);

    py_interval.def_property_readonly("inf",
                                      &interval::inf);
    py_interval.def_property_readonly("sup",
                                      &interval::sup);

    py_interval.def("contains", [](const interval& self, double arg)
    {
        return self.contains(arg);
    }, "arg"_a);
    py_interval.def("included_end", &interval::included_end);
    py_interval.def("excluded_end", &interval::excluded_end);
    py_interval.def("__str__", [](const interval& self)
    {
        return "[" + std::to_string(double(self.inf())) + ", " + std::to_string(double(self.sup())) + ")";
    });
    py_interval.def("__repr__", [](const interval& self)
    {
        return "Interval(" + std::to_string(double(self.inf())) + ", " + std::to_string(double(self.sup())) + ")";
    });

    py_interval.def("__copy__", [](const interval& self)
    {
        return interval(self);
    });
    py_interval.def("__deepcopy__", [](const interval& self, const py::dict&)
    {
        return segments::interval(self);
    }, "memo"_a);

    m.def("segment", &py_segment, "interval"_a, "predicate"_a, "tolerance"_a = py::none(),
          "signal_tolerance"_a = py::none());
    m.def("segment", &py_segment_two_floats, "interval"_a, "predicate"_a, "tolerance"_a = py::none(),
          "signal_tolerance"_a = py::none());
}

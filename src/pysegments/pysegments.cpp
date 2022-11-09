//
// Created by sam on 08/11/22.
//

#include "pysegments.h"

#include <sstream>

#include <pybind11/functional.h>
#include <pybind11/stl.h>


namespace py = pybind11;
using namespace pybind11::literals;

namespace {

std::vector<segments::interval> py_segment_two_floats(segments::interval arg,
        std::function<bool(double, double)> predicate, segments::depth_t max_depth)
{
    return segments::segment(arg, [predicate](const segments::interval& ivl) {
        return predicate(ivl.inf(), ivl.sup());
    }, max_depth);
}

} // namespace


PYBIND11_MODULE(_segments, m) {

    py::class_<segments::interval> py_interval(m, "Interval");

    py_interval.def(py::init<double, double>(), "a"_a=0.0, "b"_a=1.0);

    py_interval.def_property_readonly("inf",
            &segments::interval::inf);
    py_interval.def_property_readonly("sup",
            &segments::interval::sup);

    py_interval.def("contains", [](const segments::interval& self, double arg)
        { return self.contains(arg); }, "arg"_a);
    py_interval.def("included_end", &segments::interval::included_end);
    py_interval.def("excluded_end", &segments::interval::excluded_end);
    py_interval.def("__str__", [](const segments::interval& self) {
       return "[" + std::to_string(double(self.inf())) + ", " + std::to_string(double(self.sup())) + ")";
    });
    py_interval.def("__repr__", [](const segments::interval& self) {
        return "Interval(" + std::to_string(double(self.inf())) + ", " + std::to_string(double(self.sup())) + ")";
    });

    py_interval.def("__copy__", [](const segments::interval& self) {
        return segments::interval(self);
    });
    py_interval.def("__deepcopy__", [](const segments::interval& self, const py::dict&) {
        return segments::interval(self);
    }, "memo"_a);

    m.def("segment", &segments::segment, "interval"_a, "predicate"_a, "max_resolution"_a);
    m.def("segment", &py_segment_two_floats, "interval"_a, "predicate"_a, "max_resolution"_a);


}

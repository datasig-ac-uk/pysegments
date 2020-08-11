
import pytest

from pysegments import Interval

INTERVALS_EP = [
    (0.5, 0.75),
    (-1.0, 0.5),
    (-1.0, -0.5),
    (0.0, 1.0),
    (0.0, 1.2),
    (0.9, 1.5),
]


@pytest.fixture
def unit_interval():
    return Interval(0.0, 1.0)



def test_creation_and_member_access(unit_interval):

    assert unit_interval.sup() == 1.0
    assert unit_interval.inf() == 0.0


def test_creation_reversed_items():
    intrvl = Interval(1.0, 0.0)

    assert intrvl.sup() == 1.0
    assert intrvl.inf() == 0.0

def test_included_end(unit_interval):
    assert unit_interval.included_end() == 0.0
    assert unit_interval.excluded_end() == 1.0


def test_str_rep(unit_interval):
    assert str(unit_interval) == "[0.000000, 1.000000)"


def test_repr_function(unit_interval):
    assert repr(unit_interval) == "Interval(0.000000, 1.000000)"


def _f(ab):
    i = min(*ab)
    s = max(*ab)
    return (Interval(*ab), 0.0 <= i and s <= 1.0) 


@pytest.mark.parametrize("interval, expected", map(_f, INTERVALS_EP))
def test_contains(interval, expected, unit_interval):
    assert unit_interval.contains(interval) == expected



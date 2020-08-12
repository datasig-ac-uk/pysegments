import pytest

from pysegments import DyadicInterval


@pytest.fixture
def unit_interval():
    return DyadicInterval(0, 0)


def test_creation_and_member_access(unit_interval):
    assert unit_interval.sup == 1.0
    assert unit_interval.inf == 0.0

def test_defaults():
    interval = DyadicInterval()
    assert interval.inf == 0.0
    assert interval.sup == 1.0

DINTERVAL_PAR = [
    (0, 1),
    (0, 2),
    (1, 2),
    (3, 3),
    (-1, 3),
]


def _f(kn):
    k, n = kn
    return DyadicInterval(k, n), k/2**n, (k+1)/2**n


@pytest.mark.parametrize("interval, einf, esup", map(_f, DINTERVAL_PAR))
def test_member_access(interval, einf, esup):
    assert interval.sup == esup
    assert interval.inf == einf


@pytest.mark.parametrize("interval, einf, esup", map(_f, DINTERVAL_PAR))
def test_included_excluded_end(interval, einf, esup):
    assert interval.included_end() == einf
    assert interval.excluded_end() == esup


def _g(kn):
    k, n = kn
    return DyadicInterval(k, n), "DyadicInterval({k}, {n})".format(k=k, n=n)


@pytest.mark.parametrize("interval, expected", map(_g, DINTERVAL_PAR))
def test_repr_function(interval, expected):
    assert repr(interval) == expected


def _h(kn):
    k, n = kn
    return DyadicInterval(k, n), "[{:.06f}, {:.06f})".format(k/2**n, (k+1)/2**n)


@pytest.mark.parametrize("interval, expected", map(_h, DINTERVAL_PAR))
def test_str_function(interval, expected):
    assert str(interval) == expected

def test_flip_interval(unit_interval):
    unit_interval.flip_interval()
    assert unit_interval.inf == 1.0
    assert unit_interval.sup == 2.0

def test_shrink_to_contained_end(unit_interval):
    interval = unit_interval.shrink_to_contained_end(1)
    assert interval.sup == 0.5
    assert interval.inf == 0.0

def test_shrink_to_omitted_end(unit_interval):
    interval = unit_interval.shrink_to_omitted_end()
    assert interval.sup == 1.0
    assert interval.inf == 0.5

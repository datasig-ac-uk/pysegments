import pytest


from pysegments import segment, Interval, DyadicInterval


INTERVALS = (
    Interval(3.14159, 6.28318), 
    Interval(10.14159, 12.28318)
)

def in_character_fn(interval, check_intervals=INTERVALS):
    return any(intrvl.contains(interval) for intrvl in check_intervals)


def test_segment_function_mone():

    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, 5, -1)

    assert len(segments) == 1


def test_segment_function_zero():

    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, 5, 0)

    assert len(segments) == 2
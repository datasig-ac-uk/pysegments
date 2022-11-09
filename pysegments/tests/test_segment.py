import pytest


from pysegments import segment, Interval


INTERVALS = (
    Interval(3.14159, 6.28318), 
    Interval(10.14159, 12.28318)
)

def in_character_fn(interval, check_intervals=INTERVALS):
    for ivl in check_intervals:
        if ivl.inf <= interval.inf and interval.sup <= ivl.sup:
            return True
    return False



def test_segment_function_mone():

    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, -1)

    assert len(segments) == 1


def test_segment_function_zero():

    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, 0)

    assert len(segments) == 2

def test_segment_function_resolution_3():
    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, 3)

    assert len(segments) == 2

def test_segment_function_resolution_5():
    test_interval = Interval(0, 15.2)
    segments = segment(test_interval, in_character_fn, 5)

    assert len(segments) == 2

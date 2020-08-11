from pysegments import segment, Interval

INTERVALS = (
    Interval(3.14159, 6.28318), 
    Interval(10.14159, 12.28318)
)

def in_character_fn(interval, check_intervals=INTERVALS):
    return any(intrvl.contains(interval) for intrvl in check_intervals)




test_interval = Interval(0, 15.2)

segments = segment(test_interval, in_character_fn, 5, -1)


print(segments)

segments = segment(test_interval, in_character_fn, 16, 15)
print(segments)
import os
import sys
import csv
import numpy as np
import itertools

from bokeh.io import curdoc
from bokeh.layouts import row, column

from bokeh.models import ColumnDataSource
from bokeh.models.widgets import Slider, TextInput
from bokeh.plotting import figure
from bokeh.palettes import Spectral as Palette


from filter import IIR2Filter, BitFilter

WIDTH, HEIGHT = 1200, 800


CUTOFF = 100 # Hz
SAMPLEERATE=1000.0

BASE = os.path.dirname(__file__)

FILE = os.path.join(
    BASE, "..", "python-data-logger", "captured-runs",
    "tinyhawk-ableton-hof-paar-runden-1.csv"
    #"tinyhawk-ableton-hof-paar-runden-mit-ausflug-nach-hinten.csv"
    )
assert os.path.exists(FILE)

def read_data(reader):
    return np.array([int(value) for value, ts in itertools.islice(reader, 1, None)])


HALF_TIME = 5 * SAMPLEERATE # seconds * samplerate

CUTOFFS = [5, 10]
BITFILTERS = [3, 5, 7]

LAPTIME_THRESHOLD = 0.95

def laptimes(values):
    max_rssi = max(values)
    timestamps = []
    points = []
    i = 0
    while i < len(values):
        value = values[i]
        if value >= max_rssi * LAPTIME_THRESHOLD:
            timestamps.append(i)
            points.append(value)
            i += int(HALF_TIME)
        i += 1
    return timestamps, points


def main():
    with open(FILE) as inf:
        reader = csv.reader(inf)
        raw = read_data(reader)

    def compute_filtered(cutoff):
        filter = IIR2Filter(2, [cutoff], 'lowpass', fs=SAMPLEERATE)
        return [filter.filter(v) for v in raw]

    def compute_bitfilter(bit):
        filter = BitFilter(bit)
        return [filter.filter(v) for v in raw]

    x = np.linspace(0, len(raw), len(raw))

    data = dict(
        x=x,
        raw=raw,
    )

    def update_filter(attrname, old, new):
        print('update_filter', old, new)
        data[attrname] = compute_filtered(new)
        source.data = data

    sliders = []
    # for initial plotting of all, it's important
    # that the lower frequencies get plotted last.
    for cutoff in reversed(sorted(CUTOFFS)):
        data[f"{cutoff}Hz"] = compute_filtered(cutoff)
        slider = Slider(title=f"{cutoff}", value=cutoff, start=1, end=SAMPLEERATE / 2, step=1)
        slider.on_change('value', update_filter)
        sliders.append(slider)


    # for initial plotting of all, it's important
    # that the higher bits get plotted last
    for bit in sorted(BITFILTERS):
        data[f"{bit}bit"] = compute_bitfilter(bit)

    source = ColumnDataSource(data=data)

    # Set up plot
    plot = figure(
        plot_height=HEIGHT, plot_width=WIDTH, title="VTX laptimer data",
        x_range=[0, len(raw)], y_range=[0, 4096]
    )

    palette = Palette[len(data) - 1]
    for color, key in zip(palette, (k for k in data.keys() if k != 'x')):
        plot.line('x', key, source=source, line_width=1, line_alpha=0.6, legend=f"{key}:", line_color=color)
        lx, ly = laptimes(data[key])
        plot.circle(lx, ly, size=10, fill_color=color, line_color="black", line_width=2, legend=f"{key} laptimes")


    plot.legend.location = "top_left"
    plot.legend.click_policy="hide"

    # # Set up layouts and add to document
    inputs = column(*sliders)
    curdoc().add_root(row(inputs, plot, width=WIDTH))
    curdoc().title = "VTX Laptimer"

main()

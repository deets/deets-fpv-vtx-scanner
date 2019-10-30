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


from filter import IIR2Filter

WIDTH, HEIGHT = 1200, 800


CUTOFF = 100 # Hz
SAMPLEERATE=1000.0

BASE = os.path.dirname(__file__)

FILE = os.path.join(
    BASE, "..", "python-data-logger", "captured-runs",
    "tinyhawk-ableton-hof-paar-runden-mit-ausflug-nach-hinten.csv"
    )
assert os.path.exists(FILE)

def read_data(reader):
    return np.array([int(value) for value, ts in itertools.islice(reader, 1, None)])


CUTOFFS = [5, 10, 20, 100]

def main():
    with open(FILE) as inf:
        reader = csv.reader(inf)
        raw = read_data(reader)

    def compute_filtered(cutoff):
        filter = IIR2Filter(2, [cutoff], 'lowpass', fs=SAMPLEERATE)
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

    source = ColumnDataSource(data=data)

    # Set up plot
    plot = figure(
        plot_height=HEIGHT, plot_width=WIDTH, title="VTX laptimer data",
        x_range=[0, len(raw)], y_range=[0, 4096]
    )

    palette = Palette[len(data) - 1]
    for color, key in zip(palette, (k for k in data.keys() if k != 'x')):
        plot.line('x', key, source=source, line_width=1, line_alpha=0.6, legend=f"{key}:", line_color=color)

    plot.legend.location = "top_left"
    plot.legend.click_policy="hide"

    # # Set up layouts and add to document
    inputs = column(*sliders)
    curdoc().add_root(row(inputs, plot, width=WIDTH))
    curdoc().title = "VTX Laptimer"

main()

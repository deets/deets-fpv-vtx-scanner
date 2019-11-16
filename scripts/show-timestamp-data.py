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

# FILE = os.path.join(
#     BASE, "..", "python-data-logger", "captured-runs",
#     "tinyhawk-ableton-hof-paar-runden-1.csv"
#     #"tinyhawk-ableton-hof-paar-runden-mit-ausflug-nach-hinten.csv"
#     )
FILE = "/tmp/longer-timer-stats.csv"
assert os.path.exists(FILE)

def read_data(reader):
    return [(float(delay), float(offset)) for delay, offset in itertools.islice(reader, 0, None)]


def main():
    with open(FILE) as inf:
        reader = csv.reader(inf)
        delays, offsets  = zip(*read_data(reader))

    data = dict(delays=delays, offsets=offsets)
    source = ColumnDataSource(data=data)

    # Set up plot
    plot = figure(
        plot_height=HEIGHT, plot_width=WIDTH, title="VTX timestamp data",
        x_range=[min(delays), max(delays)], y_range=[min(offsets), max(offsets)]
    )

    plot.circle(delays, offsets, size=5, fill_color="green", line_color="black", line_width=1, legend="laptimes")

    plot.legend.location = "top_left"
    plot.legend.click_policy="hide"

    # # Set up layouts and add to document
    curdoc().add_root(row(plot, width=WIDTH))
    curdoc().title = "VTX Laptimer"

main()

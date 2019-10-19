import json
from os.path import splitext

filenames = [
    "luajit.json",
    "wren.json",
    "lua51.json",
    "lua53.json",
    "duktape.json",
]

plots = {}
configs = []

scales = {
    "ns": 1/1000,
    "us": 1,
    "ms": 1000,
}

for fname in filenames:
    with open("build/" + fname, "r") as f:
        config = splitext(fname)[0]
        configs.append(config)
        data = json.load(f)
        for benchmark in data["benchmarks"]:
            iplot = plots.setdefault(benchmark["name"], {})
            iplot[config] = benchmark["real_time"] * scales[benchmark["time_unit"]]

tests = []
times = {}
mins = []
for name, inner in plots.items():
    tests.append(name)
    m = 1000000000000
    for config in configs:
        t = times.setdefault(config, [])
        if config in inner:
            time = inner[config]
            if time < m:
                m = time
            t.append(time)
        else:
            t.append(0) # dummy
    mins.append(m)

#normalize
#for idx, m in enumerate(mins):
#    for a, t in times.items():
#        t[idx] = t[idx] / m

import matplotlib
import matplotlib.pyplot as plt
import numpy as np

x = np.arange(len(tests))  # the label locations
width = 1/len(tests) - .05  # the width of the bars

fig, ax = plt.subplots()
rects = []
i = 0
for config, timelist in times.items():
    w = (width * i) - (width * (len(tests)-1) * .5)
    rects.append(ax.bar(x+w, timelist, width, label=config))
    i = i + 1

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('''% of worst''')
#ax.set_yscale('log')
ax.set_title('Time (lower is better)')
ax.set_xticks(x)
ax.set_xticklabels(tests)
ax.legend()

def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('',
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


for r in rects:
    autolabel(r)

fig.tight_layout()

plt.show()

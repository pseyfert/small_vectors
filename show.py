#!/usr/bin/env python
from matplotlib import pylab as plt
import re
import numpy as np
import sys

do_plot_for = "UNDERFULL" # "OVERFULL" "UNDERFULL" "OVERFULL" "EXACTFULL"

# with open("multi_bench.log", "r") as f:
# with open("multi_bench_complex.log", "r") as f:
fname = sys.argv[1] if len(sys.argv) > 1 else "multi_bench_complex.log"
with open(fname, "r") as f:
    loglines = [line.split(" ns") for line in f.readlines() if re.match(
        "create.*_min", line) or re.match("reserve.*_min", line)]

grab_name = re.compile("_min *[0-9]*")
config_to_time = dict()
for line in loglines:
    key = grab_name.sub("", line[0])
    CPU_time = int(line[-2])
    config_to_time[key] = CPU_time

container_to_speeds = {}
reference_speeds = {}

for key in config_to_time.keys():
    if re.match(".*reserve.*", key):
        std_reserve = True
    else:
        std_reserve = False
    if re.match(".*DONT_RESERVE_STORAGE.*", key):
        exact_reserve = False
    else:
        exact_reserve = True
    if re.match(".*OVERFULL.*", key):
        overfull = True
    else:
        overfull = False
    if re.match(".*UNDERFULL.*", key):
        underfull = True
    else:
        underfull = False
    if underfull or overfull:
        exactfill = False
    else:
        exactfill = True
    N_threads = int(key[-1])

    container_name = None
    try:
        container_name = re.search("<[a-zA-Z:_]*[ <]", key).group()[1:-1]
        if re.match(".*boost.*pmr.*", key):
            # nasty hack
            container_name += "<boost::container::pmr::polymorphic_allocator>"
        elif re.match(".*pmr.*", key):
            container_name += "<std::pmr::polymorphic_allocator>"
    except AttributeError:
        pass
    if std_reserve:
        container_name = "std::vector::reserve(roughsize)"
    if exact_reserve:
        container_name += "::reserve(rightsize)"

    if (underfull and do_plot_for == "UNDERFULL") or (exactfill and do_plot_for == "EXACTFULL") or (overfull and do_plot_for == "OVERFULL"):
        if container_name is None:
            print("PANIC")
            continue
        if container_name in container_to_speeds.keys():
            container_to_speeds[container_name][N_threads] = config_to_time[key]
        else:
            container_to_speeds[container_name] = {
                N_threads: config_to_time[key]
            }

    if container_name == "std::vector" and ((underfull and do_plot_for == "UNDERFULL") or (exactfill and do_plot_for == "EXACTFULL") or (overfull and do_plot_for == "OVERFULL")):
        reference_speeds[N_threads] = config_to_time[key]

container_to_speedups = {}

for container_name in container_to_speeds.keys():
    container_to_speedups[container_name] = [
        1./(float(container_to_speeds[container_name][N_threads])/float(reference_speeds[N_threads])) for N_threads in [1, 2, 4]]

singlespeeds = [(container_name, container_to_speedups[container_name])
                for container_name in container_to_speeds.keys()]

singlespeeds.sort(key=lambda x: x[1])

bar_width = 1./(2.5+len(container_to_speeds))

xaxis = np.arange(len([1, 2, 4]))

fig, ax = plt.subplots()
ax.set_yscale('log')

counter = 0

print("tried to print with")
print(container_to_speedups)
slowest = min(min(container_to_speedups[k]) for k in container_to_speedups.keys())
for container in (x[0] for x in singlespeeds):
    plt.bar(xaxis + counter*bar_width,
            container_to_speedups[container], bar_width, label=container)

    counter += 1
    # print(container_to_speedups[container])

plt.xticks(xaxis, ["1 thread", "2 threads", "4 threads"])
plt.ylabel("speedup wrt. vector")
plt.xlabel("scenario")
plt.legend()
plt.tight_layout()
plt.gca().set_ylim(bottom=min(0.9, slowest))


plt.show()

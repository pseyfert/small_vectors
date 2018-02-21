from matplotlib import pylab as plt
import re
import numpy as np


with open("multi_bench_complex.log", "r") as f:
    loglines = [line.split(" ns") for line in f.readlines() if re.match("create.*", line)]

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
    except AttributeError:
        pass
    if std_reserve:
        container_name = "std::vector::reserve(roughsize)"
    if exact_reserve:
        container_name += "::reserve(rightsize)"

    #if underfull:
    #if exactfill:
    if overfull:
        if container_name is None:
            print("PANIC")
            continue
        if container_name in container_to_speeds.keys():
            container_to_speeds[container_name][N_threads] = config_to_time[key]
        else:
            container_to_speeds[container_name] = {
                N_threads: config_to_time[key]
                }
    #if underfull and container_name == "std::vector":
    #if exactfill and container_name == "std::vector":
    if overfull and container_name == "std::vector":
        reference_speeds[N_threads] = config_to_time[key]

container_to_speedups = {}

for container_name in container_to_speeds.keys():
    container_to_speedups[container_name] = [1./(float(container_to_speeds[container_name][N_threads])/float(reference_speeds[N_threads])) for N_threads in [1, 2, 4]]

singlespeeds = [(container_name, container_to_speedups[container_name]) for container_name in container_to_speeds.keys()]

singlespeeds.sort(key=lambda x: x[1])

bar_width = 1./(2.5+len(container_to_speeds))

xaxis = np.arange(len([1, 2, 4]))

fig, ax = plt.subplots()
ax.set_yscale('log')

counter = 0

print("tried to print with")
print(container_to_speedups)
for container in (x[0] for x in singlespeeds):
    plt.bar(xaxis + counter*bar_width, container_to_speedups[container], bar_width, label=container)

    counter += 1
    # print(container_to_speedups[container])

plt.xticks(xaxis, ["1 thread", "2 threads", "4 threads"])
plt.ylabel("speedup wrt. vector")
plt.xlabel("scenario")
plt.legend()
plt.tight_layout()
plt.gca().set_ylim(bottom=0.9)


plt.show()

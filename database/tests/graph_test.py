from matplotlib import pyplot as plt
from matplotlib import style
from matplotlib.ticker import MultipleLocator, FormatStrFormatter

test_data = []

# read in test results file
with open("test_data.omdbt", "r") as f:
	for line in f:
		for word in line.split():
			test_data.append(word)

test_name = test_data[0]
test_case_count = test_data[1]

speedup_time1 = []
speedup_time2 = []
speedup_time3 = []
speedup_ideal = ['1','2','4','8','16','32','64']

for x in range(2, 9):
	speedup_time1.append(float(test_data[x]))

for x in range(9, 16):
	speedup_time2.append(float(test_data[x]))

for x in range(16, 23):
	speedup_time3.append(float(test_data[x]))


# create and show plot
xticks = ['1','2','4','8','16','32','64']
x = list(range(len(speedup_time1)))

style.use('ggplot')
plt.plot(x, speedup_time1, linewidth=3, label="OpenMemDB")
plt.plot(x, speedup_time2, linewidth=3, label="MemSQL")
plt.plot(x, speedup_time3, linewidth=3, label="VoltDB")
plt.plot(x, speedup_ideal, linewidth=3, label="Ideal")

plt.xticks(x, xticks)
plt.title(test_name)
plt.legend(loc='upper left', numpoints = 1)
plt.ylabel('Speedup')
plt.xlabel('Thread Count')
plt.savefig("test_graph.png")
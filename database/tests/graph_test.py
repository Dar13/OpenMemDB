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

thread_time = []

for x in range(2, len(test_data)):
	thread_time.append(int(test_data[x]))


for x in thread_time:
	print(x)

# create and show plot
xticks = ['1','2','4','8','16','32','48']
x = list(range(len(thread_time)))

style.use('ggplot')
plt.plot(x, thread_time, linewidth=3)

plt.xticks(x, xticks)
plt.title(test_name)
plt.ylabel('Time (microseconds)')
plt.xlabel('Thread Count')
plt.savefig("test_graph.png")
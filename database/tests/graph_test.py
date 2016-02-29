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
thread_time = [int(test_data[2]), int(test_data[3]), int(test_data[4]), int(test_data[5])]

# create and show plot
xticks = ['1','2','4','8']
x = [1, 2, 3, 4]

style.use('ggplot')
plt.plot(x, thread_time, linewidth=3)

plt.xticks(x, xticks)
plt.title(test_name)
plt.ylabel('Time (microseconds)')
plt.xlabel('Thread Count')
plt.savefig("test_graph.png")
"""
=================
Stacked bar chart
=================

This is an example of creating a stacked bar plot with error bars
using `~matplotlib.pyplot.bar`.  Note the parameters *yerr* used for
error bars, and *bottom* to stack the women's bars on top of the men's
bars.
"""

import matplotlib.pyplot as plt


labels = ['G1', 'G2', 'G3']
men_means = [0, 0, 30]
women_means = [0,0, 34]
men_std = [0, 0, 0]
women_std = [0, 0, 0]
width = 0.35       # the width of the bars: can also be len(x) sequence

fig, ax = plt.subplots()

ax.bar(labels, men_means, yerr=men_std)
ax.bar(labels, women_means, bottom=men_means)

ax.set_ylabel('Scores')
ax.set_title('Scores by group and gender')
ax.legend()

plt.show()

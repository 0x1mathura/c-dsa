
import sys
import os
import math
import matplotlib.pyplot as plt
from matplotlib.ticker import (
	MaxNLocator,
	LogLocator,
	LogFormatterSciNotation,
	ScalarFormatter
)

TIMEOUT_MS = 60000

Y_SCALE = "auto"
Y_MARGIN = 0.15
MIN_Y_RANGE = 0.001
SHOW_TIMEOUT = True
Y_TICK_COUNT = 8


def main():
	print("Visualization started")

	if len(sys.argv) < 2:
		print("sys arg are few")
		sys.exit(1)

	triple, cardinality, compare_type = map(
		int, sys.argv[1].split("##")
	)

	filename = (
		f"vis_data_{compare_type}_{triple}_{cardinality}.txt"
	)

	if not os.path.isfile(filename):
		print("File not found:", filename)
		return

	inputs = []
	times = []

	with open(filename, "r") as file:
		for line in file:
			parts = line.split()

			if len(parts) < 2:
				continue

			try:
				time_ms = float(parts[0])
				n_cubed = float(parts[1])
			except ValueError:
				continue

			if not math.isfinite(time_ms) or not math.isfinite(n_cubed):
				continue

			if time_ms < 0:
				continue

			inputs.append(n_cubed)
			times.append(time_ms)

	if not times:
		print("No valid data found", flush=True)
		return

	data = sorted(zip(inputs, times))
	inputs, times = zip(*data)

	graph_types = {
		1: "Brute Force Algorithm",
		2: "Heuristic Algorithm",
		3: "Approximation Algorithm"
	}
	graph_type = graph_types.get(
		compare_type, "Unknown Algorithm"
	)

	fig, ax = plt.subplots(
		figsize=(10, 6),
		layout="constrained"
	)

	ax.plot(
		inputs,
		times,
		"o-",
		color="blue",
		linewidth=2,
		markersize=4,
		label="Execution time"
	)

	positive_times = [t for t in times if t > 0]
	max_time = max(times)

	# Decide the Y-axis scale.
	use_log = Y_SCALE == "log"

	if Y_SCALE == "auto" and positive_times:
		use_log = (
			max(positive_times) / min(positive_times) >= 100
		)

	if use_log and positive_times:
		ax.set_yscale("log")

		low = min(positive_times)
		high = max(positive_times)

		ax.set_ylim(
			low / (1 + Y_MARGIN),
			high * (1 + Y_MARGIN)
		)

		# Multiple major ticks: 1, 2, 5 per decade.
		ax.yaxis.set_major_locator(
			LogLocator(base=10, subs=(1.0, 2.0, 5.0))
		)
		ax.yaxis.set_major_formatter(
			LogFormatterSciNotation(base=10)
		)

		# Minor ticks improve readability between major ticks.
		ax.yaxis.set_minor_locator(
			LogLocator(
				base=10,
				subs=(3.0, 4.0, 6.0, 7.0, 8.0, 9.0)
			)
		)

	else:
		# Linear scale: adapt to actual execution times.
		padding = max(
			max_time * Y_MARGIN,
			MIN_Y_RANGE
		)

		ax.set_ylim(0, max_time + padding)

		# Explicitly request several readable Y-axis labels.
		ax.yaxis.set_major_locator(
			MaxNLocator(
				nbins=Y_TICK_COUNT,
				min_n_ticks=5,
				steps=[1, 2, 2.5, 5, 10]
			)
		)

		formatter = ScalarFormatter(useOffset=False)
		formatter.set_scientific(False)
		ax.yaxis.set_major_formatter(formatter)

	# Add timeout line only when it fits the current scale.
	y_low, y_high = ax.get_ylim()

	if SHOW_TIMEOUT and y_low <= TIMEOUT_MS <= y_high:
		ax.axhline(
			TIMEOUT_MS,
			color="red",
			linestyle="--",
			linewidth=1.5,
			label="60,000 ms timeout"
		)

	ax.set_xlabel("Input size (n^3)")
	ax.set_ylabel("Execution time (milliseconds)")
	ax.set_title(
		f"cardinality={cardinality}, {graph_type}"
	)

	ax.grid(True, alpha=0.3, which="major")
	ax.grid(True, alpha=0.15, which="minor")
	ax.legend()

	output_file = (
		f"vis_plot_{compare_type}_{triple}_{cardinality}.png"
	)

	fig.savefig(output_file, dpi=300)
	plt.close(fig)

	print("Graph saved:", output_file, flush=True)


if __name__ == "__main__":
	main()

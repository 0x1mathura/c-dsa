
import os
import csv
import networkx as nx
import matplotlib.pyplot as plt
from collections import Counter
from matplotlib.patches import Patch
from networkx.algorithms import community as nxc


os.makedirs("outputs", exist_ok=True)


# COMMUNITY DETECTION ALGORITHMS

def louvain(G):
    return list(nxc.louvain_communities(G))


def greedy_modularity(G):
    return list(nxc.greedy_modularity_communities(G))


def label_propagation(G):
    return list(nxc.asyn_lpa_communities(G))


def girvan_newman(G):
    best_partition = None
    best_score = -1

    for partition in nxc.girvan_newman(G):
        if len(partition) > 12:
            break

        score = nxc.modularity(G, partition)

        if score > best_score:
            best_score = score
            best_partition = partition

    return list(best_partition)


algorithms = {
    "Louvain": louvain,
    "Greedy Modularity": greedy_modularity,
    "Label Propagation": label_propagation,
    "Girvan-Newman": girvan_newman
}



def to_labels(partition):
    labels = {}

    for i in range(len(partition)):
        for node in partition[i]:
            labels[node] = i

    return labels


def comb2(x):
    return x * (x - 1) / 2


def evaluate(G, detected, truth):

    det = to_labels(detected)
    tru = to_labels(truth)

    nodes = list(G.nodes())
    common = Counter()

    for node in nodes:
        key = (det[node], tru[node])
        common[key] += 1


    detected_sizes = Counter(det.values())
    true_sizes = Counter(tru.values())

   
    tp = 0
    for x in common.values():
        tp += comb2(x)

 
    predicted_pairs = 0
    for x in detected_sizes.values():
        predicted_pairs += comb2(x)

 
    actual_pairs = 0
    for x in true_sizes.values():
        actual_pairs += comb2(x)


    if predicted_pairs == 0:
        precision = 0
    else:
        precision = tp / predicted_pairs


    if actual_pairs == 0:
        recall = 0
    else:
        recall = tp / actual_pairs

    # Calculate F1 score
    if precision + recall == 0:
        f1 = 0
    else:
        f1 = 2 * precision * recall / (precision + recall)

    return {
        "Precision": precision,
        "Recall": recall,
        "F1": f1
    }

# 3. DATASET


def load_dataset():

    G = nx.karate_club_graph()

    groups = {}

    for node, data in G.nodes(data=True):
        groups.setdefault(data["club"], set()).add(node)

    truth = list(groups.values())

    return G, truth





def run_experiments(G, truth):

    results = {}
    partitions = {}

    for name, algorithm in algorithms.items():

        print("Running:", name)

        partition = algorithm(G)

        partitions[name] = partition

        results[name] = evaluate(
            G, partition, truth
        )

    return results, partitions



# 5 SAVE RESULTS


def save_results(results):

    metrics = ["Precision", "Recall", "F1"]

    print("\nCOMMUNITY DETECTION RESULTS")
    print("-" * 65)

    print(
        f"{'Algorithm':<22}"
        f"{'Precision':>13}"
        f"{'Recall':>13}"
        f"{'F1':>13}"
    )

    for name, scores in results.items():

        print(
            f"{name:<22}"
            f"{scores['Precision']:>13.3f}"
            f"{scores['Recall']:>13.3f}"
            f"{scores['F1']:>13.3f}"
        )

    with open("outputs/results.csv", "w", newline="") as file:

        writer = csv.writer(file)

        writer.writerow(["Algorithm"] + metrics)

        for name, scores in results.items():

            writer.writerow(
                [name] + [scores[m] for m in metrics]
            )

    print("\nResults saved to outputs/results.csv")



# 6. PLOT COMMUNITY DETECTION

def plot_communities(G, truth, partitions, results):

    # Same node positions for every algorithm
    pos = nx.spring_layout(G, seed=42)

    all_partitions = {
        "Ground Truth": truth,
        **partitions
    }


    palette = [
        "#6F8FC0",  # Blue
        "#FFA500",  # Orange
        "#B8C0CC",  # Grey
        "#E6C77A",  # Light gold
        "#555555"   # Dark grey
    ]

    fig, axes = plt.subplots(
        2, 3,
        figsize=(16, 11)
    )

    axes = axes.flatten()

    for ax, (name, partition) in zip(
        axes, all_partitions.items()
    ):


        partition = sorted(
            partition,
            key=lambda c: min(c)
        )


        node_community = {}

        for i, community in enumerate(partition):

            for node in community:
                node_community[node] = i


        node_colors = [
            palette[node_community[node] % len(palette)]
            for node in G.nodes()
        ]

        # Draw edges
        nx.draw_networkx_edges(
            G,
            pos,
            ax=ax,
            edge_color="gray",
            alpha=0.65,
            width=0.8
        )

        # Draw nodes
        nx.draw_networkx_nodes(
            G,
            pos,
            ax=ax,
            node_color=node_colors,
            node_size=250,
            edgecolors="black",
            linewidths=0.5
        )

        # Draw labels
        nx.draw_networkx_labels(
            G,
            pos,
            ax=ax,
            font_size=8,
            font_color="black"
        )

        # Titles
        if name == "Ground Truth":

            title = (
                f"Ground truth ({len(partition)} factions)"
            )

        else:

            precision = results[name]["Precision"]

            title = (
                f"{name}\n"
                f"Precision = {precision:.2f}"
            )

        ax.set_title(
            title,
            fontsize=12,
            fontweight="bold"
        )

        ax.axis("off")

        # Create individual community legends
        legend_handles = []

        for i, community in enumerate(partition):

            legend_handles.append(
                Patch(
                    facecolor=palette[i % len(palette)],
                    edgecolor="black",
                    label=(
                        f"Community {i + 1} "
                        f"({len(community)} nodes)"
                    )
                )
            )

        ax.legend(
            handles=legend_handles,
            loc="upper center",
            bbox_to_anchor=(0.5, -0.04),
            ncol=2,
            fontsize=7,
            frameon=False
        )

    # Hide unused sixth subplot
    for ax in axes[len(all_partitions):]:
        ax.axis("off")

    fig.suptitle(
        "Detected communities on the Karate Club network",
        fontsize=15,
        fontweight="bold",
        y=0.97
    )

    plt.subplots_adjust(
        top=0.91,
        bottom=0.07,
        hspace=0.32,
        wspace=0.12
    )

    plt.savefig(
        "outputs/community_detection.png",
        dpi=300,
        bbox_inches="tight"
    )

    plt.close()

    print("Community visualization saved.")

# 7. PLOT EVALUATION METRICS


def plot_metrics(results):

    algorithms_names = list(results.keys())

    metrics = [
        "Precision",
        "Recall",
        "F1"
    ]

    # Metric colors
    colors = [
        "#14213D",  # Precision
        "#FCA311",  # Recall
        "#B8C0CC"   # F1
    ]

    x = range(len(algorithms_names))

    width = 0.25

    fig, ax = plt.subplots(
        figsize=(10, 6)
    )

    # Draw grouped bars
    for i, metric in enumerate(metrics):

        values = [
            results[name][metric]
            for name in algorithms_names
        ]

        positions = [
            j + (i - 1) * width
            for j in x
        ]

        bars = ax.bar(
            positions,
            values,
            width,
            label=metric,
            color=colors[i]
        )

        # Display values above bars
        for bar, value in zip(bars, values):

            ax.text(
                bar.get_x() + bar.get_width() / 2,
                value + 0.015,
                f"{value:.2f}",
                ha="center",
                va="bottom",
                fontsize=8
            )

    # Formatting
    ax.set_ylim(0, 1.12)

    ax.set_ylabel(
        "Score",
        fontsize=11
    )

    ax.set_title(
        "Pairwise Precision, Recall and F1 - Karate Club",
        fontsize=14,
        fontweight="bold"
    )

    ax.set_xticks(list(x))

    ax.set_xticklabels(
        algorithms_names,
        rotation=20,
        ha="right"
    )

    ax.legend(
        title="Metric",
        loc="upper right"
    )

    ax.grid(
        axis="y",
        alpha=0.2
    )

    ax.set_axisbelow(True)

    plt.tight_layout()

    plt.savefig(
        "outputs/metrics_comparison.png",
        dpi=300,
        bbox_inches="tight"
    )

    plt.close()

    print("Metrics visualization saved.")

# 8. MAIN PROGRAM


if __name__ == "__main__":

    G, truth = load_dataset()

    results, partitions = run_experiments(G, truth)

    save_results(results)

    plot_communities(G,truth,partitions,results)

    plot_metrics(results)

    print("\nExperiment completed successfully!")
   

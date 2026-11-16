import networkx as nx


G = nx.karate_club_graph()

with open("karate.txt", "w") as f:
    for u, v in G.edges():
        f.write(f"{u} {v}\n")

print("Saved successfully!")
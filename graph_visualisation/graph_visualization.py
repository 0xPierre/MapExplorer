"""
Install the depencies : 
$ pip install matplotlib networkx
Write a graph using Graph_write_to_file in C.
Then you can : $ python3 graph_visualization.py
"""

from typing import List, Union
import networkx as nx
import matplotlib.pyplot as plt


class ArcData:
    def __init__(self, weight):
        self.weight = weight


class ArcList:
    def __init__(self, target, data=None):
        self.target = target
        self.data = data
        self.next = None


class GraphNode:
    def __init__(self, node_id, arc_count, arc_list=None):
        self.id = node_id
        self.arc_count = arc_count
        self.arc_list = arc_list


class Graph:
    def __init__(self, size):
        self.nodes: List[Union[GraphNode, None]] = [None] * size
        self.size = size


def load_graph_form_file(filename: str) -> Graph:
    """
    Load a graph from a file
    :param filename: The name of the file
    :return: The graph
    """
    graph = None

    with open(filename, 'r') as file:
        num_nodes = int(file.readline())

        graph = Graph(num_nodes)

        for i in range(num_nodes):
            node_line = file.readline().split()
            node_id = int(node_line[0])
            arc_count = int(node_line[1])
            arc_list = []

            for j in range(arc_count):
                arc_line = file.readline().split()
                target = int(arc_line[0])
                weight = float(arc_line[1])
                arc_list.append(ArcList(target, ArcData(weight)))

            graph.nodes[node_id] = GraphNode(node_id, arc_count, arc_list)

            file.readline()

        return graph


def visualize_graph(graph: Graph) -> None:
    """
    This function use the networkx library to visualize the graph
    :param graph: The thing with some nodes and arcs to visualize
    :return:
    """
    G = nx.DiGraph()

    for node in graph.nodes:
        if node:
            G.add_node(node.id)

    for node in graph.nodes:
        if node and node.arc_list:
            for arc in node.arc_list:
                G.add_edge(node.id, arc.target, weight=arc.data.weight)

    pos = nx.spring_layout(G)
    nx.draw(G, pos, with_labels=True, node_size=700, node_color="skyblue", font_size=10, font_weight="bold", arrows=True)

    labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=labels)

    plt.show()


if __name__ == "__main__":
    graph = load_graph_form_file("./nodes.graph")
    visualize_graph(graph)
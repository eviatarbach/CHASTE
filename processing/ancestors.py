import copy

class Node:
    def __init__(self, num, time):
        self.num = num
        self.time = time
    def __repr__(self):
        return 'Node {num}, time {time}'.format(num=self.num, time=self.time)
    def __lt__(self, other):
        return self.num < other.num
    def __eq__(self, other):
        return self.num == other.num

class Tree:
    """Simple binary tree (not BST) implementation for cell family tree"""
    def __init__(self, node, left, right, init_pop=None):
        self.node = node
        self.left = left
        self.right = right
        self.init_pop = init_pop
        self.current_gen = copy.copy(init_pop)
    def __repr__(self):
        return self.node.__repr__() + ': ({left}, {right})'.format(left=self.left,
                                                                   right=self.right)
    def add_node(self, parent_num, child_node):
        for index, subtree in enumerate(self.current_gen):
            if parent_num == int(str(subtree.node.num).strip()):
                self.current_gen.pop(index)
                subtree.left = Tree(child_node, None, None)
                subtree.right = Tree(Node(str(subtree.node.num) + ' ', subtree.node.time), None, None)
                self.current_gen.append(subtree.left)
                self.current_gen.append(subtree.right)
                return

def parse_line(line):
    time, cells = line.split('\t')
    cells = map(int, cells.strip().split(' '))
    return (float(time), cells)

def build_tree(text):
    text = filter(None, text.split('\n'))
    seen = set()
    for index, line in enumerate(text):
        time, cells = parse_line(line)
        if index == 0:
            init_pop = [Tree(Node(cell, 0.0), None, None) for cell in cells]
            tree = Tree(None, None, None, init_pop)
            seen.update(cells)
            continue
        for cell_index, cell in enumerate(cells):
            if cell not in seen:
                prev_line = parse_line(text[index - 1])
                tree.add_node(prev_line[1][cell_index], Node(cell, prev_line[0]))
                seen.add(cell)
    return tree

def plot_tree(tree, title=""):
    import networkx
    import matplotlib.pyplot as plt

    G = networkx.DiGraph()

    def gen_graph(t):
        for child in filter(None, [t.left, t.right]):
            G.add_node(child.node.num)
            G.add_edge(t.node.num, child.node.num)
            gen_graph(child)

    for cell in tree.init_pop: gen_graph(cell)

    networkx.write_dot(G,'test.dot')

    plt.title(title)
    pos = networkx.graphviz_layout(G,prog='dot')
    networkx.draw(G, pos, with_labels=False, arrows=False)
    plt.savefig('nx_test.png')

    plt.show()

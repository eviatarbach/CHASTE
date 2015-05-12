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
    def __init__(self, node, left, right):
        assert left == None and right == None
        self.node = node
        self.left = left
        self.right = right
        self.current_gen = [self]
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
        for cell_index, cell in enumerate(cells):
            if index == 0:
                tree = Tree(Node(cell, 0.0), None, None)
                seen.add(cell)
            elif cell not in seen:
                prev_line = parse_line(text[index - 1])
                tree.add_node(prev_line[1][cell_index], Node(cell, prev_line[0]))
                seen.add(cell)
    return tree

def plot_tree(tree):
    import networkx
    import matplotlib.pyplot as plt

    G = networkx.DiGraph()

    def gen_graph(t):
        for child in filter(None, [t.left, t.right]):
            G.add_node(child.node.num)
            G.add_edge(t.node.num, child.node.num)
            gen_graph(child)

    gen_graph(tree)

    networkx.write_dot(G,'test.dot')

    plt.title("draw_networkx")
    pos = networkx.graphviz_layout(G,prog='dot')
    networkx.draw(G, pos, with_labels=True, arrows=False)
    #plt.savefig('nx_test.png')

    plt.show()

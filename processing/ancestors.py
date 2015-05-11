import bisect

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
    """Simple B-Tree implementation for cell family tree"""
    def __init__(self, node, children):
        self.node = node
        self.children = children
    def __repr__(self):
        return self.node.__repr__() + ': {children}'.format(children=self.children)
    def __lt__(self, other):
        return self.node < other.node
    def __eq__(self, other):
        return self.node == other.node
    def add_node(self, parent_num, child_node):
        if self.node.num == parent_num:
            bisect.insort_left(self.children, Tree(child_node, []))
        else:
            if self.children == []:
                return
            i = bisect.bisect_left(self.children, Tree(Node(parent_num, None), []))
            i = i if i < len(self.children) else len(self.children) - 1
            return self.children[i].add_node(parent_num, child_node)

def parse_line(line):
    time, cells = line.split('\t')
    cells = map(int, cells.strip().split(' '))
    return (float(time), cells)

def build_tree(text):
    tree = Tree(Node(-1, -1), [])
    text = filter(None, text.split('\n'))
    seen = set()
    for index, line in enumerate(text):
        time, cells = parse_line(line)
        for cell_index, cell in enumerate(cells):
            if index == 0:
                tree.add_node(-1, Node(cell, 0.0))
                seen.add(cell)
            elif cell not in seen:
                prev_line = parse_line(text[index - 1])
                tree.add_node(prev_line[1][cell_index], Node(cell, prev_line[0]))
                seen.add(cell)
    return tree

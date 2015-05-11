class Node:
    def __init__(self, num, time):
        self.num = num
        self.time = time

class Tree:
    def __init__(self, node, children):
        self.node = node
        self.children = children
    def __repr__(self):
        return self.node.num.__repr__()
    def add_node(self, parent_num, child_node):
        if self.node.num == parent_num:
            self.children.append(Tree(child_node, []))
        else:
            if self.children == []:
                return
            for child in self.children:
                return child.add_node(parent_num, child_node)

def parse(text):
    tree = BinaryTree()
    text = text.split('\n')
    seen = set()
    for line in text:
        time, cells = line.split('\t')
        cells = map(int, cells.strip().split(' '))
        #for cell in cells:
        #    if cell not in seen:
                

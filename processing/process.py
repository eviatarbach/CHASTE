import collections
import xml.etree.ElementTree as etree

import matplotlib.pyplot as plt

ATTRS = ['area', 'perimeter', 'contact_perimeter', 'x', 'y']


class Simulation:
    def __init__(self, filename):
        self.data = etree.parse(filename)

        for attr in ATTRS:
            setattr(self, attr, collections.OrderedDict())
            setattr(self, 'plot_' + attr, self.func_gen(attr))

        for time in self.data.getroot():
            for cell in time:
                identifier = cell.attrib['generation'] + '_' + cell.attrib['cell_id']
                for attr in ATTRS:
                    if identifier in getattr(self, attr).keys():
                        getattr(self, attr)[identifier].append(float(cell.attrib[attr]))
                    else:
                        getattr(self, attr)[identifier] = [float(cell.attrib[attr])]

    def func_gen(self, attr):
        def func(cell_id):
            plt.plot(getattr(self, attr)[cell_id])
            plt.ylabel(attr)
            plt.show()
        return func

    def plot_pos(self, cell_id):
        plt.plot(self.x[cell_id], self.y[cell_id])
        plt.xlabel('x')
        plt.ylabel('y')
        plt.show()

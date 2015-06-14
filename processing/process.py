import collections
import xml.etree.ElementTree as etree

ATTRS = ['area', 'perimeter']


class Simulation:
    def __init__(self, filename):
        self.data = etree.parse(filename)

        for attr in ATTRS:
            setattr(self, attr, collections.OrderedDict())

        for time in self.data.getroot():
            for cell in time:
                identifier = cell.attrib['generation'] + '_' + cell.attrib['cell_id']
                for attr in ATTRS:
                    if identifier in getattr(self, attr).keys():
                        getattr(self, attr)[identifier].append(float(cell.attrib[attr]))
                    else:
                        getattr(self, attr)[identifier] = [float(cell.attrib[attr])]

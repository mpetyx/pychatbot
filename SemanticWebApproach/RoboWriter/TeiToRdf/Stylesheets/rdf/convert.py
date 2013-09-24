import sys
import rdflib
g = rdflib.ConjunctiveGraph()
g.parse(sys.argv[1])
print g.serialize(format='xml')

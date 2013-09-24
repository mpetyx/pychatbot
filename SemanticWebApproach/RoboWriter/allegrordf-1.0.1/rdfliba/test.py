import unittest2

from rdflib import Namespace, Literal, URIRef
from rdflib.namespace import _XSD_NS

from store import Store

class TestStore(unittest2.TestCase):
    
    def setUp(self):
        self.store = Store(name="scratch")
        self.ns = Namespace('http://example.com/#')
        
    def tearDown(self):
        self.store.close()
        
    def testSize(self):
        """ Tests the size of the repository """
        self.assertEqual(len(self.store),0)
        
    def testAdd(self):
        bob = self.ns['bob']
        name = self.ns['name']
        value = Literal('Bob Bilbins')
        self.store.add((bob, name, value))
        self.assertEqual(len(self.store),1)
        
        
    def testRemove(self):
        triple = (self.ns['alice'],self.ns['name'],Literal('Alice'))
        self.store.add(triple)
        self.assertEqual(len(self.store),1)
        self.store.remove(triple)
        self.assertEqual(len(self.store),0)
        
    def testTriples(self):
        """ Tests the search by triple. """
        triple = (self.ns['alice'],self.ns['name'],Literal('Alice'))
        self.store.add(triple)
        for tri in self.store.triples((self.ns['alice'],None, None)):
            for i in range(3):
                self.assertEqual(tri[i], triple[i])
                
    def testSimpleSparql(self):
        triple = (self.ns['alice'],self.ns['name'],Literal('Alice'))
        self.store.add(triple)
        for tri in self.store.query("SELECT ?s ?p ?o  WHERE {?s ?p ?o .}"):
            for i in range(3):
                self.assertEqual(tri[i], triple[i])
        
    def testNamespacedSparql(self):
        triple = (self.ns['alice'],self.ns['name'],Literal('Alice'))
        self.store.add(triple)
        self.store.add((self.ns['bob'],self.ns['name'],Literal('Bob')))
        for tri in self.store.query("SELECT ?p ?o  WHERE { ex:alice ?p ?o .}", initNs={'ex':self.ns}):
            for i in range(1,3):
                self.assertEqual(tri[i-1], triple[i])
        
    def testBindedSparql(self):
        triple = (self.ns['alice'],self.ns['name'],Literal('Alice'))
        self.store.add(triple)
        self.store.add((self.ns['bob'],self.ns['name'],Literal('Bob')))
        for tri in self.store.query("SELECT ?p ?o  WHERE { ?s ?p ?o .}", initBindings={'s':self.ns['alice']}):
            for i in range(1,3):
                self.assertEqual(tri[i-1], triple[i])
    
    def testDataTypes(self):
        birth = Literal('2006-01-03', datatype=_XSD_NS.date)
        comp = Literal('2006-01-01', datatype=_XSD_NS.date)
        triple = (self.ns['alice'],self.ns['birthdate'],birth)
        self.store.add(triple)
        for s, p, o in self.store.query("SELECT ?s ?p ?o  WHERE {?s ?p ?o .}"):
            self.assertLess(comp,birth)
    
if __name__ == '__main__':
    unittest2.main()
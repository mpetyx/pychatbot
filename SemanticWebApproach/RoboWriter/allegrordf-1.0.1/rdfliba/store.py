import os
import urllib
import datetime
import time

from rdflib import Namespace, Literal, URIRef

from franz.openrdf.sail.allegrographserver import AllegroGraphServer
from franz.openrdf.repository.repository import Repository
from franz.openrdf.query.query import QueryLanguage
from franz.miniclient import repository
from franz.openrdf.model.value import URI as FURI
from franz.openrdf.model.literal import Literal as FLiteral

class Store(object):
    
    def __init__(self, **confs):
        accessMode = Repository.RENEW
        
        self.port = str(confs.get('port', "8080"))
        self.user = confs.get('user','test')
        self.password = confs.get('password','xyzzy')
        self.dbname = confs.get('dbname','scratch')
    
        self.server = AllegroGraphServer("localhost", port=self.port,
                            user=self.user, password=self.password) 
        self.catalog = self.server.openCatalog(self.dbname)
        
        self.repo = self.catalog.getRepository(self.dbname, accessMode)
        self.repo.initialize()
        self.conn = self.repo.getConnection()
        
    def close(self):
        self.conn.close()
        
    def __len__(self):
        """ Returns size of the store """
        return self.conn.size()
    
    def add(self, triple):
        return self.conn.add(*map(lambda x:self._format(x), triple))
    
    def remove(self, triple):
        return self.conn.remove(*map(lambda x:self._format(x), triple))        
    
    def commit(self):
        return self.conn.commit()
    
    def triples(self, triple=(None,None,None)):
        for tri in self.conn.getStatements(*map(lambda x:self._format(x), triple)):
            s = self._rformat(tri.getSubject())
            p = self._rformat(tri.getPredicate())
            o = self._rformat(tri.getObject())
            yield (s,p,o)
    
    
    def query(self, q, initNs={}, initBindings={}):
        
        # prepare Namespaces
        for prefix in initNs.keys():
            self.conn.setNamespace(prefix, str(initNs[prefix]))
        
        query = q
        tupleQuery = self.conn.prepareTupleQuery(QueryLanguage.SPARQL, query)
        
        # prepare Bindings
        for var in initBindings.keys():
            tupleQuery.setBinding(var, self._format(initBindings[var]))
        
        for bindingSet in tupleQuery.evaluate():
            row = []
            for index in range(bindingSet.size()):
                row.append(self._rformat(bindingSet[index]))
            yield row        
    
    
    def _rformat(self, v):
        if isinstance(v, FURI):
            return URIRef(v.getURI())
        if isinstance(v, FLiteral):
            if str(v.datatype) != "<None>":
                return Literal(v.toPython(), datatype=str(v.datatype))
            else:
                return Literal(v.toPython())
        else:
            return Literal(str(v))
    
    def _format(self, el):
        if el == None:
            return None
        elif isinstance(el, FURI) or isinstance(el, FLiteral):
            return el
        elif isinstance(el, URIRef):
            return self.conn.createURI(str(el))
        elif isinstance(el, Literal):
            return self.conn.createLiteral(str(el), datatype=str(el.datatype))
        else:
            "Defaults to literal"
            return self.conn.createLiteral(str(el))

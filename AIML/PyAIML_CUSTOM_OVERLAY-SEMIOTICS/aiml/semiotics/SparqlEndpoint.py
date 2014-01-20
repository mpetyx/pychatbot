__author__ = 'mpetyx'

#
# def query():
#
#     return 1
#
#
# import rdflib
#
# rdflib.plugin.register('sparql', rdflib.query.Processor, 'rdfextras.sparql.processor', 'Processor')
# rdflib.plugin.register('sparql', rdflib.query.Result, 'rdfextras.sparql.query', 'SPARQLQueryResult')
#
# g = rdflib.Graph()
#
#
# # query = """
# #         SELECT *
# #         FROM <http://api.talis.com/stores/bbc-backstage/services/sparql>
# #         WHERE {
# #              ?s a http://purl.org/ontology/mo/MusicArtist;
# #                 http://www.w3.org/2002/07/owl#sameAs ?o .
# #         }Limit 50
# # """
#
# query = """
# prefix rdf:   <http://www.w3.org/1999/02/22-rdf-syntax-ns#>
# prefix atom:  <http://atomowl.org/ontologies/atomrdf#>
# prefix sioc:  <http://rdfs.org/sioc/ns#>
# prefix sioct: <http://rdfs.org/sioc/types#>
# select *
# from <http://demo.openlinksw.com/dataspace>
# where
#    {
#      ?forum rdf:type sioc:Community .
#      ?forum sioc:has_part ?parentf .
#      ?parentf sioc:container_of ?post.
#      optional { ?post atom:title  ?title } .
#      optional { ?post atom:author ?auth  } .
#    }
# limit 10
# """
#
# query = """
# SELECT ?s ?o WHERE {
#          ?s a <http://purl.org/ontology/mo/MusicArtist>;
#             <http://www.w3.org/2002/07/owl#sameAs> ?o .
#     } limit 50
# """
#
#
# gres = g.query(query)
# print gres
#
# for row in gres :
#     print "yo"
#     print row

#interesting article http://mousecradle.wordpress.com/2013/05/19/sparul-via-sparqlwrapper-and-virtuoso/

# from SPARQLWrapper import SPARQLWrapper, JSON
#
# query = """
# SELECT distinct ?subject
# FROM <http://dbpedia.org>
# {
#    ?subject rdfs:domain ?object .
#    <http://dbpedia.org/ontology/Band> rdfs:subClassOf ?object
#    OPTION (TRANSITIVE, t_distinct, t_step('step_no') as ?n, t_min (0) ).
# }"""
#
# sparql = SPARQLWrapper("http://dbpedia.org/sparql")
# sparql.setQuery(query)
# sparql.setReturnFormat(JSON)
# results = sparql.query().convert()
#
# for result in results["results"]["bindings"]:
#     print(result["subject"]["value"])



from SPARQLWrapper import SPARQLWrapper, JSON, XML, N3, RDF
sparql = SPARQLWrapper("http://dbpedia.org/sparql")
sparql.setQuery("""
    PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>
    SELECT ?label
    WHERE { <http://dbpedia.org/resource/Asturias> rdfs:label ?label }
""")
print '\n\n*** JSON Example'
sparql.setReturnFormat(JSON)
results = sparql.query().convert()
for result in results["results"]["bindings"]:
    print result["label"]["value"]

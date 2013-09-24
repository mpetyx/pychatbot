import lxml.etree as ET

xml_filename = "diary.xml" 
xsl_filename = "add_ids_to_elements.xsl"

to_rdf = "tei_with_relations_to_rdf.xsl"

dom = ET.parse(xml_filename)
#xslt = ET.parse(xsl_filename)
#transform = ET.XSLT(xslt)
#newdom = transform(dom)


#print(ET.tostring(newdom, pretty_print=True))


before_rdf = ET.parse(to_rdf)

final_xslt = ET.XSLT(before_rdf)
final_dom  = final_xslt(dom)

#print(ET.tostring(final_dom, pretty_print=True))


lol = open("kouklaki3.rdf","w")
#lol.write(ET.tostring(final_dom, pretty_print=True))
final_dom.write(lol, pretty_print=True)
lol.close()
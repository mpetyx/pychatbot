import lxml.etree as ET

xml_filename = "diary.xml" 


to_rdf = "make-acdc.xsl"

dom = ET.parse(xml_filename)

before_rdf = ET.parse(to_rdf)

final_xslt = ET.XSLT(before_rdf)
final_dom = final_xslt(dom)

#print(ET.tostring(final_dom, pretty_print=True))


lol = open("kouklaki3.rdf", "w")
#lol.write(ET.tostring(final_dom, pretty_print=True))
final_dom.write(lol, pretty_print=True)
lol.close()
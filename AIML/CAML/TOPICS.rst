---TOPIC MUSIC---

Do you like music?

[Positive, Affirmative]

Nice. I know a variety of songs! Would you like me to sing anything for you?
What kind of music do you like?


[yes|no]
if yes [gender] - > query[random|top][gender][lyrics]:
sing lyrics

from http://dbpedia.org

select distinct ?a ?name where { ?a <http://xmlns.com/foaf/0.1/name>  ?name.
?a <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://xmlns.com/foaf/0.1/Person>.
 FILTER(regex(str(?name), "Michael Jackson", "i"))
} LIMIT 100
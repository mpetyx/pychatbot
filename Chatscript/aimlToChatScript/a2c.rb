# a2c - AIML-to-ChatScript 
# Kevin Glover 
# 06 DEC 2012
#
# HOW IT WORKS:
# List the AIML file that you want to be translated as the inputFile. It reads through
# the AIML xml and outputs valid ChatScript.
# If you run it from the terminal, you can add the file to the argument there and it will
# print the ChatScript output to the terminal. 
#
#
# TODO:
# - SRAI tag is not completely implemented. It should use the ^reuse() tag, 
# I haven't figured out a good way to do it yet. 
# <srai> as if it was a <star>
# - Code clean up, this was my second attempt at creating something to 
# translate AIML to ChatScript. 
# - Adding more rules for special AIML tags like those found in AIMLpad
# 


require 'rubygems'
require 'xmlsimple'

inputFile = ARGV[0];

if inputFile == nil
	print "\nOops, something went wrong. \nPlease make sure to list the AMIL file path. \nExample: ruby a2c.rb foo.aiml\n\n"
	abort
else
  config = XmlSimple.xml_in(inputFile, { 'KeyAttr' => 'name' })
end

p config.keys()

p config["category"][0].keys()

p config["category"][0].length

p config["category"].length


##Begin def section


#This checks to see if there is a random call and converts it
def hasRandom(config_n)
	begin
		ranCheck = config_n["random"][0]["li"]
	rescue
		ranCheck =false
	else
		return ranCheck
	end
end

#This checks to see if there are any other dictionaries
def hasMoreDicts(config_n)
	begin
		ranCheck = config_n["content"]
	rescue
		ranCheck =false
	else
		return ranCheck
	end
end


#This does a check for the <SRAI> tag. If there is a reference is the response,
# ( ie <srai> Something <star/> ) 
# it is treated as a star right now... It should try to duplicate another response, 
#but ChatScript can't do ^reuse(something _0) or ^reuse(something _*)

def check_for_srai(response)
	begin
		response = response['srai'][0]
		
			begin
				if(response["star"])
					response = response["content"].to_s.capitalize + "_0." 
				else
					response = "^reuse(" + response.to_s.capitalize + ")"
					
				end
			rescue
				response = response
			end
	rescue
		response = response
	else
		return response
	end
end


#This checks for <star>
def check_for_star(response)
	begin
		if(response["star"])
			response = response["content"].push( "_0.")
		end
	rescue
		response = response
	else
		return response
	end
end


#This checks to see if there is a array in the random
def check_for_star_in_random(response)
		if tmp = Array.try_convert([response])
			if response[0].length != 1
				response = response[0]
			end
		else
			response = response
		end
	return response
end

##End of def section


#ChatScript format is stored as cs_format
cs_format = ''

for x in 1..config["category"].length

	nth = x-1

	pattern = config["category"][nth]["pattern"][0].to_s.downcase

	template = config["category"][nth]["template"][0]

	random_indicator = hasRandom(template)
	
	more_dicts = hasMoreDicts(template)
	
	#if there is a random AIML tag, we need to translate that differently to CS
	
	if (random_indicator)
	
		temp = ''
		
		for i in 1..hasRandom(template).length
		
			respn = config["category"][nth]["template"][0]["random"][0]["li"][i-1]
			
				if(respn["content"])
					respn = respn["content"]
				end
								
			respn = check_for_srai(respn);
			
			respn = check_for_star_in_random(respn)
			
			temp +='['
			temp += respn.to_s
			temp +=']'
			
			template= temp.to_s
		end
	
	#if there are embedded AIML rules we need to break those out
	
	elsif (more_dicts)
		temp = ''
		temp = more_dicts
			
			if (Array.try_convert(temp))
				temp = temp.join(" ").to_s.sub("   "," _0 ")
			end
		
		template = temp.to_s
	
	else
		template = check_for_srai(template)
		template = check_for_star(template)
				
		template = template.to_s

	end #end random_indicator if-else loop
	
	

	cs_format += "u: ("+pattern + ") " + template + "\n"

	
	# AIML <star> * becomes _* for ChatScript	
	begin
		cs_format[' *'] = " _*"
	rescue
	
	end

end #end for loop for categories


#prints formatted ChatScript into the console window.  
puts cs_format
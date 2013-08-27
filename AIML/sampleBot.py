__author__ = 'mpetyx'

from PyAIML_0_8_6 import aiml

# The Kernel object is the public interface to
# the AIML interpreter.
k = aiml.Kernel()

# k.loadBrain("sampleBrain.txt")
k.setBotPredicate("name", "koukli")
k.setBotPredicate("country", "Greece")

# Use the 'learn' method to load the contents
# of an AIML file into the Kernel.
# k.learn("std-startup.xml")
# k.learn("./AIML_FILES/selftest.aiml")
# k.learn("./ReversedAiml4Pandorabot/ra-learn-quest.aiml")

# load aiml files
directory = open("./aiml-en-us-foundation-alice/directory.txt", "r")
for _file_ in directory:
    _file_ = _file_.replace("\n","")
    k.learn("./aiml-en-us-foundation-alice/%s" % _file_)

# Use the 'respond' method to compute the response
# to a user's input string.  respond() returns
# the interpreter's response, which in this case
# we ignore.
# k.respond("load aiml b")

#saving the brain for later usage
#k.saveBrain("sampleBrain.txt")

# Loop forever, reading user input from the command
# line and printing responses.
while True: print k.respond(raw_input("> "))
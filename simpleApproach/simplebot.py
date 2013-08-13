__author__ = "mpetyx"


# import pickle,random
# a=open("lexicon-luke","rb")
# successorlist=pickle.load(a)
# a.close()
# def nextword(a):
#     if a in successorlist:
#         return random.choice(successorlist[a])
#     else:
#         return "the"
# speech=""
# while speech!="quit":
#     speech=raw_input(">")
#     s=random.choice(speech.split())
#     response=""
#     while True:
#         neword=nextword(s)
#         response+=" "+neword
#         s=neword
#         if neword[-1] in ",?!.":
#             break
#     print response



import pickle,random
a=open("lexicon-luke","rb")
successorlist=pickle.load(a)
a.close()

def nextword(a):
    if a in successorlist:
        return random.choice(successorlist[a])
    else:
        return ""
speech=""

while speech!='quit':
    speech=raw_input('> ')
    s=random.choice(speech.split())
    response=''
    i = 0
    while True:
        i = i + 1
        neword=nextword(s)
        if neword == s:
            i=11
        response+=' '+neword
        s=neword
        if response[-1] in ',?!.' or i > 10:
            break
        print response
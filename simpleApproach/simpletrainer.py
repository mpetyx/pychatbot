__author__ = 'mpetyx'

import pickle
b=open('simplebot.py')
text=[]
for line in b:
    for word in line.split():
        text.append (word)
b.close()
textset=list(set(text))
follow={}
# for l in range(len(textset)):
#     working=[]
#     check=textset[l]
#     for w in range(len(text)-1):
#         if check==text[w] and text[w][-1] not in '(),.?!':
#             working.append(str(text[w+1]))
#     follow[check]=working
for w in range(len(text)-1):
    check=text[w]
    next_word=text[w+1]
    if check[-1] not in '(),.?!':
        if follow.has_key(check):
            follow[check].append(next_word)
        else:
            follow[check]=[next_word]

a=open('lexicon-luke','wb')
pickle.dump(follow,a,2)
a.close()


# import pickle
# b=open('sample_text.txt')
# text={}
# all = b.read()
#
# for line in all.split('.'):
# l = line.split()
#
# for word in l:
# if word in text:
# t = text[word]
# t.append(line)
# text[word] = t
# else:
# text[word] = [line]
#
# b.close()
#
# a=open('lexicon-dict2','wb')
# pickle.dump(text,a,2)
# a.close()
# print "done"
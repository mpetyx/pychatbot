chat Package from nltk for python

A class for simple chatbots. These perform simple pattern matching on sentences typed by users, and respond with automatically generated sentences.

These chatbots may not work using the windows command line or the windows IDLE GUI.

nltk.chat.chatbots()[source]¶
eliza Module ¶

nltk.chat.eliza.demo()[source]¶
nltk.chat.eliza.eliza_chat()[source]¶
iesha Module ¶

This chatbot is a tongue-in-cheek take on the average teen anime junky that frequents YahooMessenger or MSNM. All spelling mistakes and flawed grammar are intentional.

nltk.chat.iesha.demo()[source]¶
nltk.chat.iesha.iesha_chat()[source]¶
rude Module ¶

nltk.chat.rude.demo()[source]¶
nltk.chat.rude.rude_chat()[source]¶
suntsu Module ¶

Tsu bot responds to all queries with a Sun Tsu sayings

Quoted from Sun Tsu’s The Art of War Translated by LIONEL GILES, M.A. 1910 Hosted by the Gutenberg Project http://www.gutenberg.org/

nltk.chat.suntsu.demo()[source]¶
nltk.chat.suntsu.suntsu_chat()[source]¶
util Module ¶

class nltk.chat.util.Chat(pairs, reflections={})[source]¶
Bases: object

converse(quit='quit')[source]¶
respond(str)[source]¶
Generate a response to the user input.

Parameters:	str (str) – The string to be mapped
Return type:	str
zen Module ¶

Zen Chatbot talks in gems of Zen wisdom.

This is a sample conversation with Zen Chatbot: ZC: Welcome, my child. me: Good afternoon. ZC: Ask the question you have come to ask. me: How can I achieve enlightenment? ZC: How do you suppose? me: Through meditation. ZC: Form is emptiness, and emptiness form. me: How can I empty my mind of worldly troubles? ZC: Will an answer to that really help in your search for enlightenment? me: Yes. ZC: It is better to be right than to be certain. me: I seek truth and wisdom. ZC: The search for truth is a long journey. me: Are you sure? ZC: Maybe sure, maybe not sure.

The chatbot structure is based on that of chat.eliza. Thus, it uses a translation table to convert from question to response i.e. “I am” –> “you are”

Of course, since Zen Chatbot does not understand the meaning of any words, responses are very limited. Zen Chatbot will usually answer very vaguely, or respond to a question by asking a different question, in much the same way as Eliza.

nltk.chat.zen.demo()[source]¶
nltk.chat.zen.zen_chat()[source]¶

page 2
chunk Package ¶

Classes and interfaces for identifying non-overlapping linguistic groups (such as base noun phrases) in unrestricted text. This task is called “chunk parsing” or “chunking”, and the identified groups are called “chunks”. The chunked text is represented using a shallow tree called a “chunk structure.” A chunk structure is a tree containing tokens and chunks, where each chunk is a subtree containing only tokens. For example, the chunk structure for base noun phrase chunks in the sentence “I saw the big dog on the hill” is:

(SENTENCE:
  (NP: <I>)
  <saw>
  (NP: <the> <big> <dog>)
  <on>
  (NP: <the> <hill>))
To convert a chunk structure back to a list of tokens, simply use the chunk structure’s leaves() method.

This module defines ChunkParserI, a standard interface for chunking texts; and RegexpChunkParser, a regular-expression based implementation of that interface. It also defines ChunkScore, a utility class for scoring chunk parsers.

RegexpChunkParser ¶

RegexpChunkParser is an implementation of the chunk parser interface that uses regular-expressions over tags to chunk a text. Its parse() method first constructs a ChunkString, which encodes a particular chunking of the input text. Initially, nothing is chunked. parse.RegexpChunkParser then applies a sequence of RegexpChunkRule rules to the ChunkString, each of which modifies the chunking that it encodes. Finally, the ChunkString is transformed back into a chunk structure, which is returned.

RegexpChunkParser can only be used to chunk a single kind of phrase. For example, you can use an RegexpChunkParser to chunk the noun phrases in a text, or the verb phrases in a text; but you can not use it to simultaneously chunk both noun phrases and verb phrases in the same text. (This is a limitation of RegexpChunkParser, not of chunk parsers in general.)

RegexpChunkRules ¶

A RegexpChunkRule is a transformational rule that updates the chunking of a text by modifying its ChunkString. Each RegexpChunkRule defines the apply() method, which modifies the chunking encoded by a ChunkString. The RegexpChunkRule class itself can be used to implement any transformational rule based on regular expressions. There are also a number of subclasses, which can be used to implement simpler types of rules:

ChunkRule chunks anything that matches a given regular expression.
ChinkRule chinks anything that matches a given regular expression.
UnChunkRule will un-chunk any chunk that matches a given regular expression.
MergeRule can be used to merge two contiguous chunks.
SplitRule can be used to split a single chunk into two smaller chunks.
ExpandLeftRule will expand a chunk to incorporate new unchunked material on the left.
ExpandRightRule will expand a chunk to incorporate new unchunked material on the right.
Tag Patterns ¶

A RegexpChunkRule uses a modified version of regular expression patterns, called “tag patterns”. Tag patterns are used to match sequences of tags. Examples of tag patterns are:

r'(<DT>|<JJ>|<NN>)+'
r'<NN>+'
r'<NN.*>'
The differences between regular expression patterns and tag patterns are:

In tag patterns, '<' and '>' act as parentheses; so '<NN>+' matches one or more repetitions of '<NN>', not '<NN' followed by one or more repetitions of '>'.
Whitespace in tag patterns is ignored. So '<DT> | <NN>' is equivalant to '<DT>|<NN>'
In tag patterns, '.' is equivalant to '[^{}<>]'; so '<NN.*>' matches any single tag starting with 'NN'.
The function tag_pattern2re_pattern can be used to transform a tag pattern to an equivalent regular expression pattern.
Efficiency ¶

Preliminary tests indicate that RegexpChunkParser can chunk at a rate of about 300 tokens/second, with a moderately complex rule set.

There may be problems if RegexpChunkParser is used with more than 5,000 tokens at a time. In particular, evaluation of some regular expressions may cause the Python regular expression engine to exceed its maximum recursion depth. We have attempted to minimize these problems, but it is impossible to avoid them completely. We therefore recommend that you apply the chunk parser to a single sentence at a time.
Emacs Tip ¶

If you evaluate the following elisp expression in emacs, it will colorize a ChunkString when you use an interactive python shell with emacs or xemacs (“C-c !”):

(let ()
  (defconst comint-mode-font-lock-keywords
    '(("<[^>]+>" 0 'font-lock-reference-face)
      ("[{}]" 0 'font-lock-function-name-face)))
  (add-hook 'comint-mode-hook (lambda () (turn-on-font-lock))))
You can evaluate this code by copying it to a temporary buffer, placing the cursor after the last close parenthesis, and typing “C-x C-e”. You should evaluate it before running the interactive session. The change will last until you close emacs.
Unresolved Issues ¶

If we use the re module for regular expressions, Python’s regular expression engine generates “maximum recursion depth exceeded” errors when processing very large texts, even for regular expressions that should not require any recursion. We therefore use the pre module instead. But note that pre does not include Unicode support, so this module will not work with unicode strings. Note also that pre regular expressions are not quite as advanced as re ones (e.g., no leftward zero-length assertions).

type CHUNK_TAG_PATTERN:
 	regexp
var CHUNK_TAG_PATTERN:
 	A regular expression to test whether a tag pattern is valid.
nltk.chunk.batch_ne_chunk(tagged_sentences, binary=False)[source]¶
Use NLTK’s currently recommended named entity chunker to chunk the given list of tagged sentences, each consisting of a list of tagged tokens.
nltk.chunk.ne_chunk(tagged_tokens, binary=False)[source]¶
Use NLTK’s currently recommended named entity chunker to chunk the given list of tagged tokens.

page 3
classify Package ¶

Classes and interfaces for labeling tokens with category labels (or “class labels”). Typically, labels are represented with strings (such as 'health' or 'sports'). Classifiers can be used to perform a wide range of classification tasks. For example, classifiers can be used...

to classify documents by topic
to classify ambiguous words by which word sense is intended
to classify acoustic signals by which phoneme they represent
to classify sentences by their author
Features ¶

In order to decide which category label is appropriate for a given token, classifiers examine one or more ‘features’ of the token. These “features” are typically chosen by hand, and indicate which aspects of the token are relevant to the classification decision. For example, a document classifier might use a separate feature for each word, recording how often that word occurred in the document.
Featuresets ¶

The features describing a token are encoded using a “featureset”, which is a dictionary that maps from “feature names” to “feature values”. Feature names are unique strings that indicate what aspect of the token is encoded by the feature. Examples include 'prevword', for a feature whose value is the previous word; and 'contains-word(library)' for a feature that is true when a document contains the word 'library'. Feature values are typically booleans, numbers, or strings, depending on which feature they describe.

Featuresets are typically constructed using a “feature detector” (also known as a “feature extractor”). A feature detector is a function that takes a token (and sometimes information about its context) as its input, and returns a featureset describing that token. For example, the following feature detector converts a document (stored as a list of words) to a featureset describing the set of words included in the document:

>>> # Define a feature detector function.
>>> def document_features(document):
...     return dict([('contains-word(%s)' % w, True) for w in document])
Feature detectors are typically applied to each token before it is fed to the classifier:

>>> # Classify each Gutenberg document.
>>> from nltk.corpus import gutenberg
>>> for fileid in gutenberg.fileids():
...     doc = gutenberg.words(fileid)
...     print fileid, classifier.classify(document_features(doc))
The parameters that a feature detector expects will vary, depending on the task and the needs of the feature detector. For example, a feature detector for word sense disambiguation (WSD) might take as its input a sentence, and the index of a word that should be classified, and return a featureset for that word. The following feature detector for WSD includes features describing the left and right contexts of the target word:

>>> def wsd_features(sentence, index):
...     featureset = {}
...     for i in range(max(0, index-3), index):
...         featureset['left-context(%s)' % sentence[i]] = True
...     for i in range(index, max(index+3, len(sentence))):
...         featureset['right-context(%s)' % sentence[i]] = True
...     return featureset
Training Classifiers ¶

Most classifiers are built by training them on a list of hand-labeled examples, known as the “training set”. Training sets are represented as lists of (featuredict, label) tuples.
api Module ¶

Interfaces for labeling tokens with category labels (or “class labels”).

ClassifierI is a standard interface for “single-category classification”, in which the set of categories is known, the number of categories is finite, and each text belongs to exactly one category.

MultiClassifierI is a standard interface for “multi-category classification”, which is like single-category classification except that each text belongs to zero or more categories.

class nltk.classify.api.ClassifierI[source]¶
Bases: object

A processing interface for labeling tokens with a single category label (or “class”). Labels are typically strs or ints, but can be any immutable type. The set of labels that the classifier chooses from must be fixed and finite.

Subclasses must define:
labels()
either classify() or batch_classify() (or both)
Subclasses may define:
either prob_classify() or batch_prob_classify() (or both)
batch_classify(featuresets)[source]¶
Apply self.classify() to each element of featuresets. I.e.:

return [self.classify(fs) for fs in featuresets]
Return type:	list(label)
batch_prob_classify(featuresets)[source]¶
Apply self.prob_classify() to each element of featuresets. I.e.:

return [self.prob_classify(fs) for fs in featuresets]
Return type:	list(ProbDistI)
classify(featureset)[source]¶
Returns:	the most appropriate label for the given featureset.
Return type:	label
labels()[source]¶
Returns:	the list of category labels used by this classifier.
Return type:	list of (immutable)
prob_classify(featureset)[source]¶
Returns:	a probability distribution over labels for the given featureset.
Return type:	ProbDistI
class nltk.classify.api.MultiClassifierI[source]¶
Bases: object

A processing interface for labeling tokens with zero or more category labels (or “labels”). Labels are typically strs or ints, but can be any immutable type. The set of labels that the multi-classifier chooses from must be fixed and finite.

Subclasses must define:
labels()
either classify() or batch_classify() (or both)
Subclasses may define:
either prob_classify() or batch_prob_classify() (or both)
batch_classify(featuresets)[source]¶
Apply self.classify() to each element of featuresets. I.e.:

return [self.classify(fs) for fs in featuresets]
Return type:	list(set(label))
batch_prob_classify(featuresets)[source]¶
Apply self.prob_classify() to each element of featuresets. I.e.:

return [self.prob_classify(fs) for fs in featuresets]
Return type:	list(ProbDistI)
classify(featureset)[source]¶
Returns:	the most appropriate set of labels for the given featureset.
Return type:	set(label)
labels()[source]¶
Returns:	the list of category labels used by this classifier.
Return type:	list of (immutable)
prob_classify(featureset)[source]¶
Returns:	a probability distribution over sets of labels for the given featureset.
Return type:	ProbDistI
decisiontree Module ¶

A classifier model that decides which label to assign to a token on the basis of a tree structure, where branches correspond to conditions on feature values, and leaves correspond to label assignments.

class nltk.classify.decisiontree.DecisionTreeClassifier(label, feature_name=None, decisions=None, default=None)[source]¶
Bases: nltk.classify.api.ClassifierI

static best_binary_stump(feature_names, labeled_featuresets, feature_values, verbose=False)[source]¶
static best_stump(feature_names, labeled_featuresets, verbose=False)[source]¶
static binary_stump(feature_name, feature_value, labeled_featuresets)[source]¶
classify(featureset)[source]¶
error(labeled_featuresets)[source]¶
labels()[source]¶
static leaf(labeled_featuresets)[source]¶
pp(width=70, prefix=u'', depth=4)[source]¶
Return a string containing a pretty-printed version of this decision tree. Each line in this string corresponds to a single decision tree node or leaf, and indentation is used to display the structure of the decision tree.
pseudocode(prefix=u'', depth=4)[source]¶
Return a string representation of this decision tree that expresses the decisions it makes as a nested set of pseudocode if statements.
refine(labeled_featuresets, entropy_cutoff, depth_cutoff, support_cutoff, binary=False, feature_values=None, verbose=False)[source]¶
static stump(feature_name, labeled_featuresets)[source]¶
static train(labeled_featuresets, entropy_cutoff=0.05, depth_cutoff=100, support_cutoff=10, binary=False, feature_values=None, verbose=False)[source]¶
Parameters:	binary – If true, then treat all feature/value pairs as
individual binary features, rather than using a single n-way branch for each feature.
unicode_repr¶
x.__repr__() <==> repr(x)
nltk.classify.decisiontree.demo()[source]¶
nltk.classify.decisiontree.f(x)[source]¶
mallet Module ¶

A set of functions used to interface with the external Mallet machine learning package. Before mallet can be used, you should tell NLTK where it can find the mallet package, using the config_mallet() function. Typical usage:

>>> from nltk.classify import mallet
>>> mallet.config_mallet() # pass path to mallet as argument if needed
[Found mallet: ...]
nltk.classify.mallet.call_mallet(cmd, classpath=None, stdin=None, stdout=None, stderr=None, blocking=True)[source]¶
Call nltk.internals.java with the given command, and with the classpath modified to include both nltk.jar and all the .jar files defined by Mallet.

See nltk.internals.java for parameter and return value descriptions.
nltk.classify.mallet.config_mallet(mallet_home=None)[source]¶
Configure NLTK’s interface to the Mallet machine learning package.

Parameters:	mallet_home (str) – The full path to the mallet directory. If not specified, then NLTK will search the system for a mallet directory; and if one is not found, it will raise a LookupError exception.
maxent Module ¶

A classifier model based on maximum entropy modeling framework. This framework considers all of the probability distributions that are empirically consistent with the training data; and chooses the distribution with the highest entropy. A probability distribution is “empirically consistent” with a set of training data if its estimated frequency with which a class and a feature vector value co-occur is equal to the actual frequency in the data.

Terminology: ‘feature ’¶

The term feature is usually used to refer to some property of an unlabeled token. For example, when performing word sense disambiguation, we might define a 'prevword' feature whose value is the word preceding the target word. However, in the context of maxent modeling, the term feature is typically used to refer to a property of a “labeled” token. In order to prevent confusion, we will introduce two distinct terms to disambiguate these two different concepts:

An “input-feature” is a property of an unlabeled token.
A “joint-feature” is a property of a labeled token.
In the rest of the nltk.classify module, the term “features” is used to refer to what we will call “input-features” in this module.

In literature that describes and discusses maximum entropy models, input-features are typically called “contexts”, and joint-features are simply referred to as “features”.

Converting Input-Features to Joint-Features ¶

In maximum entropy models, joint-features are required to have numeric values. Typically, each input-feature input_feat is mapped to a set of joint-features of the form:

joint_feat(token, label) = { 1 if input_feat(token) == feat_val
{ and label == some_label
{ 0 otherwise
For all values of feat_val and some_label. This mapping is performed by classes that implement the MaxentFeatureEncodingI interface.

class nltk.classify.maxent.BinaryMaxentFeatureEncoding(labels, mapping, unseen_features=False, alwayson_features=False)[source]¶
Bases: nltk.classify.maxent.MaxentFeatureEncodingI

A feature encoding that generates vectors containing a binary joint-features of the form:

joint_feat(fs, l) = { 1 if (fs[fname] == fval) and (l == label)
{ 0 otherwise
Where fname is the name of an input-feature, fval is a value for that input-feature, and label is a label.

Typically, these features are constructed based on a training corpus, using the train() method. This method will create one feature for each combination of fname, fval, and label that occurs at least once in the training corpus.

The unseen_features parameter can be used to add “unseen-value features”, which are used whenever an input feature has a value that was not encountered in the training corpus. These features have the form:

joint_feat(fs, l) = { 1 if is_unseen(fname, fs[fname])
{ and l == label
{ 0 otherwise
Where is_unseen(fname, fval) is true if the encoding does not contain any joint features that are true when fs[fname]==fval.

The alwayson_features parameter can be used to add “always-on features”, which have the form:

|  joint_feat(fs, l) = { 1 if (l == label)
|                      {
|                      { 0 otherwise
These always-on features allow the maxent model to directly model the prior probabilities of each label.

describe(f_id)[source]¶
encode(featureset, label)[source]¶
labels()[source]¶
length()[source]¶
classmethod train(train_toks, count_cutoff=0, labels=None, **options)[source]¶
Construct and return new feature encoding, based on a given training corpus train_toks. See the class description BinaryMaxentFeatureEncoding for a description of the joint-features that will be included in this encoding.

Parameters:
train_toks (list(tuple(dict, str))) – Training data, represented as a list of pairs, the first member of which is a feature dictionary, and the second of which is a classification label.
count_cutoff (int) – A cutoff value that is used to discard rare joint-features. If a joint-feature’s value is 1 fewer than count_cutoff times in the training corpus, then that joint-feature is not included in the generated encoding.
labels (list) – A list of labels that should be used by the classifier. If not specified, then the set of labels attested in train_toks will be used.
options – Extra parameters for the constructor, such as unseen_features and alwayson_features.
nltk.classify.maxent.ConditionalExponentialClassifier¶
Alias for MaxentClassifier.

alias of MaxentClassifier
class nltk.classify.maxent.FunctionBackedMaxentFeatureEncoding(func, length, labels)[source]¶
Bases: nltk.classify.maxent.MaxentFeatureEncodingI

A feature encoding that calls a user-supplied function to map a given featureset/label pair to a sparse joint-feature vector.

describe(fid)[source]¶
encode(featureset, label)[source]¶
labels()[source]¶
length()[source]¶
class nltk.classify.maxent.GISEncoding(labels, mapping, unseen_features=False, alwayson_features=False, C=None)[source]¶
Bases: nltk.classify.maxent.BinaryMaxentFeatureEncoding

A binary feature encoding which adds one new joint-feature to the joint-features defined by BinaryMaxentFeatureEncoding: a correction feature, whose value is chosen to ensure that the sparse vector always sums to a constant non-negative number. This new feature is used to ensure two preconditions for the GIS training algorithm:

At least one feature vector index must be nonzero for every token.
The feature vector must sum to a constant non-negative number for every token.
C[source]¶
The non-negative constant that all encoded feature vectors will sum to.
describe(f_id)[source]¶
encode(featureset, label)[source]¶
length()[source]¶
class nltk.classify.maxent.MaxentClassifier(encoding, weights, logarithmic=True)[source]¶
Bases: nltk.classify.api.ClassifierI

A maximum entropy classifier (also known as a “conditional exponential classifier”). This classifier is parameterized by a set of “weights”, which are used to combine the joint-features that are generated from a featureset by an “encoding”. In particular, the encoding maps each (featureset, label) pair to a vector. The probability of each label is then computed using the following equation:

                          dotprod(weights, encode(fs,label))
prob(fs|label) = ---------------------------------------------------
                 sum(dotprod(weights, encode(fs,l)) for l in labels)
Where dotprod is the dot product:

dotprod(a,b) = sum(x*y for (x,y) in zip(a,b))
ALGORITHMS = [u'GIS', u'IIS', u'MEGAM', u'TADM']¶
A list of the algorithm names that are accepted for the train() method’s algorithm parameter.
classify(featureset)[source]¶
explain(featureset, columns=4)[source]¶
Print a table showing the effect of each of the features in the given feature set, and how they combine to determine the probabilities of each label for that featureset.
labels()[source]¶
prob_classify(featureset)[source]¶
set_weights(new_weights)[source]¶
Set the feature weight vector for this classifier. :param new_weights: The new feature weight vector. :type new_weights: list of float
show_most_informative_features(n=10, show=u'all')[source]¶
Parameters:	show – all, neg, or pos (for negative-only or positive-only)
classmethod train(train_toks, algorithm=None, trace=3, encoding=None, labels=None, sparse=None, gaussian_prior_sigma=0, **cutoffs)[source]¶
Train a new maxent classifier based on the given corpus of training samples. This classifier will have its weights chosen to maximize entropy while remaining empirically consistent with the training corpus.

Return type:
MaxentClassifier
Returns:
The new maxent classifier
Parameters:
train_toks (list) – Training data, represented as a list of pairs, the first member of which is a featureset, and the second of which is a classification label.
algorithm (str) –
A case-insensitive string, specifying which algorithm should be used to train the classifier. The following algorithms are currently available.

Iterative Scaling Methods: Generalized Iterative Scaling ('GIS'), Improved Iterative Scaling ('IIS')
External Libraries (requiring megam): LM-BFGS algorithm, with training performed by Megam ('megam')
The default algorithm is 'IIS'.
trace (int) – The level of diagnostic tracing output to produce. Higher values produce more verbose output.
encoding (MaxentFeatureEncodingI) – A feature encoding, used to convert featuresets into feature vectors. If none is specified, then a BinaryMaxentFeatureEncoding will be built based on the features that are attested in the training corpus.
labels (list(str)) – The set of possible labels. If none is given, then the set of all labels attested in the training data will be used instead.
gaussian_prior_sigma – The sigma value for a gaussian prior on model weights. Currently, this is supported by megam. For other algorithms, its value is ignored.
cutoffs –
Arguments specifying various conditions under which the training should be halted. (Some of the cutoff conditions are not supported by some algorithms.)

max_iter=v: Terminate after v iterations.
min_ll=v: Terminate after the negative average log-likelihood drops under v.
min_lldelta=v: Terminate if a single iteration improves log likelihood by less than v.
unicode_repr()¶
weights()[source]¶
Returns:	The feature weight vector for this classifier.
Return type:	list of float
class nltk.classify.maxent.MaxentFeatureEncodingI[source]¶
Bases: object

A mapping that converts a set of input-feature values to a vector of joint-feature values, given a label. This conversion is necessary to translate featuresets into a format that can be used by maximum entropy models.

The set of joint-features used by a given encoding is fixed, and each index in the generated joint-feature vectors corresponds to a single joint-feature. The length of the generated joint-feature vectors is therefore constant (for a given encoding).

Because the joint-feature vectors generated by MaxentFeatureEncodingI are typically very sparse, they are represented as a list of (index, value) tuples, specifying the value of each non-zero joint-feature.

Feature encodings are generally created using the train() method, which generates an appropriate encoding based on the input-feature values and labels that are present in a given corpus.

describe(fid)[source]¶
Returns:	A string describing the value of the joint-feature whose index in the generated feature vectors is fid.
Return type:	str
encode(featureset, label)[source]¶
Given a (featureset, label) pair, return the corresponding vector of joint-feature values. This vector is represented as a list of (index, value) tuples, specifying the value of each non-zero joint-feature.

Return type:	list(tuple(int, int))
labels()[source]¶
Returns:	A list of the “known labels” – i.e., all labels l such that self.encode(fs,l) can be a nonzero joint-feature vector for some value of fs.
Return type:	list
length()[source]¶
Returns:	The size of the fixed-length joint-feature vectors that are generated by this encoding.
Return type:	int
train(train_toks)[source]¶
Construct and return new feature encoding, based on a given training corpus train_toks.

Parameters:	train_toks (list(tuple(dict, str))) – Training data, represented as a list of pairs, the first member of which is a feature dictionary, and the second of which is a classification label.
class nltk.classify.maxent.TadmEventMaxentFeatureEncoding(labels, mapping, unseen_features=False, alwayson_features=False)[source]¶
Bases: nltk.classify.maxent.BinaryMaxentFeatureEncoding

describe(fid)[source]¶
encode(featureset, label)[source]¶
labels()[source]¶
length()[source]¶
classmethod train(train_toks, count_cutoff=0, labels=None, **options)[source]¶
class nltk.classify.maxent.TadmMaxentClassifier(encoding, weights, logarithmic=True)[source]¶
Bases: nltk.classify.maxent.MaxentClassifier

classmethod train(train_toks, **kwargs)[source]¶
class nltk.classify.maxent.TypedMaxentFeatureEncoding(labels, mapping, unseen_features=False, alwayson_features=False)[source]¶
Bases: nltk.classify.maxent.MaxentFeatureEncodingI

A feature encoding that generates vectors containing integer, float and binary joint-features of the form:

Binary (for string and boolean features):

joint_feat(fs, l) = { 1 if (fs[fname] == fval) and (l == label)
{ 0 otherwise
Value (for integer and float features):

joint_feat(fs, l) = { fval if (fs[fname] == type(fval))
{ and (l == label)
{ not encoded otherwise
Where fname is the name of an input-feature, fval is a value for that input-feature, and label is a label.

Typically, these features are constructed based on a training corpus, using the train() method.

For string and boolean features [type(fval) not in (int, float)] this method will create one feature for each combination of fname, fval, and label that occurs at least once in the training corpus.

For integer and float features [type(fval) in (int, float)] this method will create one feature for each combination of fname and label that occurs at least once in the training corpus.

For binary features the unseen_features parameter can be used to add “unseen-value features”, which are used whenever an input feature has a value that was not encountered in the training corpus. These features have the form:

joint_feat(fs, l) = { 1 if is_unseen(fname, fs[fname])
{ and l == label
{ 0 otherwise
Where is_unseen(fname, fval) is true if the encoding does not contain any joint features that are true when fs[fname]==fval.

The alwayson_features parameter can be used to add “always-on features”, which have the form:

joint_feat(fs, l) = { 1 if (l == label)
{ 0 otherwise
These always-on features allow the maxent model to directly model the prior probabilities of each label.

describe(f_id)[source]¶
encode(featureset, label)[source]¶
labels()[source]¶
length()[source]¶
classmethod train(train_toks, count_cutoff=0, labels=None, **options)[source]¶
Construct and return new feature encoding, based on a given training corpus train_toks. See the class description TypedMaxentFeatureEncoding for a description of the joint-features that will be included in this encoding.

Note: recognized feature values types are (int, float), over types are interpreted as regular binary features.

Parameters:
train_toks (list(tuple(dict, str))) – Training data, represented as a list of pairs, the first member of which is a feature dictionary, and the second of which is a classification label.
count_cutoff (int) – A cutoff value that is used to discard rare joint-features. If a joint-feature’s value is 1 fewer than count_cutoff times in the training corpus, then that joint-feature is not included in the generated encoding.
labels (list) – A list of labels that should be used by the classifier. If not specified, then the set of labels attested in train_toks will be used.
options – Extra parameters for the constructor, such as unseen_features and alwayson_features.
nltk.classify.maxent.calculate_deltas(train_toks, classifier, unattested, ffreq_empirical, nfmap, nfarray, nftranspose, encoding)[source]¶
Calculate the update values for the classifier weights for this iteration of IIS. These update weights are the value of delta that solves the equation:

ffreq_empirical[i]
       =
SUM[fs,l] (classifier.prob_classify(fs).prob(l) *
           feature_vector(fs,l)[i] *
           exp(delta[i] * nf(feature_vector(fs,l))))
Where:
(fs,l) is a (featureset, label) tuple from train_toks
feature_vector(fs,l) = encoding.encode(fs,l)
nf(vector) = sum([val for (id,val) in vector])
This method uses Newton’s method to solve this equation for delta[i]. In particular, it starts with a guess of delta[i] = 1; and iteratively updates delta with:

delta[i] -= (ffreq_empirical[i] - sum1[i])/(-sum2[i])
until convergence, where sum1 and sum2 are defined as:

sum1[i](delta) = SUM[fs,l] f[i](fs,l,delta)
sum2[i](delta) = SUM[fs,l] (f[i](fs,l,delta).nf(feature_vector(fs,l)))
f[i](fs,l,delta) = (classifier.prob_classify(fs).prob(l) .
feature_vector(fs,l)[i] .
exp(delta[i] . nf(feature_vector(fs,l))))
Note that sum1 and sum2 depend on delta; so they need to be re-computed each iteration.

The variables nfmap, nfarray, and nftranspose are used to generate a dense encoding for nf(ltext). This allows _deltas to calculate sum1 and sum2 using matrices, which yields a significant performance improvement.

Parameters:
train_toks (list(tuple(dict, str))) – The set of training tokens.
classifier (ClassifierI) – The current classifier.
ffreq_empirical (sequence of float) – An array containing the empirical frequency for each feature. The ith element of this array is the empirical frequency for feature i.
unattested (sequence of int) – An array that is 1 for features that are not attested in the training data; and 0 for features that are attested. In other words, unattested[i]==0 iff ffreq_empirical[i]==0.
nfmap (dict(int -> int)) – A map that can be used to compress nf to a dense vector.
nfarray (array(float)) – An array that can be used to uncompress nf from a dense vector.
nftranspose (array(float)) – The transpose of nfarray
nltk.classify.maxent.calculate_empirical_fcount(train_toks, encoding)[source]¶
nltk.classify.maxent.calculate_estimated_fcount(classifier, train_toks, encoding)[source]¶
nltk.classify.maxent.calculate_nfmap(train_toks, encoding)[source]¶
Construct a map that can be used to compress nf (which is typically sparse).

nf(feature_vector) is the sum of the feature values for feature_vector.

This represents the number of features that are active for a given labeled text. This method finds all values of nf(t) that are attested for at least one token in the given list of training tokens; and constructs a dictionary mapping these attested values to a continuous range 0...N. For example, if the only values of nf() that were attested were 3, 5, and 7, then _nfmap might return the dictionary {3:0, 5:1, 7:2}.

Returns:	A map that can be used to compress nf to a dense vector.
Return type:	dict(int -> int)
nltk.classify.maxent.demo()[source]¶
nltk.classify.maxent.train_maxent_classifier_with_gis(train_toks, trace=3, encoding=None, labels=None, **cutoffs)[source]¶
Train a new ConditionalExponentialClassifier, using the given training samples, using the Generalized Iterative Scaling algorithm. This ConditionalExponentialClassifier will encode the model that maximizes entropy from all the models that are empirically consistent with train_toks.

See :	train_maxent_classifier() for parameter descriptions.
nltk.classify.maxent.train_maxent_classifier_with_iis(train_toks, trace=3, encoding=None, labels=None, **cutoffs)[source]¶
Train a new ConditionalExponentialClassifier, using the given training samples, using the Improved Iterative Scaling algorithm. This ConditionalExponentialClassifier will encode the model that maximizes entropy from all the models that are empirically consistent with train_toks.

See :	train_maxent_classifier() for parameter descriptions.
nltk.classify.maxent.train_maxent_classifier_with_megam(train_toks, trace=3, encoding=None, labels=None, gaussian_prior_sigma=0, **kwargs)[source]¶
Train a new ConditionalExponentialClassifier, using the given training samples, using the external megam library. This ConditionalExponentialClassifier will encode the model that maximizes entropy from all the models that are empirically consistent with train_toks.

See :	train_maxent_classifier() for parameter descriptions.
See :	nltk.classify.megam
megam Module ¶

A set of functions used to interface with the external megam maxent optimization package. Before megam can be used, you should tell NLTK where it can find the megam binary, using the config_megam() function. Typical usage:

>>> from nltk.classify import megam
>>> megam.config_megam() # pass path to megam if not found in PATH
[Found megam: ...]
Use with MaxentClassifier. Example below, see MaxentClassifier documentation for details.

nltk.classify.MaxentClassifier.train(corpus, ‘megam’)
nltk.classify.megam.call_megam(args)[source]¶
Call the megam binary with the given arguments.
nltk.classify.megam.config_megam(bin=None)[source]¶
Configure NLTK’s interface to the megam maxent optimization package.

Parameters:	bin (str) – The full path to the megam binary. If not specified, then nltk will search the system for a megam binary; and if one is not found, it will raise a LookupError exception.
nltk.classify.megam.parse_megam_weights(s, features_count, explicit=True)[source]¶
Given the stdout output generated by megam when training a model, return a numpy array containing the corresponding weight vector. This function does not currently handle bias features.
nltk.classify.megam.write_megam_file(train_toks, encoding, stream, bernoulli=True, explicit=True)[source]¶
Generate an input file for megam based on the given corpus of classified tokens.

Parameters:
train_toks (list(tuple(dict, str))) – Training data, represented as a list of pairs, the first member of which is a feature dictionary, and the second of which is a classification label.
encoding (MaxentFeatureEncodingI) – A feature encoding, used to convert featuresets into feature vectors. May optionally implement a cost() method in order to assign different costs to different class predictions.
stream (stream) – The stream to which the megam input file should be written.
bernoulli – If true, then use the ‘bernoulli’ format. I.e., all joint features have binary values, and are listed iff they are true. Otherwise, list feature values explicitly. If bernoulli=False, then you must call megam with the -fvals option.
explicit – If true, then use the ‘explicit’ format. I.e., list the features that would fire for any of the possible labels, for each token. If explicit=True, then you must call megam with the -explicit option.
naivebayes Module ¶

A classifier based on the Naive Bayes algorithm. In order to find the probability for a label, this algorithm first uses the Bayes rule to express P(label|features) in terms of P(label) and P(features|label):

P(label) * P(features|label)
P(label|features) = ——————————
P(features)
The algorithm then makes the ‘naive’ assumption that all features are independent, given the label:

P(label) * P(f1|label) * ... * P(fn|label)
P(label|features) = ——————————————–
P(features)
Rather than computing P(featues) explicitly, the algorithm just calculates the denominator for each label, and normalizes them so they sum to one:

P(label) * P(f1|label) * ... * P(fn|label)
P(label|features) = ——————————————–
SUM[l]( P(l) * P(f1|l) * ... * P(fn|l) )
class nltk.classify.naivebayes.NaiveBayesClassifier(label_probdist, feature_probdist)[source]¶
Bases: nltk.classify.api.ClassifierI

A Naive Bayes classifier. Naive Bayes classifiers are paramaterized by two probability distributions:

P(label) gives the probability that an input will receive each label, given no information about the input’s features.
P(fname=fval|label) gives the probability that a given feature (fname) will receive a given value (fval), given that the label (label).
If the classifier encounters an input with a feature that has never been seen with any label, then rather than assigning a probability of 0 to all labels, it will ignore that feature.

The feature value ‘None’ is reserved for unseen feature values; you generally should not use ‘None’ as a feature value for one of your own features.

classify(featureset)[source]¶
labels()[source]¶
most_informative_features(n=100)[source]¶
Return a list of the ‘most informative’ features used by this classifier. For the purpose of this function, the informativeness of a feature (fname,fval) is equal to the highest value of P(fname=fval|label), for any label, divided by the lowest value of P(fname=fval|label), for any label:

max[ P(fname=fval|label1) / P(fname=fval|label2) ]
prob_classify(featureset)[source]¶
show_most_informative_features(n=10)[source]¶
static train(labeled_featuresets, estimator=<class 'nltk.probability.ELEProbDist'>)[source]¶
Parameters:	labeled_featuresets – A list of classified featuresets,
i.e., a list of tuples (featureset, label).
nltk.classify.naivebayes.demo()[source]¶
positivenaivebayes Module ¶

A variant of the Naive Bayes Classifier that performs binary classification with partially-labeled training sets. In other words, assume we want to build a classifier that assigns each example to one of two complementary classes (e.g., male names and female names). If we have a training set with labeled examples for both classes, we can use a standard Naive Bayes Classifier. However, consider the case when we only have labeled examples for one of the classes, and other, unlabeled, examples. Then, assuming a prior distribution on the two labels, we can use the unlabeled set to estimate the frequencies of the various features.

Let the two possible labels be 1 and 0, and let’s say we only have examples labeled 1 and unlabeled examples. We are also given an estimate of P(1).

We compute P(feature|1) exactly as in the standard case.

To compute P(feature|0), we first estimate P(feature) from the unlabeled set (we are assuming that the unlabeled examples are drawn according to the given prior distribution) and then express the conditional probability as:

P(feature) - P(feature|1) * P(1)
P(feature|0) = ———————————-
Example:

>>> from nltk.classify import PositiveNaiveBayesClassifier
Some sentences about sports:

>>> sports_sentences = [ 'The team dominated the game',
...                      'They lost the ball',
...                      'The game was intense',
...                      'The goalkeeper catched the ball',
...                      'The other team controlled the ball' ]
Mixed topics, including sports:

>>> various_sentences = [ 'The President did not comment',
...                       'I lost the keys',
...                       'The team won the game',
...                       'Sara has two kids',
...                       'The ball went off the court',
...                       'They had the ball for the whole game',
...                       'The show is over' ]
The features of a sentence are simply the words it contains:

>>> def features(sentence):
...     words = sentence.lower().split()
...     return dict(('contains(%s)' % w, True) for w in words)
We use the sports sentences as positive examples, the mixed ones ad unlabeled examples:

>>> positive_featuresets = list(map(features, sports_sentences))
>>> unlabeled_featuresets = list(map(features, various_sentences))
>>> classifier = PositiveNaiveBayesClassifier.train(positive_featuresets,
...                                                 unlabeled_featuresets)
Is the following sentence about sports?

>>> classifier.classify(features('The cat is on the table'))
False
What about this one?

>>> classifier.classify(features('My team lost the game'))
True
class nltk.classify.positivenaivebayes.PositiveNaiveBayesClassifier(label_probdist, feature_probdist)[source]¶
Bases: nltk.classify.naivebayes.NaiveBayesClassifier

static train(positive_featuresets, unlabeled_featuresets, positive_prob_prior=0.5, estimator=<class 'nltk.probability.ELEProbDist'>)[source]¶
Parameters:
positive_featuresets – A list of featuresets that are known as positive examples (i.e., their label is True).
unlabeled_featuresets – A list of featuresets whose label is unknown.
positive_prob_prior – A prior estimate of the probability of the label True (default 0.5).
nltk.classify.positivenaivebayes.demo()[source]¶
rte_classify Module ¶

Simple classifier for RTE corpus.

It calculates the overlap in words and named entities between text and hypothesis, and also whether there are words / named entities in the hypothesis which fail to occur in the text, since this is an indicator that the hypothesis is more informative than (i.e not entailed by) the text.

TO DO: better Named Entity classification TO DO: add lemmatization

class nltk.classify.rte_classify.RTEFeatureExtractor(rtepair, stop=True, lemmatize=False)[source]¶
Bases: object

This builds a bag of words for both the text and the hypothesis after throwing away some stopwords, then calculates overlap and difference.

hyp_extra(toktype, debug=True)[source]¶
Compute the extraneous material in the hypothesis.

Parameters:	toktype (‘ne’ or ‘word’) – distinguish Named Entities from ordinary words
overlap(toktype, debug=False)[source]¶
Compute the overlap between text and hypothesis.

Parameters:	toktype (‘ne’ or ‘word’) – distinguish Named Entities from ordinary words
nltk.classify.rte_classify.demo()[source]¶
nltk.classify.rte_classify.demo_feature_extractor()[source]¶
nltk.classify.rte_classify.demo_features()[source]¶
nltk.classify.rte_classify.lemmatize(word)[source]¶
Use morphy from WordNet to find the base form of verbs.
nltk.classify.rte_classify.ne(token)[source]¶
This just assumes that words in all caps or titles are named entities.

nltk.classify.rte_classify.rte_classifier(trainer, features=<function rte_features at 0x1103f7758>)[source]¶
Classify RTEPairs
nltk.classify.rte_classify.rte_features(rtepair)[source]¶
scikitlearn Module ¶

scikit-learn (http://scikit-learn.org ) is a machine learning library for Python, supporting most of the basic classification algorithms, including SVMs, Naive Bayes, logistic regression and decision trees.

This package implement a wrapper around scikit-learn classifiers. To use this wrapper, construct a scikit-learn classifier, then use that to construct a SklearnClassifier. E.g., to wrap a linear SVM classifier with default settings, do

>>> from sklearn.svm.sparse import LinearSVC
>>> from nltk.classify.scikitlearn import SklearnClassifier
>>> classif = SklearnClassifier(LinearSVC())
The scikit-learn classifier may be arbitrarily complex. E.g., the following constructs and wraps a Naive Bayes estimator with tf-idf weighting and chi-square feature selection:

>>> from sklearn.feature_extraction.text import TfidfTransformer
>>> from sklearn.feature_selection import SelectKBest, chi2
>>> from sklearn.naive_bayes import MultinomialNB
>>> from sklearn.pipeline import Pipeline
>>> pipeline = Pipeline([('tfidf', TfidfTransformer()),
...                      ('chi2', SelectKBest(chi2, k=1000)),
...                      ('nb', MultinomialNB())])
>>> classif = SklearnClassifier(pipeline)
(Such a classifier could be trained on word counts for text classification.)

class nltk.classify.scikitlearn.SklearnClassifier(estimator, dtype=<type 'float'>, sparse=True)[source]¶
Bases: nltk.classify.api.ClassifierI

Wrapper for scikit-learn classifiers.

batch_classify(featuresets)[source]¶
batch_prob_classify(featuresets)[source]¶
labels()[source]¶
train(labeled_featuresets)[source]¶
Train (fit) the scikit-learn estimator.

Parameters:	labeled_featuresets – A list of classified featuresets, i.e., a list of tuples (featureset, label).
unicode_repr()¶
nltk.classify.scikitlearn.setup_module(module)[source]¶
svm Module ¶

A classifier based on a support vector machine. This code uses Thorsten Joachims’ SVM^light implementation (http://svmlight.joachims.org/ ), wrapped using PySVMLight (https://bitbucket.org/wcauchois/pysvmlight ). The default settings are to train a linear classification kernel, though through minor modification, full SVMlight capabilities should be accessible if needed. Only binary classification is possible at present.

class nltk.classify.svm.SvmClassifier(labels, labelmapping, svmfeatures, model=None)[source]¶
Bases: nltk.classify.api.ClassifierI

A Support Vector Machine classifier. To explain briefly, support vector machines (SVM) treat each feature as a dimension, and position features in n-dimensional feature space. An optimal hyperplane is then determined that best divides feature space into classes, and future instances classified based on which side of the hyperplane they lie on, and their proximity to it.

This implementation is for a binary SVM - that is, only two classes are supported. You may achieve perform classification with more classes by training an SVM per class and then picking a best option for new instances given results from each binary class-SVM.

classify(featureset)[source]¶
Use a trained SVM to predict a label given for an unlabelled instance

Parameters:	featureset – a dict of feature/value pairs in NLTK format, representing a single instance
labels()[source]¶
Return the list of class labels.
prob_classify(featureset)[source]¶
Return a probability distribution of classifications

Parameters:	featureset – a dict of feature/value pairs in NLTK format, representing a single instance
resolve_prediction(prediction)[source]¶
resolve a float (in this case, probably from svmlight.learn().classify()) to either -1 or +1, and then look up the label for that class in _labelmapping, and return the text label

Parameters:	prediction – a signed float describing classifier confidence
svm_label_name(label)[source]¶
searches values of _labelmapping to resolve +1 or -1 to a string

Parameters:	label – the string label to look up
static train(featuresets)[source]¶
given a set of training instances in nltk format: [ ( {feature:value, ..}, str(label) ) ] train a support vector machine

Parameters:	featuresets – training instances
nltk.classify.svm.demo()[source]¶
nltk.classify.svm.featurename(feature, value)[source]¶
Parameters:
feature – a string denoting a feature name
value – the value of the feature
nltk.classify.svm.map_features_to_svm(features, svmfeatureindex)[source]¶
Parameters:
features – a dict of features in the format {‘feature’:value}
svmfeatureindex – a mapping from feature:value pairs to integer SVMlight feature labels
nltk.classify.svm.map_instance_to_svm(instance, labelmapping, svmfeatureindex)[source]¶
Parameters:
instance – an NLTK format instance, which is in the tuple format (dict(), label), where the dict contains feature:value pairs, and the label signifies the target attribute’s value for this instance (e.g. its class)
labelmapping – a previously-defined dict mapping from text labels in the NLTK instance format to SVMlight labels of either +1 or -1
@svmfeatureindex: a mapping from feature:value pairs to integer SVMlight feature labels
tadm Module ¶

nltk.classify.tadm.call_tadm(args)[source]¶
Call the tadm binary with the given arguments.
nltk.classify.tadm.config_tadm(bin=None)[source]¶
nltk.classify.tadm.encoding_demo()[source]¶
nltk.classify.tadm.names_demo()[source]¶
nltk.classify.tadm.parse_tadm_weights(paramfile)[source]¶
Given the stdout output generated by tadm when training a model, return a numpy array containing the corresponding weight vector.
nltk.classify.tadm.write_tadm_file(train_toks, encoding, stream)[source]¶
Generate an input file for tadm based on the given corpus of classified tokens.

Parameters:
train_toks (list(tuple(dict, str))) – Training data, represented as a list of pairs, the first member of which is a feature dictionary, and the second of which is a classification label.
encoding (TadmEventMaxentFeatureEncoding) – A feature encoding, used to convert featuresets into feature vectors.
stream (stream) – The stream to which the tadm input file should be written.
util Module ¶

Utility functions and classes for classifiers.

class nltk.classify.util.CutoffChecker(cutoffs)[source]¶
Bases: object

A helper class that implements cutoff checks based on number of iterations and log likelihood.

Accuracy cutoffs are also implemented, but they’re almost never a good idea to use.

check(classifier, train_toks)[source]¶
nltk.classify.util.accuracy(classifier, gold)[source]¶
nltk.classify.util.apply_features(feature_func, toks, labeled=None)[source]¶
Use the LazyMap class to construct a lazy list-like object that is analogous to map(feature_func, toks). In particular, if labeled=False, then the returned list-like object’s values are equal to:

[feature_func(tok) for tok in toks]
If labeled=True, then the returned list-like object’s values are equal to:

[(feature_func(tok), label) for (tok, label) in toks]
The primary purpose of this function is to avoid the memory overhead involved in storing all the featuresets for every token in a corpus. Instead, these featuresets are constructed lazily, as-needed. The reduction in memory overhead can be especially significant when the underlying list of tokens is itself lazy (as is the case with many corpus readers).

Parameters:
feature_func – The function that will be applied to each token. It should return a featureset – i.e., a dict mapping feature names to feature values.
toks – The list of tokens to which feature_func should be applied. If labeled=True, then the list elements will be passed directly to feature_func(). If labeled=False, then the list elements should be tuples (tok,label), and tok will be passed to feature_func().
labeled – If true, then toks contains labeled tokens – i.e., tuples of the form (tok, label). (Default: auto-detect based on types.)
nltk.classify.util.attested_labels(tokens)[source]¶
Returns:	A list of all labels that are attested in the given list of tokens.
Return type:	list of (immutable)
Parameters:	tokens (list) – The list of classified tokens from which to extract labels. A classified token has the form (token, label).
nltk.classify.util.binary_names_demo_features(name)[source]¶
nltk.classify.util.log_likelihood(classifier, gold)[source]¶
nltk.classify.util.names_demo(trainer, features=<function names_demo_features at 0x1103f7320>)[source]¶
nltk.classify.util.names_demo_features(name)[source]¶
nltk.classify.util.partial_names_demo(trainer, features=<function names_demo_features at 0x1103f7320>)[source]¶
nltk.classify.util.wsd_demo(trainer, word, features, n=1000)[source]¶
weka Module ¶

Classifiers that make use of the external ‘Weka’ package.

class nltk.classify.weka.ARFF_Formatter(labels, features)[source]¶
Converts featuresets and labeled featuresets to ARFF-formatted strings, appropriate for input into Weka.

Features and classes can be specified manually in the constructor, or may be determined from data using from_train.

data_section(tokens, labeled=None)[source]¶
Returns the ARFF data section for the given data.

Parameters:
tokens – a list of featuresets (dicts) or labelled featuresets which are tuples (featureset, label).
labeled – Indicates whether the given tokens are labeled or not. If None, then the tokens will be assumed to be labeled if the first token’s value is a tuple or list.
format(tokens)[source]¶
Returns a string representation of ARFF output for the given data.
static from_train(tokens)[source]¶
Constructs an ARFF_Formatter instance with class labels and feature types determined from the given data. Handles boolean, numeric and string (note: not nominal) types.
header_section()[source]¶
Returns an ARFF header as a string.
labels()[source]¶
Returns the list of classes.
write(outfile, tokens)[source]¶
Writes ARFF data to a file for the given data.
class nltk.classify.weka.WekaClassifier(formatter, model_filename)[source]¶
Bases: nltk.classify.api.ClassifierI

batch_classify(featuresets)[source]¶
batch_prob_classify(featuresets)[source]¶
parse_weka_distribution(s)[source]¶
parse_weka_output(lines)[source]¶
classmethod train(model_filename, featuresets, classifier='naivebayes', options=[], quiet=True)[source]¶
nltk.classify.weka.config_weka(classpath=None)[source]¶
Table Of Contents

Search

Enter search terms or a module, class or function name.

page 4
cluster Package ¶

This module contains a number of basic clustering algorithms. Clustering describes the task of discovering groups of similar items with a large collection. It is also describe as unsupervised machine learning, as the data from which it learns is unannotated with class information, as is the case for supervised learning. Annotated data is difficult and expensive to obtain in the quantities required for the majority of supervised learning algorithms. This problem, the knowledge acquisition bottleneck, is common to most natural language processing tasks, thus fueling the need for quality unsupervised approaches.

This module contains a k-means clusterer, E-M clusterer and a group average agglomerative clusterer (GAAC). All these clusterers involve finding good cluster groupings for a set of vectors in multi-dimensional space.

The K-means clusterer starts with k arbitrary chosen means then allocates each vector to the cluster with the closest mean. It then recalculates the means of each cluster as the centroid of the vectors in the cluster. This process repeats until the cluster memberships stabilise. This is a hill-climbing algorithm which may converge to a local maximum. Hence the clustering is often repeated with random initial means and the most commonly occurring output means are chosen.

The GAAC clusterer starts with each of the N vectors as singleton clusters. It then iteratively merges pairs of clusters which have the closest centroids. This continues until there is only one cluster. The order of merges gives rise to a dendrogram - a tree with the earlier merges lower than later merges. The membership of a given number of clusters c, 1 <= c <= N, can be found by cutting the dendrogram at depth c.

The Gaussian EM clusterer models the vectors as being produced by a mixture of k Gaussian sources. The parameters of these sources (prior probability, mean and covariance matrix) are then found to maximise the likelihood of the given data. This is done with the expectation maximisation algorithm. It starts with k arbitrarily chosen means, priors and covariance matrices. It then calculates the membership probabilities for each vector in each of the clusters - this is the ‘E’ step. The cluster parameters are then updated in the ‘M’ step using the maximum likelihood estimate from the cluster membership probabilities. This process continues until the likelihood of the data does not significantly increase.

They all extend the ClusterI interface which defines common operations available with each clusterer. These operations include.

cluster: clusters a sequence of vectors
classify: assign a vector to a cluster
classification_probdist: give the probability distribution over cluster memberships
The current existing classifiers also extend cluster.VectorSpace, an abstract class which allows for singular value decomposition (SVD) and vector normalisation. SVD is used to reduce the dimensionality of the vector space in such a manner as to preserve as much of the variation as possible, by reparameterising the axes in order of variability and discarding all bar the first d dimensions. Normalisation ensures that vectors fall in the unit hypersphere.

Usage example (see also demo())::
from nltk import cluster from nltk.cluster import euclidean_distance from numpy import array

vectors = [array(f) for f in [[3, 3], [1, 2], [4, 2], [4, 0]]]

# initialise the clusterer (will also assign the vectors to clusters) clusterer = cluster.KMeansClusterer(2, euclidean_distance) clusterer.cluster(vectors, True)

# classify a new vector print(clusterer.classify(array([3, 3])))
Note that the vectors must use numpy array-like objects. nltk_contrib.unimelb.tacohn.SparseArrays may be used for efficiency when required.


page 5
NLTK corpus readers. The modules in this package provide functions that can be used to read corpus files in a variety of formats. These functions can be used to read both the corpus files that are distributed in the NLTK corpus package, and corpus files that are part of external corpora.

Available Corpora ¶

Please see http://nltk.googlecode.com/svn/trunk/nltk_data/index.xml for a complete list. Install corpora using nltk.download().
Corpus Reader Functions ¶

Each corpus module defines one or more “corpus reader functions”, which can be used to read documents from that corpus. These functions take an argument, item, which is used to indicate which document should be read from the corpus:

If item is one of the unique identifiers listed in the corpus module’s items variable, then the corresponding document will be loaded from the NLTK corpus package.
If item is a filename, then that file will be read.
Additionally, corpus reader functions can be given lists of item names; in which case, they will return a concatenation of the corresponding documents.

Corpus reader functions are named based on the type of information they return. Some common examples, and their return types, are:

words(): list of str
sents(): list of (list of str)
paras(): list of (list of (list of str))
tagged_words(): list of (str,str) tuple
tagged_sents(): list of (list of (str,str))
tagged_paras(): list of (list of (list of (str,str)))
chunked_sents(): list of (Tree w/ (str,str) leaves)
parsed_sents(): list of (Tree with str leaves)
parsed_paras(): list of (list of (Tree with str leaves))
xml(): A single xml ElementTree
raw(): unprocessed corpus contents
For example, to read a list of the words in the Brown Corpus, use nltk.corpus.brown.words():

>>> from nltk.corpus import brown
>>> print(", ".join(brown.words()))
The, Fulton, County, Grand, Jury, said, ...
nltk.corpus.demo()[source]¶
nltk.corpus.teardown_module(module)[source]¶

page 6
NLTK corpus readers. The modules in this package provide functions that can be used to read corpus fileids in a variety of formats. These functions can be used to read both the corpus fileids that are distributed in the NLTK corpus package, and corpus fileids that are part of external corpora.

Corpus Reader Functions ¶

Each corpus module defines one or more “corpus reader functions”, which can be used to read documents from that corpus. These functions take an argument, item, which is used to indicate which document should be read from the corpus:

If item is one of the unique identifiers listed in the corpus module’s items variable, then the corresponding document will be loaded from the NLTK corpus package.
If item is a fileid, then that file will be read.
Additionally, corpus reader functions can be given lists of item names; in which case, they will return a concatenation of the corresponding documents.

Corpus reader functions are named based on the type of information they return. Some common examples, and their return types, are:

words(): list of str
sents(): list of (list of str)
paras(): list of (list of (list of str))
tagged_words(): list of (str,str) tuple
tagged_sents(): list of (list of (str,str))
tagged_paras(): list of (list of (list of (str,str)))
chunked_sents(): list of (Tree w/ (str,str) leaves)
parsed_sents(): list of (Tree with str leaves)
parsed_paras(): list of (list of (Tree with str leaves))
xml(): A single xml ElementTree
raw(): unprocessed corpus contents
For example, to read a list of the words in the Brown Corpus, use nltk.corpus.brown.words():

>>> from nltk.corpus import brown
>>> print(", ".join(brown.words()))
The, Fulton, County, Grand, Jury, said, ...
class nltk.corpus.reader.CorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: object

A base class for “corpus reader” classes, each of which can be used to read a specific corpus format. Each individual corpus reader instance is used to read a specific corpus, consisting of one or more files under a common root directory. Each file is identified by its file identifier, which is the relative path to the file from the root directory.

A separate subclass is be defined for each corpus format. These subclasses define one or more methods that provide ‘views’ on the corpus contents, such as words() (for a list of words) and parsed_sents() (for a list of parsed sentences). Called with no arguments, these methods will return the contents of the entire corpus. For most corpora, these methods define one or more selection arguments, such as fileids or categories, which can be used to select which portion of the corpus should be returned.

abspath(fileid)¶
Return the absolute path for the given file.

Parameters:	file (str) – The file identifier for the file whose path should be returned.
Return type:	PathPointer
abspaths(fileids=None, include_encoding=False, include_fileid=False)¶
Return a list of the absolute paths for all fileids in this corpus; or for the given list of fileids, if specified.

Parameters:
fileids (None or str or list) – Specifies the set of fileids for which paths should be returned. Can be None, for all fileids; a list of file identifiers, for a specified set of fileids; or a single file identifier, for a single file. Note that the return value is always a list of paths, even if fileids is a single file identifier.
include_encoding – If true, then return a list of (path_pointer, encoding) tuples.
Return type:
list(PathPointer)
encoding(file)¶
Return the unicode encoding for the given corpus file, if known. If the encoding is unknown, or if the given file should be processed using byte strings (str), then return None.
fileids()¶
Return a list of file identifiers for the fileids that make up this corpus.
open(file)¶
Return an open stream that can be used to read the given file. If the file’s encoding is not None, then the stream will automatically decode the file’s contents into unicode.

Parameters:	file – The file identifier of the file to read.
readme()¶
Return the contents of the corpus README file, if it exists.
root¶
The directory where this corpus is stored.

Type :	PathPointer
unicode_repr()¶
class nltk.corpus.reader.CategorizedCorpusReader(kwargs)¶
Bases: object

A mixin class used to aid in the implementation of corpus readers for categorized corpora. This class defines the method categories(), which returns a list of the categories for the corpus or for a specified set of fileids; and overrides fileids() to take a categories argument, restricting the set of fileids to be returned.

Subclasses are expected to:

Call __init__() to set up the mapping.
Override all view methods to accept a categories parameter, which can be used instead of the fileids parameter, to select which fileids should be included in the returned view.
categories(fileids=None)¶
Return a list of the categories that are defined for this corpus, or for the file(s) if it is given.
fileids(categories=None)¶
Return a list of file identifiers for the files that make up this corpus, or that make up the given category(s) if specified.
class nltk.corpus.reader.PlaintextCorpusReader¶
Bases: nltk.corpus.reader.api.CorpusReader

Reader for corpora that consist of plaintext documents. Paragraphs are assumed to be split using blank lines. Sentences and words can be tokenized using the default tokenizers, or by custom tokenizers specificed as parameters to the constructor.

This corpus reader can be customized (e.g., to skip preface sections of specific document formats) by creating a subclass and overriding the CorpusView class variable.

CorpusView¶
alias of StreamBackedCorpusView
paras(fileids=None)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as lists of word strings.
Return type:	list(list(list(str)))
raw(fileids=None)¶
Returns:	the given file(s) as a single string.
Return type:	str
sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:	list(list(str))
words(fileids=None)¶
Returns:	the given file(s) as a list of words and punctuation symbols.
Return type:	list(str)
nltk.corpus.reader.find_corpus_fileids(root, regexp)¶
class nltk.corpus.reader.TaggedCorpusReader(root, fileids, sep='/', word_tokenizer=WhitespaceTokenizer(pattern=u'\s+', gaps=True, discard_empty=True, flags=56), sent_tokenizer=RegexpTokenizer(pattern='n', gaps=True, discard_empty=True, flags=56), para_block_reader=<function read_blankline_block at 0x1104f45f0>, encoding='utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

Reader for simple part-of-speech tagged corpora. Paragraphs are assumed to be split using blank lines. Sentences and words can be tokenized using the default tokenizers, or by custom tokenizers specified as parameters to the constructor. Words are parsed using nltk.tag.str2tuple. By default, '/' is used as the separator. I.e., words should have the form:

word1/tag1 word2/tag2 word3/tag3 ...
But custom separators may be specified as parameters to the constructor. Part of speech tags are case-normalized to upper case.

paras(fileids=None)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as lists of word strings.
Return type:	list(list(list(str)))
raw(fileids=None)¶
Returns:	the given file(s) as a single string.
Return type:	str
sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:	list(list(str))
tagged_paras(fileids=None, simplify_tags=False)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as lists of (word,tag) tuples.
Return type:	list(list(list(tuple(str,str))))
tagged_sents(fileids=None, simplify_tags=False)¶
Returns:	the given file(s) as a list of sentences, each encoded as a list of (word,tag) tuples.
Return type:	list(list(tuple(str,str)))
tagged_words(fileids=None, simplify_tags=False)¶
Returns:	the given file(s) as a list of tagged words and punctuation symbols, encoded as tuples (word,tag).
Return type:	list(tuple(str,str))
words(fileids=None)¶
Returns:	the given file(s) as a list of words and punctuation symbols.
Return type:	list(str)
class nltk.corpus.reader.CMUDictCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

dict()¶
Returns:	the cmudict lexicon as a dictionary, whose keys are
lowercase words and whose values are lists of pronunciations.
entries()¶
Returns:	the cmudict lexicon as a list of entries
containing (word, transcriptions) tuples.
raw()¶
Returns:	the cmudict lexicon as a raw string.
words()¶
Returns:	a list of all words defined in the cmudict lexicon.
class nltk.corpus.reader.ConllChunkCorpusReader(root, fileids, chunk_types, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.conll.ConllCorpusReader

A ConllCorpusReader whose data file contains three columns: words, pos, and chunk.
class nltk.corpus.reader.WordListCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

List of words, one per line. Blank lines are ignored.

raw(fileids=None)¶
words(fileids=None)¶
class nltk.corpus.reader.PPAttachmentCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

sentence_id verb noun1 preposition noun2 attachment

attachments(fileids)¶
raw(fileids=None)¶
tuples(fileids)¶
class nltk.corpus.reader.SensevalCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

instances(fileids=None)¶
raw(fileids=None)¶
Returns:	the text contents of the given fileids, as a single string.
class nltk.corpus.reader.IEERCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

docs(fileids=None)¶
parsed_docs(fileids=None)¶
raw(fileids=None)¶
class nltk.corpus.reader.ChunkedCorpusReader(root, fileids, extension='', str2chunktree=<function tagstr2tree at 0x1104867d0>, sent_tokenizer=RegexpTokenizer(pattern='n', gaps=True, discard_empty=True, flags=56), para_block_reader=<function read_blankline_block at 0x1104f45f0>, encoding='utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

Reader for chunked (and optionally tagged) corpora. Paragraphs are split using a block reader. They are then tokenized into sentences using a sentence tokenizer. Finally, these sentences are parsed into chunk trees using a string-to-chunktree conversion function. Each of these steps can be performed using a default function or a custom function. By default, paragraphs are split on blank lines; sentences are listed one per line; and sentences are parsed into chunk trees using nltk.chunk.tagstr2tree.

chunked_paras(fileids=None)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as a shallow Tree. The leaves of these trees are encoded as (word, tag) tuples (if the corpus has tags) or word strings (if the corpus has no tags).
Return type:	list(list(Tree))
chunked_sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences, each encoded as a shallow Tree. The leaves of these trees are encoded as (word, tag) tuples (if the corpus has tags) or word strings (if the corpus has no tags).
Return type:	list(Tree)
chunked_words(fileids=None)¶
Returns:	the given file(s) as a list of tagged words and chunks. Words are encoded as (word, tag) tuples (if the corpus has tags) or word strings (if the corpus has no tags). Chunks are encoded as depth-one trees over (word,tag) tuples or word strings.
Return type:	list(tuple(str,str) and Tree)
paras(fileids=None)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as lists of word strings.
Return type:	list(list(list(str)))
raw(fileids=None)¶
Returns:	the given file(s) as a single string.
Return type:	str
sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:	list(list(str))
tagged_paras(fileids=None)¶
Returns:	the given file(s) as a list of paragraphs, each encoded as a list of sentences, which are in turn encoded as lists of (word,tag) tuples.
Return type:	list(list(list(tuple(str,str))))
tagged_sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences, each encoded as a list of (word,tag) tuples.
Return type:	list(list(tuple(str,str)))
tagged_words(fileids=None)¶
Returns:	the given file(s) as a list of tagged words and punctuation symbols, encoded as tuples (word,tag).
Return type:	list(tuple(str,str))
words(fileids=None)¶
Returns:	the given file(s) as a list of words and punctuation symbols.
Return type:	list(str)
class nltk.corpus.reader.SinicaTreebankCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.SyntaxCorpusReader

Reader for the sinica treebank.
class nltk.corpus.reader.BracketParseCorpusReader(root, fileids, comment_char=None, detect_blocks='unindented_paren', encoding='utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.SyntaxCorpusReader

Reader for corpora that consist of parenthesis-delineated parse trees.
class nltk.corpus.reader.IndianCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

List of words, one per line. Blank lines are ignored.

raw(fileids=None)¶
sents(fileids=None)¶
tagged_sents(fileids=None, simplify_tags=False)¶
tagged_words(fileids=None, simplify_tags=False)¶
words(fileids=None)¶
class nltk.corpus.reader.ToolboxCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

entries(fileids, **kwargs)¶
fields(fileids, strip=True, unwrap=True, encoding='utf8', errors='strict', unicode_fields=None)¶
raw(fileids)¶
words(fileids, key='lx')¶
xml(fileids, key=None)¶
class nltk.corpus.reader.TimitCorpusReader(root, encoding=u'utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

Reader for the TIMIT corpus (or any other corpus with the same file layout and use of file formats). The corpus root directory should contain the following files:

timitdic.txt: dictionary of standard transcriptions
spkrinfo.txt: table of speaker information
In addition, the root directory should contain one subdirectory for each speaker, containing three files for each utterance:

<utterance-id>.txt: text content of utterances
<utterance-id>.wrd: tokenized text content of utterances
<utterance-id>.phn: phonetic transcription of utterances
<utterance-id>.wav: utterance sound file
audiodata(utterance, start=0, end=None)¶
fileids(filetype=None)¶
Return a list of file identifiers for the files that make up this corpus.

Parameters:	filetype – If specified, then filetype indicates that only the files that have the given type should be returned. Accepted values are: txt, wrd, phn, wav, or metadata,
phone_times(utterances=None)¶
offset is represented as a number of 16kHz samples!
phone_trees(utterances=None)¶
phones(utterances=None)¶
play(utterance, start=0, end=None)¶
Play the given audio sample.

Parameters:	utterance – The utterance id of the sample to play
sent_times(utterances=None)¶
sentid(utterance)¶
sents(utterances=None)¶
spkrid(utterance)¶
spkrinfo(speaker)¶
Returns:	A dictionary mapping .. something.
spkrutteranceids(speaker)¶
Returns:	A list of all utterances associated with a given
speaker.
transcription_dict()¶
Returns:	A dictionary giving the ‘standard’ transcription for
each word.
utterance(spkrid, sentid)¶
utteranceids(dialect=None, sex=None, spkrid=None, sent_type=None, sentid=None)¶
Returns:	A list of the utterance identifiers for all
utterances in this corpus, or for the given speaker, dialect region, gender, sentence type, or sentence number, if specified.
wav(utterance, start=0, end=None)¶
word_times(utterances=None)¶
words(utterances=None)¶
class nltk.corpus.reader.YCOECorpusReader(root, encoding='utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

Corpus reader for the York-Toronto-Helsinki Parsed Corpus of Old English Prose (YCOE), a 1.5 million word syntactically-annotated corpus of Old English prose texts.

documents(fileids=None)¶
Return a list of document identifiers for all documents in this corpus, or for the documents with the given file(s) if specified.
fileids(documents=None)¶
Return a list of file identifiers for the files that make up this corpus, or that store the given document(s) if specified.
paras(documents=None)¶
parsed_sents(documents=None)¶
sents(documents=None)¶
tagged_paras(documents=None)¶
tagged_sents(documents=None)¶
tagged_words(documents=None)¶
words(documents=None)¶
class nltk.corpus.reader.MacMorphoCorpusReader(root, fileids, encoding='utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.tagged.TaggedCorpusReader

A corpus reader for the MAC_MORPHO corpus. Each line contains a single tagged word, using ‘_’ as a separator. Sentence boundaries are based on the end-sentence tag (‘_.’). Paragraph information is not included in the corpus, so each paragraph returned by self.paras() and self.tagged_paras() contains a single sentence.
class nltk.corpus.reader.SyntaxCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

An abstract base class for reading corpora consisting of syntactically parsed text. Subclasses should define:

__init__, which specifies the location of the corpus and a method for detecting the sentence blocks in corpus files.
_read_block, which reads a block from the input stream.
_word, which takes a block and returns a list of list of words.
_tag, which takes a block and returns a list of list of tagged words.
_parse, which takes a block and returns a list of parsed sentences.
parsed_sents(fileids=None)¶
raw(fileids=None)¶
sents(fileids=None)¶
tagged_sents(fileids=None, simplify_tags=False)¶
tagged_words(fileids=None, simplify_tags=False)¶
words(fileids=None)¶
class nltk.corpus.reader.AlpinoCorpusReader(root, encoding='ISO-8859-1', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.bracket_parse.BracketParseCorpusReader

Reader for the Alpino Dutch Treebank.
class nltk.corpus.reader.RTECorpusReader(root, fileids, wrap_etree=False)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

Corpus reader for corpora in RTE challenges.

This is just a wrapper around the XMLCorpusReader. See module docstring above for the expected structure of input documents.

pairs(fileids)¶
Build a list of RTEPairs from a RTE corpus.

Parameters:	fileids – a list of RTE corpus fileids
Type :	list
Return type:	list(RTEPair)
class nltk.corpus.reader.StringCategoryCorpusReader(root, fileids, delimiter=' ', encoding='utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

raw(fileids=None)¶
Returns:	the text contents of the given fileids, as a single string.
tuples(fileids=None)¶
class nltk.corpus.reader.EuroparlCorpusReader¶
Bases: nltk.corpus.reader.plaintext.PlaintextCorpusReader

Reader for Europarl corpora that consist of plaintext documents. Documents are divided into chapters instead of paragraphs as for regular plaintext documents. Chapters are separated using blank lines. Everything is inherited from PlaintextCorpusReader except that:

Since the corpus is pre-processed and pre-tokenized, the word tokenizer should just split the line at whitespaces.
For the same reason, the sentence tokenizer should just split the paragraph at line breaks.
There is a new ‘chapters()’ method that returns chapters instead instead of paragraphs.
The ‘paras()’ method inherited from PlaintextCorpusReader is made non-functional to remove any confusion between chapters and paragraphs for Europarl.
chapters(fileids=None)¶
Returns:	the given file(s) as a list of chapters, each encoded as a list of sentences, which are in turn encoded as lists of word strings.
Return type:	list(list(list(str)))
paras(fileids=None)¶
class nltk.corpus.reader.CategorizedBracketParseCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.api.CategorizedCorpusReader, nltk.corpus.reader.bracket_parse.BracketParseCorpusReader

A reader for parsed corpora whose documents are divided into categories based on their file identifiers. @author: Nathan Schneider <nschneid@cs.cmu.edu >

paras(fileids=None, categories=None)¶
parsed_paras(fileids=None, categories=None)¶
parsed_sents(fileids=None, categories=None)¶
parsed_words(fileids=None, categories=None)¶
raw(fileids=None, categories=None)¶
sents(fileids=None, categories=None)¶
tagged_paras(fileids=None, categories=None, simplify_tags=False)¶
tagged_sents(fileids=None, categories=None, simplify_tags=False)¶
tagged_words(fileids=None, categories=None, simplify_tags=False)¶
words(fileids=None, categories=None)¶
class nltk.corpus.reader.CategorizedTaggedCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.api.CategorizedCorpusReader, nltk.corpus.reader.tagged.TaggedCorpusReader

A reader for part-of-speech tagged corpora whose documents are divided into categories based on their file identifiers.

paras(fileids=None, categories=None)¶
raw(fileids=None, categories=None)¶
sents(fileids=None, categories=None)¶
tagged_paras(fileids=None, categories=None, simplify_tags=False)¶
tagged_sents(fileids=None, categories=None, simplify_tags=False)¶
tagged_words(fileids=None, categories=None, simplify_tags=False)¶
words(fileids=None, categories=None)¶
class nltk.corpus.reader.CategorizedPlaintextCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.api.CategorizedCorpusReader, nltk.corpus.reader.plaintext.PlaintextCorpusReader

A reader for plaintext corpora whose documents are divided into categories based on their file identifiers.

paras(fileids=None, categories=None)¶
raw(fileids=None, categories=None)¶
sents(fileids=None, categories=None)¶
words(fileids=None, categories=None)¶
class nltk.corpus.reader.PortugueseCategorizedPlaintextCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.plaintext.CategorizedPlaintextCorpusReader
nltk.corpus.reader.tagged_treebank_para_block_reader(stream)¶
class nltk.corpus.reader.PropbankCorpusReader(root, propfile, framefiles=u'', verbsfile=None, parse_fileid_xform=None, parse_corpus=None, encoding=u'utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

Corpus reader for the propbank corpus, which augments the Penn Treebank with information about the predicate argument structure of every verb instance. The corpus consists of two parts: the predicate-argument annotations themselves, and a set of “frameset files” which define the argument labels used by the annotations, on a per-verb basis. Each “frameset file” contains one or more predicates, such as 'turn' or 'turn_on', each of which is divided into coarse-grained word senses called “rolesets”. For each “roleset”, the frameset file provides descriptions of the argument roles, along with examples.

instances(baseform=None)¶
Returns:	a corpus view that acts as a list of
PropBankInstance objects, one for each noun in the corpus.
lines()¶
Returns:	a corpus view that acts as a list of strings, one for
each line in the predicate-argument annotation file.
raw(fileids=None)¶
Returns:	the text contents of the given fileids, as a single string.
roleset(roleset_id)¶
Returns:	the xml description for the given roleset.
rolesets(baseform=None)¶
Returns:	list of xml descriptions for rolesets.
verbs()¶
Returns:	a corpus view that acts as a list of all verb lemmas
in this corpus (from the verbs.txt file).
class nltk.corpus.reader.VerbnetCorpusReader(root, fileids, wrap_etree=False)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

classids(lemma=None, wordnetid=None, fileid=None, classid=None)¶
Return a list of the verbnet class identifiers. If a file identifier is specified, then return only the verbnet class identifiers for classes (and subclasses) defined by that file. If a lemma is specified, then return only verbnet class identifiers for classes that contain that lemma as a member. If a wordnetid is specified, then return only identifiers for classes that contain that wordnetid as a member. If a classid is specified, then return only identifiers for subclasses of the specified verbnet class.
fileids(vnclass_ids=None)¶
Return a list of fileids that make up this corpus. If vnclass_ids is specified, then return the fileids that make up the specified verbnet class(es).
lemmas(classid=None)¶
Return a list of all verb lemmas that appear in any class, or in the classid if specified.
longid(shortid)¶
Given a short verbnet class identifier (eg ‘37.10’), map it to a long id (eg ‘confess-37.10’). If shortid is already a long id, then return it as-is
pprint(vnclass)¶
Return a string containing a pretty-printed representation of the given verbnet class.

Parameters:	vnclass – A verbnet class identifier; or an ElementTree
containing the xml contents of a verbnet class.
pprint_description(vnframe, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet frame description.

Parameters:	vnframe – An ElementTree containing the xml contents of a verbnet frame.
pprint_frame(vnframe, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet frame.

Parameters:	vnframe – An ElementTree containing the xml contents of a verbnet frame.
pprint_members(vnclass, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet class’s member verbs.

Parameters:	vnclass – A verbnet class identifier; or an ElementTree containing the xml contents of a verbnet class.
pprint_semantics(vnframe, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet frame semantics.

Parameters:	vnframe – An ElementTree containing the xml contents of a verbnet frame.
pprint_subclasses(vnclass, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet class’s subclasses.

Parameters:	vnclass – A verbnet class identifier; or an ElementTree containing the xml contents of a verbnet class.
pprint_syntax(vnframe, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet frame syntax.

Parameters:	vnframe – An ElementTree containing the xml contents of a verbnet frame.
pprint_themroles(vnclass, indent=u'')¶
Return a string containing a pretty-printed representation of the given verbnet class’s thematic roles.

Parameters:	vnclass – A verbnet class identifier; or an ElementTree containing the xml contents of a verbnet class.
shortid(longid)¶
Given a long verbnet class identifier (eg ‘confess-37.10’), map it to a short id (eg ‘37.10’). If longid is already a short id, then return it as-is.
vnclass(fileid_or_classid)¶
Return an ElementTree containing the xml for the specified verbnet class.

Parameters:	fileid_or_classid – An identifier specifying which class should be returned. Can be a file identifier (such as 'put-9.1.xml'), or a verbnet class identifier (such as 'put-9.1') or a short verbnet class identifier (such as '9.1').
wordnetids(classid=None)¶
Return a list of all wordnet identifiers that appear in any class, or in classid if specified.
class nltk.corpus.reader.BNCCorpusReader(root, fileids, lazy=True)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

Corpus reader for the XML version of the British National Corpus. For access to the complete XML data structure, use the xml() method. For access to simple word lists and tagged word lists, use words(), sents(), tagged_words(), and tagged_sents().

sents(fileids=None, strip_space=True, stem=False)¶
Returns:
the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:
list(list(str))
Parameters:
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
stem – If true, then use word stems instead of word strings.
tagged_sents(fileids=None, c5=False, strip_space=True, stem=False)¶
Returns:
the given file(s) as a list of sentences, each encoded as a list of (word,tag) tuples.
Return type:
list(list(tuple(str,str)))
Parameters:
c5 – If true, then the tags used will be the more detailed c5 tags. Otherwise, the simplified tags will be used.
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
stem – If true, then use word stems instead of word strings.
tagged_words(fileids=None, c5=False, strip_space=True, stem=False)¶
Returns:
the given file(s) as a list of tagged words and punctuation symbols, encoded as tuples (word,tag).
Return type:
list(tuple(str,str))
Parameters:
c5 – If true, then the tags used will be the more detailed c5 tags. Otherwise, the simplified tags will be used.
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
stem – If true, then use word stems instead of word strings.
words(fileids=None, strip_space=True, stem=False)¶
Returns:
the given file(s) as a list of words and punctuation symbols.
Return type:
list(str)
Parameters:
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
stem – If true, then use word stems instead of word strings.
class nltk.corpus.reader.ConllCorpusReader(root, fileids, columntypes, chunk_types=None, top_node=u'S', pos_in_tree=False, srl_includes_roleset=True, encoding=u'utf8', tree_class=<class 'nltk.tree.Tree'>, tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

A corpus reader for CoNLL-style files. These files consist of a series of sentences, separated by blank lines. Each sentence is encoded using a table (or “grid”) of values, where each line corresponds to a single word, and each column corresponds to an annotation type. The set of columns used by CoNLL-style files can vary from corpus to corpus; the ConllCorpusReader constructor therefore takes an argument, columntypes, which is used to specify the columns that are used by a given corpus.

@todo: Add support for reading from corpora where different
parallel files contain different columns.
@todo: Possibly add caching of the grid corpus view? This would
allow the same grid view to be used by different data access methods (eg words() and parsed_sents() could both share the same grid corpus view object).
@todo: Better support for -DOCSTART-. Currently, we just ignore
it, but it could be used to define methods that retrieve a document at a time (eg parsed_documents()).
CHUNK = u'chunk'¶
COLUMN_TYPES = (u'words', u'pos', u'tree', u'chunk', u'ne', u'srl', u'ignore')¶
IGNORE = u'ignore'¶
NE = u'ne'¶
POS = u'pos'¶
SRL = u'srl'¶
TREE = u'tree'¶
WORDS = u'words'¶
chunked_sents(fileids=None, chunk_types=None, simplify_tags=False)¶
chunked_words(fileids=None, chunk_types=None, simplify_tags=False)¶
iob_sents(fileids=None, simplify_tags=False)¶
Returns:	a list of lists of word/tag/IOB tuples
Return type:	list(list)
Parameters:	fileids (None or str or list) – the list of fileids that make up this corpus
iob_words(fileids=None, simplify_tags=False)¶
Returns:	a list of word/tag/IOB tuples
Return type:	list(tuple)
Parameters:	fileids (None or str or list) – the list of fileids that make up this corpus
parsed_sents(fileids=None, pos_in_tree=None, simplify_tags=False)¶
raw(fileids=None)¶
sents(fileids=None)¶
srl_instances(fileids=None, pos_in_tree=None, flatten=True)¶
srl_spans(fileids=None)¶
tagged_sents(fileids=None, simplify_tags=False)¶
tagged_words(fileids=None, simplify_tags=False)¶
words(fileids=None)¶
class nltk.corpus.reader.XMLCorpusReader(root, fileids, wrap_etree=False)¶
Bases: nltk.corpus.reader.api.CorpusReader

Corpus reader for corpora whose documents are xml files.

Note that the XMLCorpusReader constructor does not take an encoding argument, because the unicode encoding is specified by the XML files themselves. See the XML specs for more info.

raw(fileids=None)¶
words(fileid=None)¶
Returns all of the words and punctuation symbols in the specified file that were in text nodes – ie, tags are ignored. Like the xml() method, fileid can only specify one file.

Returns:	the given file’s text nodes as a list of words and punctuation symbols
Return type:	list(str)
xml(fileid=None)¶
class nltk.corpus.reader.NPSChatCorpusReader(root, fileids, wrap_etree=False, tag_mapping_function=None)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

posts(fileids=None)¶
tagged_posts(fileids=None, simplify_tags=False)¶
tagged_words(fileids=None, simplify_tags=False)¶
words(fileids=None)¶
xml_posts(fileids=None)¶
class nltk.corpus.reader.SwadeshCorpusReader(root, fileids, encoding=u'utf8', tag_mapping_function=None)¶
Bases: nltk.corpus.reader.wordlist.WordListCorpusReader

entries(fileids=None)¶
Returns:	a tuple of words for the specified fileids.
class nltk.corpus.reader.WordNetCorpusReader(root)¶
Bases: nltk.corpus.reader.api.CorpusReader

A corpus reader used to access wordnet or its variants.

ADJ = u'a'¶
ADJ_SAT = u's'¶
ADV = u'r'¶
MORPHOLOGICAL_SUBSTITUTIONS = {u'a': [(u'er', u''), (u'est', u''), (u'er', u'e'), (u'est', u'e')], u'r': [], u'v': [(u's', u''), (u'ies', u'y'), (u'es', u'e'), (u'es', u''), (u'ed', u'e'), (u'ed', u''), (u'ing', u'e'), (u'ing', u'')], u'n': [(u's', u''), (u'ses', u's'), (u'ves', u'f'), (u'xes', u'x'), (u'zes', u'z'), (u'ches', u'ch'), (u'shes', u'sh'), (u'men', u'man'), (u'ies', u'y')]}¶
NOUN = u'n'¶
VERB = u'v'¶
all_lemma_names(pos=None)¶
Return all lemma names for all synsets for the given part of speech tag. If pos is not specified, all synsets for all parts of speech will be used.
all_synsets(pos=None)¶
Iterate over all synsets with a given part of speech tag. If no pos is specified, all synsets for all parts of speech will be loaded.
get_version()¶
ic(corpus, weight_senses_equally=False, smoothing=1.0)¶
Creates an information content lookup dictionary from a corpus.

Parameters:	corpus (CorpusReader) – The corpus from which we create an information
content dictionary. :type weight_senses_equally: bool :param weight_senses_equally: If this is True, gives all possible senses equal weight rather than dividing by the number of possible senses. (If a word has 3 synses, each sense gets 0.3333 per appearance when this is False, 1.0 when it is true.) :param smoothing: How much do we smooth synset counts (default is 1.0) :type smoothing: float :return: An information content dictionary
jcn_similarity(synset1, synset2, ic, verbose=False)¶
Jiang-Conrath Similarity: Return a score denoting how similar two word senses are, based on the Information Content (IC) of the Least Common Subsumer (most specific ancestor node) and that of the two input Synsets. The relationship is given by the equation 1 / (IC(s1) + IC(s2) - 2 * IC(lcs)).

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
ic (dict) – an information content object (as returned by nltk.corpus.wordnet_ic.ic()).
Returns:
A float score denoting the similarity of the two Synset objects.
lch_similarity(synset1, synset2, verbose=False, simulate_root=True)¶
Leacock Chodorow Similarity: Return a score denoting how similar two word senses are, based on the shortest path that connects the senses (as above) and the maximum depth of the taxonomy in which the senses occur. The relationship is given as -log(p/2d) where p is the shortest path length and d is the taxonomy depth.

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
simulate_root (bool) – The various verb taxonomies do not share a single root which disallows this metric from working for synsets that are not connected. This flag (True by default) creates a fake root that connects all the taxonomies. Set it to false to disable this behavior. For the noun taxonomy, there is usually a default root except for WordNet version 1.6. If you are using wordnet 1.6, a fake root will be added for nouns as well.
Returns:
A score denoting the similarity of the two Synset objects, normally greater than 0. None is returned if no connecting path could be found. If a Synset is compared with itself, the maximum score is returned, which varies depending on the taxonomy depth.
lemma(name)¶
lemma_count(lemma)¶
Return the frequency count for this Lemma
lemma_from_key(key)¶
lemmas(lemma, pos=None)¶
Return all Lemma objects with a name matching the specified lemma name and part of speech tag. Matches any part of speech tag if none is specified.
lin_similarity(synset1, synset2, ic, verbose=False)¶
Lin Similarity: Return a score denoting how similar two word senses are, based on the Information Content (IC) of the Least Common Subsumer (most specific ancestor node) and that of the two input Synsets. The relationship is given by the equation 2 * IC(lcs) / (IC(s1) + IC(s2)).

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
ic (dict) – an information content object (as returned by nltk.corpus.wordnet_ic.ic()).
Returns:
A float score denoting the similarity of the two Synset objects, in the range 0 to 1.
morphy(form, pos=None)¶
Find a possible base form for the given form, with the given part of speech, by checking WordNet’s list of exceptional forms, and by recursively stripping affixes for this part of speech until a form in WordNet is found.

>>> from nltk.corpus import wordnet as wn
>>> print(wn.morphy('dogs'))
dog
>>> print(wn.morphy('churches'))
church
>>> print(wn.morphy('aardwolves'))
aardwolf
>>> print(wn.morphy('abaci'))
abacus
>>> wn.morphy('hardrock', wn.ADV)
>>> print(wn.morphy('book', wn.NOUN))
book
>>> wn.morphy('book', wn.ADJ)
path_similarity(synset1, synset2, verbose=False, simulate_root=True)¶
Path Distance Similarity: Return a score denoting how similar two word senses are, based on the shortest path that connects the senses in the is-a (hypernym/hypnoym) taxonomy. The score is in the range 0 to 1, except in those cases where a path cannot be found (will only be true for verbs as there are many distinct verb taxonomies), in which case None is returned. A score of 1 represents identity i.e. comparing a sense with itself will return 1.

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
simulate_root (bool) – The various verb taxonomies do not share a single root which disallows this metric from working for synsets that are not connected. This flag (True by default) creates a fake root that connects all the taxonomies. Set it to false to disable this behavior. For the noun taxonomy, there is usually a default root except for WordNet version 1.6. If you are using wordnet 1.6, a fake root will be added for nouns as well.
Returns:
A score denoting the similarity of the two Synset objects, normally between 0 and 1. None is returned if no connecting path could be found. 1 is returned if a Synset is compared with itself.
res_similarity(synset1, synset2, ic, verbose=False)¶
Resnik Similarity: Return a score denoting how similar two word senses are, based on the Information Content (IC) of the Least Common Subsumer (most specific ancestor node).

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
ic (dict) – an information content object (as returned by nltk.corpus.wordnet_ic.ic()).
Returns:
A float score denoting the similarity of the two Synset objects. Synsets whose LCS is the root node of the taxonomy will have a score of 0 (e.g. N[‘dog’][0] and N[‘table’][0]).
synset(name)¶
synsets(lemma, pos=None)¶
Load all synsets with a given lemma and part of speech tag. If no pos is specified, all synsets for all parts of speech will be loaded.
wup_similarity(synset1, synset2, verbose=False, simulate_root=True)¶
Wu-Palmer Similarity: Return a score denoting how similar two word senses are, based on the depth of the two senses in the taxonomy and that of their Least Common Subsumer (most specific ancestor node). Previously, the scores computed by this implementation did _not_ always agree with those given by Pedersen’s Perl implementation of WordNet Similarity. However, with the addition of the simulate_root flag (see below), the score for verbs now almost always agree but not always for nouns.

The LCS does not necessarily feature in the shortest path connecting the two senses, as it is by definition the common ancestor deepest in the taxonomy, not closest to the two senses. Typically, however, it will so feature. Where multiple candidates for the LCS exist, that whose shortest path to the root node is the longest will be selected. Where the LCS has multiple paths to the root, the longer path is used for the purposes of the calculation.

Parameters:
other (Synset) – The Synset that this Synset is being compared to.
simulate_root (bool) – The various verb taxonomies do not share a single root which disallows this metric from working for synsets that are not connected. This flag (True by default) creates a fake root that connects all the taxonomies. Set it to false to disable this behavior. For the noun taxonomy, there is usually a default root except for WordNet version 1.6. If you are using wordnet 1.6, a fake root will be added for nouns as well.
Returns:
A float score denoting the similarity of the two Synset objects, normally greater than zero. If no connecting path between the two senses can be found, None is returned.
class nltk.corpus.reader.WordNetICCorpusReader(root, fileids)¶
Bases: nltk.corpus.reader.api.CorpusReader

A corpus reader for the WordNet information content corpus.

ic(icfile)¶
Load an information content file from the wordnet_ic corpus and return a dictionary. This dictionary has just two keys, NOUN and VERB, whose values are dictionaries that map from synsets to information content values.

Parameters:	icfile (str) – The name of the wordnet_ic file (e.g. “ic-brown.dat”)
Returns:	An information content dictionary
class nltk.corpus.reader.SwitchboardCorpusReader(root, tag_mapping_function=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

discourses()¶
tagged_discourses(simplify_tags=False)¶
tagged_turns(simplify_tags=False)¶
tagged_words(simplify_tags=False)¶
turns()¶
words()¶
class nltk.corpus.reader.DependencyCorpusReader(root, fileids, encoding='utf8', word_tokenizer=<nltk.tokenize.simple.TabTokenizer object at 0x11057cbd0>, sent_tokenizer=RegexpTokenizer(pattern='n', gaps=True, discard_empty=True, flags=56), para_block_reader=<function read_blankline_block at 0x1104f45f0>)¶
Bases: nltk.corpus.reader.api.SyntaxCorpusReader

parsed_sents(fileids=None)¶
raw(fileids=None)¶
Returns:	the given file(s) as a single string.
Return type:	str
sents(fileids=None)¶
tagged_sents(fileids=None)¶
tagged_words(fileids=None)¶
words(fileids=None)¶
class nltk.corpus.reader.NombankCorpusReader(root, nomfile, framefiles=u'', nounsfile=None, parse_fileid_xform=None, parse_corpus=None, encoding=u'utf8')¶
Bases: nltk.corpus.reader.api.CorpusReader

Corpus reader for the nombank corpus, which augments the Penn Treebank with information about the predicate argument structure of every noun instance. The corpus consists of two parts: the predicate-argument annotations themselves, and a set of “frameset files” which define the argument labels used by the annotations, on a per-noun basis. Each “frameset file” contains one or more predicates, such as 'turn' or 'turn_on', each of which is divided into coarse-grained word senses called “rolesets”. For each “roleset”, the frameset file provides descriptions of the argument roles, along with examples.

instances(baseform=None)¶
Returns:	a corpus view that acts as a list of
NombankInstance objects, one for each noun in the corpus.
lines()¶
Returns:	a corpus view that acts as a list of strings, one for
each line in the predicate-argument annotation file.
nouns()¶
Returns:	a corpus view that acts as a list of all noun lemmas
in this corpus (from the nombank.1.0.words file).
raw(fileids=None)¶
Returns:	the text contents of the given fileids, as a single string.
roleset(roleset_id)¶
Returns:	the xml description for the given roleset.
rolesets(baseform=None)¶
Returns:	list of xml descriptions for rolesets.
class nltk.corpus.reader.IPIPANCorpusReader(root, fileids)¶
Bases: nltk.corpus.reader.api.CorpusReader

Corpus reader designed to work with corpus created by IPI PAN. See http://korpus.pl/en/ for more details about IPI PAN corpus.

The corpus includes information about text domain, channel and categories. You can access possible values using domains(), channels() and categories(). You can use also this metadata to filter files, e.g.: fileids(channel='prasa'), fileids(categories='publicystyczny').

The reader supports methods: words, sents, paras and their tagged versions. You can get part of speech instead of full tag by giving “simplify_tags=True” parameter, e.g.: tagged_sents(simplify_tags=True).

Also you can get all tags disambiguated tags specifying parameter “one_tag=False”, e.g.: tagged_paras(one_tag=False).

You can get all tags that were assigned by a morphological analyzer specifying parameter “disamb_only=False”, e.g. tagged_words(disamb_only=False).

The IPIPAN Corpus contains tags indicating if there is a space between two tokens. To add special “no space” markers, you should specify parameter “append_no_space=True”, e.g. tagged_words(append_no_space=True). As a result in place where there should be no space between two tokens new pair (‘’, ‘no-space’) will be inserted (for tagged data) and just ‘’ for methods without tags.

The corpus reader can also try to append spaces between words. To enable this option, specify parameter “append_space=True”, e.g. words(append_space=True). As a result either ‘ ‘ or (‘ ‘, ‘space’) will be inserted between tokens.

By default, xml entities like " and & are replaced by corresponding characters. You can turn off this feature, specifying parameter “replace_xmlentities=False”, e.g. words(replace_xmlentities=False).

categories(fileids=None)¶
channels(fileids=None)¶
domains(fileids=None)¶
fileids(channels=None, domains=None, categories=None)¶
paras(fileids=None, **kwargs)¶
raw(fileids=None)¶
sents(fileids=None, **kwargs)¶
tagged_paras(fileids=None, **kwargs)¶
tagged_sents(fileids=None, **kwargs)¶
tagged_words(fileids=None, **kwargs)¶
words(fileids=None, **kwargs)¶
class nltk.corpus.reader.Pl196xCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.api.CategorizedCorpusReader, nltk.corpus.reader.xmldocs.XMLCorpusReader

decode_tag(tag)¶
headLen = 2770¶
paras(fileids=None, categories=None, textids=None)¶
raw(fileids=None, categories=None)¶
sents(fileids=None, categories=None, textids=None)¶
tagged_paras(fileids=None, categories=None, textids=None)¶
tagged_sents(fileids=None, categories=None, textids=None)¶
tagged_words(fileids=None, categories=None, textids=None)¶
textids(fileids=None, categories=None)¶
In the pl196x corpus each category is stored in single file and thus both methods provide identical functionality. In order to accommodate finer granularity, a non-standard textids() method was implemented. All the main functions can be supplied with a list of required chunks—giving much more control to the user.
words(fileids=None, categories=None, textids=None)¶
xml(fileids=None, categories=None)¶
class nltk.corpus.reader.TEICorpusView(corpus_file, tagged, group_by_sent, group_by_para, tag_mapping_function=None, headLen=0, textids=None)¶
Bases: nltk.corpus.reader.util.StreamBackedCorpusView

read_block(stream)¶
class nltk.corpus.reader.KNBCorpusReader(root, fileids, encoding='utf8', morphs2str=<function <lambda> at 0x110593a28>)¶
Bases: nltk.corpus.reader.api.SyntaxCorpusReader

This class implements:
__init__, which specifies the location of the corpus and a method for detecting the sentence blocks in corpus files.
_read_block, which reads a block from the input stream.
_word, which takes a block and returns a list of list of words.
_tag, which takes a block and returns a list of list of tagged words.
_parse, which takes a block and returns a list of parsed sentences.
The structure of tagged words:
tagged_word = (word(str), tags(tuple)) tags = (surface, reading, lemma, pos1, posid1, pos2, posid2, pos3, posid3, others ...)
class nltk.corpus.reader.ChasenCorpusReader(root, fileids, encoding='utf8', sent_splitter=None)¶
Bases: nltk.corpus.reader.api.CorpusReader

paras(fileids=None)¶
raw(fileids=None)¶
sents(fileids=None)¶
tagged_paras(fileids=None)¶
tagged_sents(fileids=None)¶
tagged_words(fileids=None)¶
words(fileids=None)¶
class nltk.corpus.reader.CHILDESCorpusReader(root, fileids, lazy=True)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

Corpus reader for the XML version of the CHILDES corpus. The CHILDES corpus is available at http://childes.psy.cmu.edu/. The XML version of CHILDES is located at http://childes.psy.cmu.edu/data-xml/. Copy the needed parts of the CHILDES XML corpus into the NLTK data directory (nltk_data/corpora/CHILDES/).

For access to the file text use the usual nltk functions, words(), sents(), tagged_words() and tagged_sents().

MLU(fileids=None, speaker='CHI')¶
Returns:	the given file(s) as a floating number
Return type:	list(float)
age(fileids=None, speaker='CHI', month=False)¶
Returns:	the given file(s) as string or int
Return type:	list or int
Parameters:	month – If true, return months instead of year-month-date
childes_url_base = 'http://childes.psy.cmu.edu/browser/index.php?url='¶
convert_age(age_year)¶
Caclculate age in months from a string in CHILDES format
corpus(fileids=None)¶
Returns:	the given file(s) as a dict of (corpus_property_key, value)
Return type:	list(dict)
participants(fileids=None)¶
Returns:	the given file(s) as a dict of (participant_property_key, value)
Return type:	list(dict)
sents(fileids=None, speaker='ALL', stem=False, relation=None, strip_space=True, replace=False)¶
Returns:
the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:
list(list(str))
Parameters:
speaker – If specified, select specific speaker(s) defined in the corpus. Default is ‘ALL’ (all participants). Common choices are ‘CHI’ (the child), ‘MOT’ (mother), [‘CHI’,’MOT’] (exclude researchers)
stem – If true, then use word stems instead of word strings.
relation – If true, then return tuples of (str,pos,relation_list). If there is manually-annotated relation info, it will return tuples of (str,pos,test_relation_list,str,pos,gold_relation_list)
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
replace – If true, then use the replaced (intended) word instead of the original word (e.g., ‘wat’ will be replaced with ‘watch’)
tagged_sents(fileids=None, speaker='ALL', stem=False, relation=None, strip_space=True, replace=False)¶
Returns:
the given file(s) as a list of sentences, each encoded as a list of (word,tag) tuples.
Return type:
list(list(tuple(str,str)))
Parameters:
speaker – If specified, select specific speaker(s) defined in the corpus. Default is ‘ALL’ (all participants). Common choices are ‘CHI’ (the child), ‘MOT’ (mother), [‘CHI’,’MOT’] (exclude researchers)
stem – If true, then use word stems instead of word strings.
relation – If true, then return tuples of (str,pos,relation_list). If there is manually-annotated relation info, it will return tuples of (str,pos,test_relation_list,str,pos,gold_relation_list)
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
replace – If true, then use the replaced (intended) word instead of the original word (e.g., ‘wat’ will be replaced with ‘watch’)
tagged_words(fileids=None, speaker='ALL', stem=False, relation=False, strip_space=True, replace=False)¶
Returns:
the given file(s) as a list of tagged words and punctuation symbols, encoded as tuples (word,tag).
Return type:
list(tuple(str,str))
Parameters:
speaker – If specified, select specific speaker(s) defined in the corpus. Default is ‘ALL’ (all participants). Common choices are ‘CHI’ (the child), ‘MOT’ (mother), [‘CHI’,’MOT’] (exclude researchers)
stem – If true, then use word stems instead of word strings.
relation – If true, then return tuples of (stem, index, dependent_index)
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
replace – If true, then use the replaced (intended) word instead of the original word (e.g., ‘wat’ will be replaced with ‘watch’)
webview_file(fileid, urlbase=None)¶
Map a corpus file to its web version on the CHILDES website, and open it in a web browser.

The complete URL to be used is:
childes.childes_url_base + urlbase + fileid.replace(‘.xml’, ‘.cha’)
If no urlbase is passed, we try to calculate it. This requires that the childes corpus was set up to mirror the folder hierarchy under childes.psy.cmu.edu/data-xml/, e.g.: nltk_data/corpora/childes/Eng-USA/Cornell/??? or nltk_data/corpora/childes/Romance/Spanish/Aguirre/???

The function first looks (as a special case) if “Eng-USA” is on the path consisting of <corpus root>+fileid; then if “childes”, possibly followed by “data-xml”, appears. If neither one is found, we use the unmodified fileid and hope for the best. If this is not right, specify urlbase explicitly, e.g., if the corpus root points to the Cornell folder, urlbase=’Eng-USA/Cornell’.
words(fileids=None, speaker='ALL', stem=False, relation=False, strip_space=True, replace=False)¶
Returns:
the given file(s) as a list of words
Return type:
list(str)
Parameters:
speaker – If specified, select specific speaker(s) defined in the corpus. Default is ‘ALL’ (all participants). Common choices are ‘CHI’ (the child), ‘MOT’ (mother), [‘CHI’,’MOT’] (exclude researchers)
stem – If true, then use word stems instead of word strings.
relation – If true, then return tuples of (stem, index, dependent_index)
strip_space – If true, then strip trailing spaces from word tokens. Otherwise, leave the spaces on the tokens.
replace – If true, then use the replaced (intended) word instead of the original word (e.g., ‘wat’ will be replaced with ‘watch’)
class nltk.corpus.reader.AlignedCorpusReader(root, fileids, sep='/', word_tokenizer=WhitespaceTokenizer(pattern=u'\s+', gaps=True, discard_empty=True, flags=56), sent_tokenizer=RegexpTokenizer(pattern='n', gaps=True, discard_empty=True, flags=56), alignedsent_block_reader=<function read_alignedsent_block at 0x1104f4668>, encoding='latin1')¶
Bases: nltk.corpus.reader.api.CorpusReader

Reader for corpora of word-aligned sentences. Tokens are assumed to be separated by whitespace. Sentences begin on separate lines.

aligned_sents(fileids=None)¶
Returns:	the given file(s) as a list of AlignedSent objects.
Return type:	list(AlignedSent)
raw(fileids=None)¶
Returns:	the given file(s) as a single string.
Return type:	str
sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences or utterances, each encoded as a list of word strings.
Return type:	list(list(str))
words(fileids=None)¶
Returns:	the given file(s) as a list of words and punctuation symbols.
Return type:	list(str)
class nltk.corpus.reader.TimitTaggedCorpusReader(*args, **kwargs)¶
Bases: nltk.corpus.reader.tagged.TaggedCorpusReader

A corpus reader for tagged sentences that are included in the TIMIT corpus.

paras()¶
tagged_paras()¶
class nltk.corpus.reader.LinThesaurusCorpusReader(root, badscore=0.0)¶
Bases: nltk.corpus.reader.api.CorpusReader

Wrapper for the LISP-formatted thesauruses distributed by Dekang Lin.

scored_synonyms(ngram, fileid=None)¶
Returns a list of scored synonyms (tuples of synonyms and scores) for the current ngram

Parameters:
ngram (C{string}) – ngram to lookup
fileid (C{string}) – thesaurus fileid to search in. If None, search all fileids.
Returns:
If fileid is specified, list of tuples of scores and synonyms; otherwise, list of tuples of fileids and lists, where inner lists consist of tuples of scores and synonyms.
similarity(ngram1, ngram2, fileid=None)¶
Returns the similarity score for two ngrams.

Parameters:
ngram1 (C{string}) – first ngram to compare
ngram2 (C{string}) – second ngram to compare
fileid (C{string}) – thesaurus fileid to search in. If None, search all fileids.
Returns:
If fileid is specified, just the score for the two ngrams; otherwise, list of tuples of fileids and scores.
synonyms(ngram, fileid=None)¶
Returns a list of synonyms for the current ngram.

Parameters:
ngram (C{string}) – ngram to lookup
fileid (C{string}) – thesaurus fileid to search in. If None, search all fileids.
Returns:
If fileid is specified, list of synonyms; otherwise, list of tuples of fileids and lists, where inner lists contain synonyms.
class nltk.corpus.reader.SemcorCorpusReader(root, fileids, lazy=True)¶
Bases: nltk.corpus.reader.xmldocs.XMLCorpusReader

Corpus reader for the SemCor Corpus. For access to the complete XML data structure, use the xml() method. For access to simple word lists and tagged word lists, use words(), sents(), tagged_words(), and tagged_sents().

chunk_sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences, each encoded as a list of chunks.
Return type:	list(list(list(str)))
chunks(fileids=None)¶
Returns:	the given file(s) as a list of chunks, each of which is a list of words and punctuation symbols that form a unit.
Return type:	list(list(str))
sents(fileids=None)¶
Returns:	the given file(s) as a list of sentences, each encoded as a list of word strings.
Return type:	list(list(str))
tagged_chunks(fileids=None, tag=u'pos')¶
Returns:	the given file(s) as a list of tagged chunks, represented in tree form.
Return type:	list(Tree)
Parameters:	tag – ‘pos’ (part of speech), ‘sem’ (semantic), or ‘both’ to indicate the kind of tags to include. Semantic tags consist of WordNet lemma IDs, plus an ‘NE’ node if the chunk is a named entity without a specific entry in WordNet. (Named entities of type ‘other’ have no lemma. Other chunks not in WordNet have no semantic tag. Punctuation tokens have None for their part of speech tag.)
tagged_sents(fileids=None, tag=u'pos')¶
Returns:	the given file(s) as a list of sentences. Each sentence is represented as a list of tagged chunks (in tree form).
Return type:	list(list(Tree))
Parameters:	tag – ‘pos’ (part of speech), ‘sem’ (semantic), or ‘both’ to indicate the kind of tags to include. Semantic tags consist of WordNet lemma IDs, plus an ‘NE’ node if the chunk is a named entity without a specific entry in WordNet. (Named entities of type ‘other’ have no lemma. Other chunks not in WordNet have no semantic tag. Punctuation tokens have None for their part of speech tag.)
words(fileids=None)¶
Returns:	the given file(s) as a list of words and punctuation symbols.
Return type:	list(str)
class nltk.corpus.reader.UdhrCorpusReader(root=u'udhr')¶
Bases: nltk.corpus.reader.plaintext.PlaintextCorpusReader

ENCODINGS = [(u'.*-Latin1$', u'latin-1'), (u'.*-Hebrew$', u'hebrew'), (u'.*-Arabic$', u'cp1256'), (u'Czech_Cesky-UTF8', u'cp1250'), (u'.*-Cyrillic$', u'cyrillic'), (u'.*-SJIS$', u'SJIS'), (u'.*-GB2312$', u'GB2312'), (u'.*-Latin2$', u'ISO-8859-2'), (u'.*-Greek$', u'greek'), (u'.*-UTF8$', u'utf-8'), (u'Hungarian_Magyar-Unicode', u'utf-16-le'), (u'Amahuaca', u'latin1'), (u'Turkish_Turkce-Turkish', u'latin5'), (u'Lithuanian_Lietuviskai-Baltic', u'latin4'), (u'Japanese_Nihongo-EUC', u'EUC-JP'), (u'Japanese_Nihongo-JIS', u'iso2022_jp'), (u'Chinese_Mandarin-HZ', u'hz'), (u'Abkhaz\\-Cyrillic\\+Abkh', u'cp1251')]¶
SKIP = set([u'Vietnamese-TCVN', u'Vietnamese-VPS', u'Gujarati-UTF8', u'Marathi-UTF8', u'Chinese_Mandarin-HZ', u'Azeri_Azerbaijani_Latin-Az.Times.Lat0117', u'Bhojpuri-Agra', u'Magahi-UTF8', u'Tigrinya_Tigrigna-VG2Main', u'Hungarian_Magyar-Unicode', u'Magahi-Agra', u'Armenian-DallakHelv', u'Chinese_Mandarin-UTF8', u'Burmese_Myanmar-WinResearcher', u'Amharic-Afenegus6..60375', u'Azeri_Azerbaijani_Cyrillic-Az.Times.Cyr.Normal0117', u'Japanese_Nihongo-JIS', u'Burmese_Myanmar-UTF8', u'Czech-Latin2-err', u'Navaho_Dine-Navajo-Navaho-font', u'Lao-UTF8', u'Russian_Russky-UTF8~', u'Esperanto-T61', u'Vietnamese-VIQR', u'Tamil-UTF8'])¶

page 7
table Module ¶

Tkinter widgets for displaying multi-column listboxes and tables.

class nltk.draw.table.MultiListbox(master, columns, column_weights=None, cnf={}, **kw)[source]¶
Bases: Tkinter.Frame

A multi-column listbox, where the current selection applies to an entire row. Based on the MultiListbox Tkinter widget recipe from the Python Cookbook (http://code.activestate.com/recipes/52266/ )

For the most part, MultiListbox methods delegate to its contained listboxes. For any methods that do not have docstrings, see Tkinter.Listbox for a description of what that method does.

FRAME_CONFIG = {'highlightthickness': 1, 'takefocus': True, 'background': '#888'}¶
Default configuration values for the frame.
LABEL_CONFIG = {'foreground': 'white', 'font': 'helvetica -16 bold', 'relief': 'raised', 'background': '#444', 'borderwidth': 1}¶
Default configurations for the column labels.
LISTBOX_CONFIG = {'borderwidth': 1, 'highlightthickness': 0, 'activestyle': 'none', 'selectborderwidth': 0, 'selectbackground': '#888', 'takefocus': False, 'exportselection': False}¶
Default configuration for the column listboxes.
activate(*args, **kwargs)[source]¶
bbox(row, col)[source]¶
Return the bounding box for the given table cell, relative to this widget’s top-left corner. The bounding box is a tuple of integers (left, top, width, height).
bind_to_columns(sequence=None, func=None, add=None)[source]¶
Add a binding to each Tkinter.Label and Tkinter.Listbox widget in this mult-column listbox that will call func in response to the event sequence.

Returns:	A list of the identifiers of replaced binding functions (if any), allowing for their deletion (to prevent a memory leak).
bind_to_labels(sequence=None, func=None, add=None)[source]¶
Add a binding to each Tkinter.Label widget in this mult-column listbox that will call func in response to the event sequence.

Returns:	A list of the identifiers of replaced binding functions (if any), allowing for their deletion (to prevent a memory leak).
bind_to_listboxes(sequence=None, func=None, add=None)[source]¶
Add a binding to each Tkinter.Listbox widget in this mult-column listbox that will call func in response to the event sequence.

Returns:	A list of the identifiers of replaced binding functions (if any), allowing for their deletion (to prevent a memory leak).
column_labels[source]¶
A tuple containing the Tkinter.Label widgets used to display the label of each column. If this multi-column listbox was created without labels, then this will be an empty tuple. These widgets will all be augmented with a column_index attribute, which can be used to determine which column they correspond to. This can be convenient, e.g., when defining callbacks for bound events.
column_names[source]¶
A tuple containing the names of the columns used by this multi-column listbox.
columnconfig(col_index, cnf={}, **kw)¶
Configure all table cells in the given column. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
columnconfigure(col_index, cnf={}, **kw)[source]¶
Configure all table cells in the given column. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
configure(cnf={}, **kw)[source]¶
Configure this widget. Use label_* to configure all labels; and listbox_* to configure all listboxes. E.g.:

>>> mlb = MultiListbox(master, 5)
>>> mlb.configure(label_foreground='red')
>>> mlb.configure(listbox_foreground='red')
curselection(*args, **kwargs)[source]¶
delete(*args, **kwargs)[source]¶
get(first, last=None)[source]¶
Return the value(s) of the specified row(s). If last is not specified, then return a single row value; otherwise, return a list of row values. Each row value is a tuple of cell values, one for each column in the row.
hide_column(col_index)[source]¶
Hide the given column. The column’s state is still maintained: its values will still be returned by get(), and you must supply its values when calling insert(). It is safe to call this on a column that is already hidden.

See :	show_column()
index(*args, **kwargs)[source]¶
insert(index, *rows)[source]¶
Insert the given row or rows into the table, at the given index. Each row value should be a tuple of cell values, one for each column in the row. Index may be an integer or any of the special strings (such as 'end') accepted by Tkinter.Listbox.
itemcget(*args, **kwargs)[source]¶
itemconfig(row_index, col_index, cnf=None, **kw)¶
Configure the table cell at the given row and column. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
itemconfigure(row_index, col_index, cnf=None, **kw)[source]¶
Configure the table cell at the given row and column. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
listboxes[source]¶
A tuple containing the Tkinter.Listbox widgets used to display individual columns. These widgets will all be augmented with a column_index attribute, which can be used to determine which column they correspond to. This can be convenient, e.g., when defining callbacks for bound events.
nearest(*args, **kwargs)[source]¶
rowconfig(row_index, cnf={}, **kw)¶
Configure all table cells in the given row. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
rowconfigure(row_index, cnf={}, **kw)[source]¶
Configure all table cells in the given row. Valid keyword arguments are: background, bg, foreground, fg, selectbackground, selectforeground.
scan_dragto(*args, **kwargs)[source]¶
scan_mark(*args, **kwargs)[source]¶
see(*args, **kwargs)[source]¶
select(index=None, delta=None, see=True)[source]¶
Set the selected row. If index is specified, then select row index. Otherwise, if delta is specified, then move the current selection by delta (negative numbers for up, positive numbers for down). This will not move the selection past the top or the bottom of the list.

Parameters:	see – If true, then call self.see() with the newly selected index, to ensure that it is visible.
select_anchor(*args, **kwargs)¶
select_clear(*args, **kwargs)¶
select_includes(*args, **kwargs)¶
select_set(*args, **kwargs)¶
selection_anchor(*args, **kwargs)[source]¶
selection_clear(*args, **kwargs)[source]¶
selection_includes(*args, **kwargs)[source]¶
selection_set(*args, **kwargs)[source]¶
show_column(col_index)[source]¶
Display a column that has been hidden using hide_column(). It is safe to call this on a column that is not hidden.
size(*args, **kwargs)[source]¶
yview(*args, **kwargs)[source]¶
yview_moveto(*args, **kwargs)[source]¶
yview_scroll(*args, **kwargs)[source]¶
class nltk.draw.table.Table(master, column_names, rows=None, column_weights=None, scrollbar=True, click_to_sort=True, reprfunc=None, cnf={}, **kw)[source]¶
Bases: object

A display widget for a table of values, based on a MultiListbox widget. For many purposes, Table can be treated as a list-of-lists. E.g., table[i] is a list of the values for row i; and table.append(row) adds a new row with the given lits of values. Individual cells can be accessed using table[i,j], which refers to the j-th column of the i-th row. This can be used to both read and write values from the table. E.g.:

>>> table[i,j] = 'hello'
The column (j) can be given either as an index number, or as a column name. E.g., the following prints the value in the 3rd row for the ‘First Name’ column:

>>> print(table[3, 'First Name'])
John
You can configure the colors for individual rows, columns, or cells using rowconfig(), columnconfig(), and itemconfig(). The color configuration for each row will be preserved if the table is modified; however, when new rows are added, any color configurations that have been made for columns will not be applied to the new row.

Note: Although Table acts like a widget in some ways (e.g., it defines grid(), pack(), and bind()), it is not itself a widget; it just contains one. This is because widgets need to define __getitem__(), __setitem__(), and __nonzero__() in a way that’s incompatible with the fact that Table behaves as a list-of-lists.

Variables:
_mlb – The multi-column listbox used to display this table’s data.
_rows – A list-of-lists used to hold the cell values of this table. Each element of _rows is a row value, i.e., a list of cell values, one for each column in the row.
append(rowvalue)[source]¶
Add a new row to the end of the table.

Parameters:	rowvalue – A tuple of cell values, one for each column in the new row.
bind(sequence=None, func=None, add=None)[source]¶
Add a binding to this table’s main frame that will call func in response to the event sequence.
bind_to_columns(sequence=None, func=None, add=None)[source]¶
See :	MultiListbox.bind_to_columns()
bind_to_labels(sequence=None, func=None, add=None)[source]¶
See :	MultiListbox.bind_to_labels()
bind_to_listboxes(sequence=None, func=None, add=None)[source]¶
See :	MultiListbox.bind_to_listboxes()
clear()[source]¶
Delete all rows in this table.
column_index(i)[source]¶
If i is a valid column index integer, then return it as is. Otherwise, check if i is used as the name for any column; if so, return that column’s index. Otherwise, raise a KeyError exception.
column_names[source]¶
A list of the names of the columns in this table.
columnconfig(col_index, cnf={}, **kw)¶
See :	MultiListbox.columnconfigure()
columnconfigure(col_index, cnf={}, **kw)[source]¶
See :	MultiListbox.columnconfigure()
extend(rowvalues)[source]¶
Add new rows at the end of the table.

Parameters:	rowvalues – A list of row values used to initialze the table. Each row value should be a tuple of cell values, one for each column in the row.
focus()[source]¶
Direct (keyboard) input foxus to this widget.
grid(*args, **kwargs)[source]¶
Position this table’s main frame widget in its parent widget. See Tkinter.Frame.grid() for more info.
hide_column(column_index)[source]¶
See :	MultiListbox.hide_column()
insert(row_index, rowvalue)[source]¶
Insert a new row into the table, so that its row index will be row_index. If the table contains any rows whose row index is greater than or equal to row_index, then they will be shifted down.

Parameters:	rowvalue – A tuple of cell values, one for each column in the new row.
itemconfig(row_index, col_index, cnf=None, **kw)¶
See :	MultiListbox.itemconfigure()
itemconfigure(row_index, col_index, cnf=None, **kw)[source]¶
See :	MultiListbox.itemconfigure()
pack(*args, **kwargs)[source]¶
Position this table’s main frame widget in its parent widget. See Tkinter.Frame.pack() for more info.
rowconfig(row_index, cnf={}, **kw)¶
See :	MultiListbox.rowconfigure()
rowconfigure(row_index, cnf={}, **kw)[source]¶
See :	MultiListbox.rowconfigure()
select(index=None, delta=None, see=True)[source]¶
See :	MultiListbox.select()
selected_row()[source]¶
Return the index of the currently selected row, or None if no row is selected. To get the row value itself, use table[table.selected_row()].
show_column(column_index)[source]¶
See :	MultiListbox.show_column()
sort_by(column_index, order='toggle')[source]¶
Sort the rows in this table, using the specified column’s values as a sort key.

Parameters:
column_index – Specifies which column to sort, using either a column index (int) or a column’s label name (str).
order –
Specifies whether to sort the values in ascending or descending order:

'ascending': Sort from least to greatest.
'descending': Sort from greatest to least.
'toggle': If the most recent call to sort_by() sorted the table by the same column (column_index), then reverse the rows; otherwise sort in ascending order.
nltk.draw.table.demo()[source]¶

page 8
pt Module ¶

nltk.examples.pt.sents()[source]¶
nltk.examples.pt.texts()[source]¶
Table Of Contents

Search

Enter search terms or a module, class or function name.



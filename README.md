plane_trie
==========

Read only Trie (patricia like tree).

Build from sorted string list.
Load via mmap.
Nodes are stored in postorder i.e root is last.


build from splited sorted file.
nodes are writen in file in postorder.

TODO: merge tries


half_trie:
words are plited in two parts
prefix_trie -  plane trie build of prefixes
second_tries - set of tries. each corresponds to it's own prefix.


first: search for prefix in prefix trie
second: search for rest of the word in corresponding second trie

TODO: UT for half trie

TODO: fb-trie - two half_tries one with words in straight order - another with reversed words
TODO: UT for fb-trie

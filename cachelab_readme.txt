cachelab_readme.txt

Name: Joseph Scollo
Special Instructions for compiling the program:

If compiling in linux terminal make sure to set lm flag.

Special Instructions for running the program:
program must be run with specific arguments to get desired result.
 example : -m 64 -s 2 -e 1 -b 3 -i address02 -r fifo

 m 64 = total bits and unique addresses in the cache (log(2)M)
 e 1 = number of lines in the cache  (log(2)E)
 s 2 = number of sets in the cache   (log(2)S)
 b 3 = represent data block size.     (log(2)B)

 the above four parameters determine the type of cache, direct, fully associative and set associative.

 -i = address file, you must set the file path to where your address text file to run the program.
 - r = represents the replacement policy. supports "fifo", "optimal" and "lru".

 Note: Type in the file path on line 171.
# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show
# Create empty queue
new
# See how long it is
size
# Fill it with some values.  First at the head
ih dolphin
ih bear
ih gerbil
# Now at the tail
it meerkat
it bear
it RAND 5
# Reverse it
reverse
# Swap it
swap
# Delete mid
dm
# Sort it
sort
# Shuffle
shuffle
# See how long it is
size
# Delete duplicate
dedup
# Delete queue.  Goes back to a NULL queue.
free
# Exit program
quit

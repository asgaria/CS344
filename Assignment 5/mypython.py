import string
import random 
from random import randint 


files = [open("file%02d.txt"%x, "w+r") for x in xrange(0,3)]

for y in xrange(0,3):
	lower_choices = string.letters.lower()
	for z in xrange(0,10):
		files[y].write(random.choice(lower_choices))

	files[y].write('\n')		
	files[y].seek(0)	
	temp = files[y].read()
	print(("File %d contains: "%y) + temp)
	files[y].close()

a = randint(1,42)
b = randint(1,42)
product = a * b
print("Number 1: %d"%a)
print("Number 2: %d"%b)

print("Product: %d"%product)


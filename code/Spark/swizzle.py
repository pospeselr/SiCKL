#!/usr/bin/python

def int_to_str(val):
	if val == 1:
		return 'X'
	elif val == 2:
		return 'Y'
	elif val == 3:
		return 'Z'
	elif val == 4:
		return 'W'
	return 'FAIL'

for i in range(1, 5):
	base5 = i
	name = int_to_str(i)
	print(name + " = " + str(base5) + ",")

for i in range(1, 5):
	for j in range(1, 5):
		base5 = i * 5 + j
		name = int_to_str(i) + int_to_str(j)
		print(name + " = " + str(base5) + ",")

for i in range(1, 5):
	for j in range(1, 5):
		for k in range(1, 5):
			base5 = i * 25 + j * 5 + k
			name = int_to_str(i) + int_to_str(j) + int_to_str(k)
			print(name + " = " + str(base5) + ",")

for i in range(1, 5):
	for j in range(1, 5):
		for k in range(1, 5):
			for l in range(1, 5):
				base5 = i * 125 + j * 25 + k * 5 + l
				name = int_to_str(i) + int_to_str(j) + int_to_str(k) + int_to_str(l)
				print(name + " = " + str(base5) + ",")


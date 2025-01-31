# Generates traces for the following C code:
'''
int x[n],y[n],z[n],max[n],i
main(){
    for (i=0; i<max; i++)
        x[i]=x[i] + y[i]
        z[i]=z[i] + y[i]
}
'''

n = 2                  # The number of iterations of the loop
top_addr = "0x08"
addr_length = 8 + 2     #The 2 corresponds to the 0x
current_address = 0

## The addresses are initiated
dirx = []
diry = []
dirz = []
dirmax = 0
diri = 0
values = dict()

for _ in range(n):
    dirx.append(None)
    diry.append(None)
    dirz.append(None)

#The addresses for x, y and z are calculated
for k in range(3):
	for i in range(n):
		# The padding's length is calculated
		padding_length = addr_length - len(top_addr) - len(f"{current_address:X}")

        # The address is calculated
		address = top_addr + '0' * padding_length + f"{current_address:X}"

		# The address is saved on the corresponding var
		if k == 0:
			dirx[i] = address
		elif k == 1:
			diry[i] = address
		elif k == 2:
			dirz[i] = address
		print("Generated ", address)
		current_address+=4
		values[address] = i + k * n
		print("Assigned value", values[address])

address = top_addr + '0' * padding_length + f"{current_address:X}"
dirmax = address
values[address] = n * 3
print("Generated ", address)
print("Assigned value", values[address])
current_address+=4

address = top_addr + '0' * padding_length + f"{current_address:X}"
diri = address
values[address] = n * 3 + 1
print("Generated ", address)
print("Assigned value", values[address])

print("\n Trace start:\n")

# The address range is printed
print("# x =", dirx[0], dirx[n-1])
print("# y =", diry[0], diry[n-1])
print("# z =", dirz[0], dirz[n-1])
print("# max =", dirmax)
print("# i =", diri)

print("\n\n")

# I is initiated to 0
print("\n# i = 0")
print("S", diri ,"D 4 0")

# i and max are loaded and compared
print("\n# i < max")
print("L", diri ,"D")
print("L", dirmax,"D")

# On each iteration of the C for loop
for i in range(n):
    # x and y are fetched and the result is stored
	print("\n# x[", i, "] = x[", i, "] + y[", i, "]")
	print("L", dirx[i],"D")
	print("L", diry[i],"D")
	print("S", dirx[i],"D 4", values[diry[i]] + values[dirx[i]])

    # same for z
	print("\n# z[", i, "] = z[", i, "] + y[", i, "]")
	print("L", dirz[i],"D")
	print("L", diry[i],"D")
	print("S", dirz[i],"D 4", values[diry[i]] + values[dirz[i]])

    # i is incremented
	print("\n# i++")
	print("L", diri ,"D")
	print("S", diri ,"D 4", i + 1)

    # i and max are loaded and compared
	print("\n# i < max")
	print("L", diri ,"D")
	print("L", dirmax,"D")

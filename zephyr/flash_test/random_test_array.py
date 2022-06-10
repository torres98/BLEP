from random import randint

array_size = int(input("Insert the desired size: "))

f = open("src/flash_array.h", "w")
f.write(f'#define TEST_ARRAY_SIZE {array_size}\n\nconst unsigned int test_array[TEST_ARRAY_SIZE] = {{')

for _ in range(array_size):
    f.write(f'{randint(0, 255)}, ')

f.write("};")
f.close()

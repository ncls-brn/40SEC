import multiprocessing
import os


manager = multiprocessing.Manager()
numbers = manager.list([1, 2, 3])

def print_hello():
    for _ in range(10):
        print(numbers[:])  
        print(f"Hello from process {os.getppid()}")
        numbers.append(4)

def print_hi():
    for _ in range(10):
        numbers[0] = 5
        numbers[1] = 6
        numbers[2] = 7
        print(numbers[:])  
        print(f"Hi from process {os.getppid()}")

p1 = multiprocessing.Process(target=print_hello)
p2 = multiprocessing.Process(target=print_hi)


p1.start()
p2.start()


p1.join()
p2.join()

print("Programme terminé.")

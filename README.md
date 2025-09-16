# Simple Concurrent Reader/Writer Pattern

In this college assignment, I implement a simple Reader/Writer pattern concurrently using C and the pthread.h library.

In the assignment, there are a number of threads concurrently summing up a global variable while another thread prints whenever the global variable is a multiple of 1000.

The printer thread should only print multiples of 1000, should print every multiple of 1000 and only once for each one.
___

To achieve that, we use the following logic:

In the writer thread:

first we lock the mutex, then check if I should print the number, If so, I wait until the number is printed. After I'm done waiting, I sum up once and check if the number is a multiple of 1000, if so, I update the state variable. Then signal the printer thread that it should print. Note that, When I signal here, Another writer thread could get CPU time instead of the printer thread, that's why we check wether the state says you should print or have already printed the number before summing up.
In pseudocode:
```python
lock()

while should_print
    wait(condition_write)
sum++
if sum is multiple of 1000
    should_print = False

signal(condition_read)

unlock()
```

In the reader thread:

first we check If I have already printed (this is necessary because, whenever the printer thread broadcasts, It could pick up CPU time instead of a writer thread as it's not yet waiting). Then, After I wake up I print and change the state variable to remember I already printed. After that, I broadcast to every worker thread that they can sum up again.
In pseudocode:
```python
lock()

while not should_print
    wait(condition_read)

print(sum)
should_print = False

broadcast(condition_write)

unlock()
```
___
To build the program, in src:
```
>>> make main
```
To run the program, in src:
```
>>> bin/main <number of producer threads>
```
Note that for our purpose, there is always only one reader thread.

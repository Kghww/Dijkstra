# Terrain Pathfinding with Lifts
**Algorithms and Data Structures – Project 2024/2025**

## Project Overview

The goal of this task is to **find the shortest travel time across a terrain map**.

The terrain is represented as a **grid of fields**, where each field has a **height value**.  
Moving between fields takes time depending on the **difference in height**.

Additionally, the terrain may contain **lifts** that allow faster travel between distant locations.  
However, lifts operate only at specific time intervals, which means the traveler may need to **wait before departure**.

The objective is to compute the **minimum time required to reach the destination** from the starting point.

---

# Terrain Map

The terrain is represented as a **2D grid** with dimensions:

```
width × height
```

Each cell contains a **non-negative integer representing its height**.

Movement rules:

- You can move only to **adjacent cells sharing a side**
- From each cell you can move to **at most four neighbors**:
  - up
  - down
  - left
  - right
- Movement **outside the map is not allowed**

---

# Movement Cost

Moving from a field of height **B** to a field of height **A** takes:

```
A - B + 1 minutes   if A > B
1 minute            if A ≤ B
```

This means:

- Moving **uphill** is slower
- Moving **downhill or flat** always costs **1 minute**

---

# Lifts

The map may contain **lifts**, which allow one-way travel between two positions.

Each lift is defined by:

- **starting position** `(column, row)`
- **destination position** `(column, row)`
- **travel time**
- **departure interval**

### Lift Timing

A lift can only be used at times that are **multiples of its departure interval**.

If you arrive at a lift before its next departure time, you must **wait**.

Example:

```
Arrival time: 8
Lift interval: 5
Travel time: 3
```

Next departure occurs at minute **10**, so:

```
Waiting time = 2
Travel time = 3
Arrival = 8 + 2 + 3 = 13
```

---

# Input Format

The input contains the following values in order:

```
width height
start_column start_row
destination_column destination_row
number_of_lifts
```

Then the description of each lift:

```
start_column start_row
destination_column destination_row
travel_time
departure_interval
```

Finally, the **terrain height map** is given:

```
height_0_0 height_0_1 ... height_0_w
height_1_0 height_1_1 ... height_1_w
...
```

All values are **non-negative integers**.

It can be assumed that **all calculations fit within the `int` type range**.

---

# Output

The program should output a **single number**:

```
shortest_time
```

This is the **minimum number of minutes required to reach the destination** from the starting position.

---

# Example 1

### Input

```
9 8 0 0 8 7 0
0 0 0 0 0 0 0 0 8 
0 9 9 9 9 9 9 9 0 
0 1 0 1 0 0 0 9 0 
0 9 0 0 0 1 0 9 0 
0 9 1 1 1 1 0 9 0 
0 9 0 0 0 0 0 9 0 
0 9 9 9 9 1 9 9 0 
8 0 0 0 0 0 0 0 0 
```

### Output

```
18
```

---

# Example 2

### Input

```
9 9 0 0 8 8 5
0 1 2 0 5 2
0 1 2 0 1 3
2 0 4 0 3 7
4 1 8 8 20 11
4 2 8 8 10 12
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
0 9 0 9 0 9 0 9 0
```

### Output

```
22
```

---

# Explanation for Example 2

1. Move **one step down** – cost **1 minute**
2. Two lifts are available:
   - one departs every **2 minutes**
   - one departs every **3 minutes**

Choosing the second lift results in:

```
Arrival at (2,0) at minute 4
```

Then:

- wait **3 minutes** for the next lift
- travel to **(4,0)** and arrive at **minute 10**
- move **two steps down** to **(4,2)** (arrive at minute 12)
- take the lift to **(8,8)**

Final arrival time:

```
22 minutes
```

---

# Problem Type

This problem is a **shortest path problem on a weighted graph** with:

- grid-based movement
- additional edges representing lifts
- time-dependent edges due to lift schedules

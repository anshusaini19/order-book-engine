# In-Memory Order Book Engine

## Overview

This project implements a simple **in-memory limit order book** for a single instrument.
The engine processes BUY and SELL orders, matches them using **price-time priority**, and prints trade executions in real time.

Orders are read from **standard input**, and the final state of the order book (top bid/ask levels) is printed after all inputs are processed.

---

## Design Decisions

* The order book is divided into **two sides**:

  * **Bids (Buy Orders)** – sorted by highest price first
  * **Asks (Sell Orders)** – sorted by lowest price first
* Orders at the same price level are executed using **FIFO (time priority)**.
* Incoming orders are matched against the opposite side of the book until the quantity is filled or no price match exists.

---

## Data Structures Used

* `map<double, deque<Order>, greater<double>>` → stores **BUY orders**
* `map<double, deque<Order>>` → stores **SELL orders**
* `deque` ensures **first-in-first-out execution** for orders at the same price.
* `unordered_map<string, pair<string,double>>` is used to quickly locate orders for **CANCEL operations**.

---

## Build

Compile using:

g++ main.cpp -o engine

---

## Run

Example usage:

engine < input.txt

---

## Features Implemented

* Limit orders
* Market orders (price = 0)
* Price-time priority matching
* Partial fills
* Order cancellation
* Final aggregated order book output

---

## Limitations

* Supports only **one instrument**
* Order book is **in-memory only**
* No persistence or concurrency handling

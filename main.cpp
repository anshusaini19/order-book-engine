#include <iostream>
#include <map>
#include <deque>
#include <unordered_map>
#include <sstream>
#include <string>

using namespace std;

struct Order {
    string id;
    string side;
    double price;
    int quantity;
};

class OrderBook {

private:

    // BUY side (highest price first)
    map<double, deque<Order>, greater<double> > bids;

    // SELL side (lowest price first)
    map<double, deque<Order> > asks;

    // Order lookup for cancellation
    unordered_map<string, pair<string,double> > orderIndex;

public:

    void processOrder(Order order) {

        if(order.side == "BUY")
            matchBuy(order);
        else
            matchSell(order);
    }

    void matchBuy(Order order) {

        while(order.quantity > 0 && !asks.empty()) {

            map<double, deque<Order> >::iterator bestAsk = asks.begin();

            if(order.price != 0 && bestAsk->first > order.price)
                break;

            Order &sellOrder = bestAsk->second.front();

            if(order.id == sellOrder.id)
                break;

            int tradeQty = min(order.quantity, sellOrder.quantity);
            double tradePrice = bestAsk->first;

            cout << "TRADE "
                 << order.id << " "
                 << sellOrder.id << " "
                 << tradePrice << " "
                 << tradeQty << endl;

            order.quantity -= tradeQty;
            sellOrder.quantity -= tradeQty;

            if(sellOrder.quantity == 0) {

                orderIndex.erase(sellOrder.id);
                bestAsk->second.pop_front();

                if(bestAsk->second.empty())
                    asks.erase(bestAsk);
            }
        }

        // Remaining quantity goes to book if LIMIT order
        if(order.quantity > 0 && order.price != 0) {

            bids[order.price].push_back(order);
            orderIndex[order.id] = make_pair("BUY", order.price);
        }
    }

    void matchSell(Order order) {

        while(order.quantity > 0 && !bids.empty()) {

            map<double, deque<Order>, greater<double> >::iterator bestBid = bids.begin();

            if(order.price != 0 && bestBid->first < order.price)
                break;

            Order &buyOrder = bestBid->second.front();

            if(order.id == buyOrder.id)
                break;

            int tradeQty = min(order.quantity, buyOrder.quantity);
            double tradePrice = bestBid->first;

            cout << "TRADE "
                 << buyOrder.id << " "
                 << order.id << " "
                 << tradePrice << " "
                 << tradeQty << endl;

            order.quantity -= tradeQty;
            buyOrder.quantity -= tradeQty;

            if(buyOrder.quantity == 0) {

                orderIndex.erase(buyOrder.id);
                bestBid->second.pop_front();

                if(bestBid->second.empty())
                    bids.erase(bestBid);
            }
        }

        if(order.quantity > 0 && order.price != 0) {

            asks[order.price].push_back(order);
            orderIndex[order.id] = make_pair("SELL", order.price);
        }
    }

    void cancelOrder(string id) {

        if(orderIndex.find(id) == orderIndex.end())
            return;

        string side = orderIndex[id].first;
        double price = orderIndex[id].second;

        if(side == "BUY") {

            deque<Order> &dq = bids[price];

            for(deque<Order>::iterator it = dq.begin(); it != dq.end(); it++) {

                if(it->id == id) {

                    dq.erase(it);
                    break;
                }
            }

            if(dq.empty())
                bids.erase(price);
        }
        else {

            deque<Order> &dq = asks[price];

            for(deque<Order>::iterator it = dq.begin(); it != dq.end(); it++) {

                if(it->id == id) {

                    dq.erase(it);
                    break;
                }
            }

            if(dq.empty())
                asks.erase(price);
        }

        orderIndex.erase(id);
    }

    void printBook() {

        cout << "--- Book ---" << endl;

        int count = 0;

        for(map<double, deque<Order> >::iterator it = asks.begin(); it != asks.end(); it++) {

            if(count == 5) break;

            double price = it->first;
            deque<Order> &orders = it->second;

            int total = 0;

            for(size_t i = 0; i < orders.size(); i++)
                total += orders[i].quantity;

            cout << "ASK: " << price << " x " << total << endl;

            count++;
        }

        if(asks.empty())
            cout << "ASK: (empty)" << endl;

        count = 0;

        for(map<double, deque<Order>, greater<double> >::iterator it = bids.begin(); it != bids.end(); it++) {

            if(count == 5) break;

            double price = it->first;
            deque<Order> &orders = it->second;

            int total = 0;

            for(size_t i = 0; i < orders.size(); i++)
                total += orders[i].quantity;

            cout << "BID: " << price << " x " << total << endl;

            count++;
        }

        if(bids.empty())
            cout << "BID: (empty)" << endl;
    }
};

int main() {

    OrderBook book;
    string line;

    while(getline(cin, line)) {

        if(line.empty())
            continue;

        stringstream ss(line);
        string first;

        ss >> first;

        if(first == "CANCEL") {

            string id;
            ss >> id;

            book.cancelOrder(id);
        }
        else {

            Order order;

            order.id = first;
            ss >> order.side >> order.price >> order.quantity;

            book.processOrder(order);
        }
    }

    book.printBook();

    return 0;
}
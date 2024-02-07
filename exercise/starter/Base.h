// ************************
// IMPORTANT. You will not need to read this. All the information you need should be in the document,
// and if you have questions, don't hesitate to ask your interviewer.
// ************************

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

// Each offer across all farmers has a unique refNum.
struct OfferID {
    uint32_t refNum; // This is unique across all offers
    array<char, 4> farmerID;

    OfferID(char const* b = "??", uint32_t r = -1) : refNum(r), farmerID{} {
        strncpy(farmerID.data(), b, 4);
    }

    // ignore IDs for simplicity
    bool operator==(OfferID rhs) const { return refNum == rhs.refNum; }
    bool operator <(OfferID rhs) const { return refNum <  rhs.refNum; }
};
ostream& operator<<(ostream& os, OfferID oid);

struct Offer {
    unsigned price = 0;

    // The maximum number of apples visible in the offers list.
    // If this is less than qty, the offer is an iceberg.
    const unsigned displayedQtyMax = 0;

    // The following variables are mutable, because Offer is a set key,
    // and set keys are const, but we need to successively adjust the quantity
    // upon partial executions.

    // The total number of apples (still) comprising this offer.
    mutable unsigned qty = 0;
    // displayedQty is the subset of apples in qty that is currently
    // visible on the offer list. I.e. displayedQty <= qty always holds.
    mutable unsigned displayedQty = 0;
    // The globally unique offer identifier.
    mutable OfferID oid;

    // Populates displayedQty from qty. Returns true if successful.
    bool replenish() const {
        assert(displayedQty == 0);
        displayedQty = min(qty, displayedQtyMax);
        return displayedQty > 0;
    }

    Offer(unsigned p, unsigned qy, unsigned displayed = -1)
        : price(p), displayedQtyMax(displayed), qty(qy),
          displayedQty(min(displayedQtyMax, qty)) {}

    bool operator<(const Offer& r) const {
        if (price == r.price) {
            // Older entries are preferred when prices are equal
            return oid.refNum > r.oid.refNum;
        }
        return price > r.price;
    }
};

using OfferList = set<Offer>;

void print(const OfferList& b);

// Forward declaration
using ConsolList = vector<struct PublicOffer>;

class MatchingEngine {
    static unsigned nextRefNum;
    string farmerID = "??";
    ConsolList& cl;
    OfferList offers;

public:

    MatchingEngine(const char* ID,
                   ConsolList& c) : farmerID(ID), cl(c) {}

    // Specify addToCL=false for special stress testing where
    // you populate the ConsolList yourself
    void addOffer(Offer o, bool addToCL = true);
    // Performs a purchase.
    void purchase(unsigned price, unsigned qty);

    void print() const { ::print(offers); }

    const char* getFarmerID() const { return farmerID.c_str(); }
    const OfferList& getList() const { return offers; }
};